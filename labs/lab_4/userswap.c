#include "userswap.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <math.h>
#include <sys/types.h>
#include <string.h>
#include <sys/queue.h>
#include <semaphore.h>
#include <fcntl.h>

/* **************************************************************************************************** MACROS ********************* */
/* -------- MACROS for PAGE-TABLE -------------------------------------------------------------------------------------------------- */
#define PAGE_SIZE 4096 // size of a page is 4096 bytes
#define PRESENT 1
#define NOT_PRESENT 0
#define DIRTY 1
#define CLEAN 0

/* -------- MACROS for SWAP-FILE TABLE --------------------------------------------------------------------------------------------- */
#define FREE 0
#define OCCUPIED 1

/* *************************************************************************************************** STRUCTS ********************* */
/* -------- STRUCTS for PAGE-TABLE ------------------------------------------------------------------------------------------------- */
typedef struct PAGE_TABLE_ENTRY {
  pid_t pid;            // process-id
  int page_number;      // page number (only unique to the current process)

  void* base_addr;      // pointer to the base of the Controlled Memory Region allocated to this process
  size_t len;           // size of the Controlled Memory Region allocated to this process (in bytes)
  int present;          // indicates if the page is memory-resident or not

  int dirty;            // indicates if the page has been written-to or not
  off_t swap_offset;    // indicates which position of the SWAP file to retrieve the data of this page from; 
                        //      is initialised to -1; so if swap_offset=-1, 
                        //      that means this page was NEVER written to a SWAP file

  int backing_fd;       // file-descriptor of the BACKING file
  off_t backing_offset; // indicates which position of the BACKING file to retrieve the data of this page from; 
                        //      is initialised to -1 for userswap_alloc(...) calls; so if backing_offset=-1, 
                        //      that means this page's data is held in the BACKING file
  
  LIST_ENTRY(PAGE_TABLE_ENTRY) page_table_entries;
} PTE;
LIST_HEAD(PAGE_TABLE_HEAD, PAGE_TABLE_ENTRY);

/* -------- STRUCTS for RESIDENT-PAGE TABLE -------------------------------------------------------------------------------------- */
typedef struct RESIDENT_PAGE_ENTRY {
  PTE *entry;

  TAILQ_ENTRY(RESIDENT_PAGE_ENTRY) resident_page_entries;
} RPE;
TAILQ_HEAD(RESIDENT_PAGE_TABLE_HEAD, RESIDENT_PAGE_ENTRY);

/* -------- STRUCTS for SWAP-FILE TABLE ---------------------------------------------------------------------------------------- */
typedef struct SWAP_BLOCK_ENTRY {
  off_t offset; // the offset to the starting-location of the block in the swap-file
  int free;     // indicates if this block is free: if this block is free, this means that byte-locations: [pos, pos+4095] within the swap-file is available for writing!

  TAILQ_ENTRY(SWAP_BLOCK_ENTRY) swap_block_entries;
} SBE;
TAILQ_HEAD(SWAP_BLOCK_TABLE_HEAD, SWAP_BLOCK_ENTRY);

typedef struct SWAP_FILE_ENTRY { /* STRUCT CONTAINING META-DATA FOR A SWAP-FILE OF A SPECIFIC PROCESS */
  pid_t pid;  // which process does this swap-file belong to
  int fd;     // file-descriptor of the swap-file; swap-file remains OPEN all the time

  struct SWAP_BLOCK_TABLE_HEAD swap_block_table;    // a linked-list that holds the data of various blocks in the swap file
  struct SWAP_BLOCK_TABLE_HEAD *swap_block_table_p;

  LIST_ENTRY(SWAP_FILE_ENTRY) swap_file_entries;
} SFE;
LIST_HEAD(SWAP_FILE_TABLE_HEAD, SWAP_FILE_ENTRY);

/* ************************************************************************************ FUNCTION-DECLARATIONS **************** */
/* -------- FUNCTION-DECLARATIONS for GENERAL -------------------------------------------------------------------------------- */
size_t round_up(size_t size);
void assert_mmap_succeeded(void *addr);
void assert_sigaction_succeeded(int success);
void assert_mprotect_succeeded(int success);
void sig_handler(int sig, siginfo_t *info, void *ucontext);
void pagefault_handler(siginfo_t *info);
void evict_page(PTE *entry);

/* -------- FUNCTION-DECLARATIONS for PAGE-TABLE ---------------------------------------------------------------------------- */
void add_PTE(pid_t pid, int page_number, void *addr, size_t len);
void add_PTE_mapped(pid_t pid, int page_number, void *base_addr, size_t len, int backing_fd, off_t offset);
PTE *get_PTE_faulting_addr(void *fault_addr);
void delete_PTE_for_process(pid_t pid);
void assert_munmap_succeeded(int success);

/* -------- FUNCTION-DECLARATIONS for RESIDENT-PAGE TABLE ------------------------------------------------------------------- */
void enqueue_resident_page(PTE *entry);
PTE *dequeue_resident_page();
void remove_resident_pages(pid_t pid);

/* -------- FUNCTION-DECLARATIONS for SWAP-FILE TABLE ----------------------------------------------------------------------- */
int create_swap_file(pid_t pid);
SFE *add_SFE(pid_t pid, int fd);
SFE *get_SFE(pid_t pid);
void delete_SFE(pid_t pid);
SBE *get_free_block(SFE *swap_file_entry);
SBE *create_block(SFE *swap_file_entry);
void free_block_at_offset(SFE *swap_file_entry, off_t offset);
void free_block(SBE *entry);
void occupy_block(SBE *entry);
void delete_swap_block_table(SFE *swap_file_entry);

/* ********************************************************************************* GLOBAL VARIABLES ********************* */
/* -------- GLOBAL-VARIABLES for GENERAL ---------------------------------------------------------------------------------- */
size_t LORM = 8626176; // Limit of Resident Memory (LORM) -> default size is 8,626,176 bytes -> 2,106 pages
size_t RM_IN_USE = 0;  //  0 bytes of resident-memory are used initially

/* -------- GLOBAL-VARIABLES for PAGE-TABLE ------------------------------------------------------------------------------- */
struct PAGE_TABLE_HEAD page_table; // A global linked-list representing an Inverted Page-Table
struct PAGE_TABLE_HEAD *page_table_p;

/* -------- GLOBAL-VARIABLES for RESIDENT-PAGE TABLE ---------------------------------------------------------------------- */
struct RESIDENT_PAGE_TABLE_HEAD resident_page_table; // A global queue (FIFO) representing the order in which Page-Table-Entries are made resident
struct RESIDENT_PAGE_TABLE_HEAD *resident_page_table_p;

/* -------- GLOBAL-VARIABLES for SWAP-FILE TABLE -------------------------------------------------------------------------- */
struct SWAP_FILE_TABLE_HEAD swap_file_table; // A global linked-list representing the free-locations in the swap-files for each process
struct SWAP_FILE_TABLE_HEAD *swap_file_table_p;

/* ***************************************************************************** FUNCTION-DEFINITIONS ********************* */
/* -------- FUNCTION-DEFINITIONS for GENERAL ------------------------------------------------------------------------------ */
void userswap_set_size(size_t size) {
  // if the new size is smaller than the existing size,
  //    then we need to evict some pages!
  if (size < LORM) {
    LORM = size; // update the new size

    PTE *page_to_evict = dequeue_resident_page();
    while (RM_IN_USE > LORM && page_to_evict != NULL) {      
      evict_page(page_to_evict);

      if (RM_IN_USE > LORM) {
        page_to_evict = dequeue_resident_page();
      }
    }

  }
}

void *userswap_alloc(size_t size) {
  /* ---- install SIGSEGV handler if not yet done ---- */
  struct sigaction act;  
  sigemptyset(&act.sa_mask);
  act.sa_sigaction = sig_handler;
  act.sa_flags = SA_SIGINFO;

  int sigaction_success = sigaction(
    SIGSEGV, 
    &act,
    NULL
  );
  assert_sigaction_succeeded(sigaction_success);

  /* ---- request memory-space from kernel ----*/
  void* addr = mmap(
    NULL,                               //  addr
    round_up(size),                     //  length (in bytes)
    PROT_NONE,                          //  prot
    MAP_ANONYMOUS | MAP_PRIVATE,       //  flags
    -1,                                //  fd
    0                                   //  offset (in bytes)
  );
  assert_mmap_succeeded(addr);

  /* ---- initialize Page-Table Entries and add them to the Page-Table ---- */
  long int base_addr_decimal = (long int)addr;  
  void *page_ptr;
  int n_PTE_needed = round_up(size) / PAGE_SIZE;
  pid_t pid = getpid();

  for (int i=0; i<n_PTE_needed; i++) {
    page_ptr = (void *)( base_addr_decimal + (i * PAGE_SIZE) ); // for each 4096-byte block of memory, create a Page-Table Entry
    add_PTE(pid, i, page_ptr, PAGE_SIZE);

    // printf("added new Page-Table Entry for process: %d\n", getpid()); /* <---- FOR DEBUGGING ----> */
  }

  return addr;
}

void userswap_free(void *mem) {
  PTE *entry = get_PTE_faulting_addr(mem);

  remove_resident_pages(entry->pid);

  SFE *swap_file_entry = get_SFE(entry->pid);
  if (swap_file_entry != NULL) {
    delete_swap_block_table(get_SFE(entry->pid));
  }

  delete_SFE(entry->pid);
  delete_PTE_for_process(entry->pid);
}

void *userswap_map(int fd, size_t size) {
  /* ---- install SIGSEGV handler if not yet done ---- */
  struct sigaction act;  
  sigemptyset(&act.sa_mask);
  act.sa_sigaction = sig_handler;
  act.sa_flags = SA_SIGINFO;

  int sigaction_success = sigaction(
    SIGSEGV, 
    &act,
    NULL
  );

  assert_sigaction_succeeded(sigaction_success);

  /* ---- request memory-space from kernel ----*/

  void* addr = mmap(
    NULL,                               //  addr
    round_up(size),                     //  length (in bytes)
    PROT_NONE,                          //  prot
    MAP_ANONYMOUS | MAP_PRIVATE,        //  flags
    -1,                                 //  fd
    0                                   //  offset (in bytes)
  );

  assert_mmap_succeeded(addr);

  /* ---- initialize Page-Table Entries and add them to the Page-Table ---- */
  long int base_addr_decimal = (long int)addr;  
  void *page_ptr;
  off_t backing_offset;
  int n_PTE_needed = round_up(size) / PAGE_SIZE;
  pid_t pid = getpid();

  for (int i=0; i<n_PTE_needed; i++) {
    page_ptr = (void *)( base_addr_decimal + (i * PAGE_SIZE) ); // for each 4096-byte block of memory, create a Page-Table Entry
    backing_offset = (off_t)( i * PAGE_SIZE );
    add_PTE_mapped(pid, i, page_ptr, PAGE_SIZE, fd, backing_offset);

    // printf("added new Page-Table Entry for process: %d\n", getpid()); /* <---- FOR DEBUGGING ----> */
  }

  return addr;
}

size_t round_up(size_t size) {
  double multiple = ceil(size / PAGE_SIZE);
  size_t result = (size_t)(multiple * PAGE_SIZE); // cast to size_t

  return result;
}

void assert_mmap_succeeded(void *addr) {
  if (addr == MAP_FAILED) {
    fprintf(stderr, "userswap_alloc encountered an error: %d\n", errno);
    exit(EXIT_FAILURE);    
  }
}

void assert_mprotect_succeeded(int success) {
  if (success != 0) {
    fprintf(stderr, "mprotect encountered an error: %d\n", errno);
    exit(EXIT_FAILURE);
  }
}

void assert_sigaction_succeeded(int success) {
  if (success != 0) {
    fprintf(stderr, "handler for SIGSEGV could not be installed!\n");
    exit(EXIT_FAILURE);
  }
}

void assert_write_succeeded(int bytes_written) { 
  if (bytes_written == -1) {
    fprintf(stderr, "data could not be written!\n");
    exit(EXIT_FAILURE);
  } else {
    // printf("%d bytes successfully written!\n", bytes_written); /* <--- FOR DEBUGGING ----> */
  }
}

void assert_madvise_succeeded(int success) {
  if (success != 0) {
    fprintf(stderr, "madvise encountered an error: %d!\n", errno);
    exit(EXIT_FAILURE);
  }
}

void sig_handler(int sig, siginfo_t *info, void *ucontext) {
  //  call page-fault handler 
  //    need the address to perform "mprotect", which is found in siginfo_t
  pagefault_handler(info);
}

void pagefault_handler(siginfo_t *info) {
  PTE *entry = get_PTE_faulting_addr(info->si_addr);

  if (entry == NULL) {
    signal(SIGSEGV, SIG_DFL); // reset SIGSEGV handler to the default handler used for SIGSEGV
    raise(SIGSEGV); // cause an actual Segmentation-Fault

    return;         // don't do anything else
  }

  if (entry->present == NOT_PRESENT) { /* ----------- FIRST page-fault ----------- */

    // if not enough resident-memory, evict a Page to make space for this one
    if (RM_IN_USE >= LORM) {
      PTE *page_to_evict = dequeue_resident_page();    

      evict_page(page_to_evict);
    }

    // check if any data has to be loaded into the page from the BACKING file
    if (entry->backing_offset != -1) {
      int mprotect_success = mprotect(
        entry->base_addr,  //  addr
        PAGE_SIZE,         //  len
        PROT_WRITE         //  prot    
      );

      assert_mprotect_succeeded(mprotect_success);

      int bytes_written = pread(
        entry->backing_fd,
        entry->base_addr,
        PAGE_SIZE,
        entry->backing_offset
      );

      assert_write_succeeded(bytes_written);
    }

    // check if any data has to be loaded into the page from the SWAP file
    if (entry->swap_offset != -1) {
      int mprotect_success = mprotect(
        entry->base_addr,  //  addr
        PAGE_SIZE,         //  len
        PROT_WRITE         //  prot    
      );

      assert_mprotect_succeeded(mprotect_success);

      SFE *swap_file_entry = get_SFE(entry->pid);

      int bytes_written = pread(
        swap_file_entry->fd,
        entry->base_addr,
        PAGE_SIZE,
        entry->swap_offset
      );

      assert_write_succeeded(bytes_written);

      // now that the data has been re-written into the physical frame from secondary-storage,
      //    the block inside the swap-file can be used for other pages
      free_block_at_offset(swap_file_entry, entry->swap_offset);
      entry->swap_offset = -1;
    }

    // make the page containing the accessed memory PROT_READ, thereby making the page memory-resident
    int mprotect_success = mprotect(
      entry->base_addr,  //  addr
      PAGE_SIZE,         //  len
      PROT_READ          //  prot    
    );

    assert_mprotect_succeeded(mprotect_success);    

    entry->present = PRESENT; // update the Page-Table Entry
    enqueue_resident_page(entry); // enqueue it so that we know the order of page-eviction for later (FIFO)
    RM_IN_USE += PAGE_SIZE;   // update the total-page size in use

  } else {                           /* ----------- SECOND page-fault ----------- */

    // give the page; which is already memory-resident, but only has read-access, write access as well, thereby making the page "dirty"
    int mprotect_success = mprotect(
      entry->base_addr,          //  addr
      PAGE_SIZE,                 //  len
      PROT_READ | PROT_WRITE     //  prot    
    );

    assert_mprotect_succeeded(mprotect_success);       

    entry->dirty = DIRTY; // update the Page-Table Entry
  }

  // printf("page successfully made memory-resident for %p\n", page_addr); // <----- FOR DEBUGGING ----->
}

// evicts the page located inside the Page-Table Entry:
void evict_page(PTE *entry) {
  // printf("page %d being evicted\n", entry->page_number);

  if (entry->dirty == DIRTY) { /* if the page is DIRTY */
    if (entry->backing_offset != -1) { // if the page is mapped to a BACKING file, save the changes back to the BACKING file
      int bytes_written = pwrite(
        entry->backing_fd,
        entry->base_addr,
        PAGE_SIZE,
        entry->backing_offset
      );

      assert_write_succeeded(bytes_written);
    } else { // if the page is NOT mapped to a BACKING file, save the changes to a SWAP file
      /* -------- create a swap-file for this process (if one doesn't exist) -------- */
      SFE *swap_file_entry = get_SFE(entry->pid);    
      int fd;

      if (swap_file_entry == NULL) {
        fd = create_swap_file(entry->pid);
        swap_file_entry = add_SFE(entry->pid, fd);
      } else {
        fd = swap_file_entry->fd;
      }

      SBE *swap_block_entry = get_free_block(swap_file_entry);

      int bytes_written = pwrite(
        fd,
        entry->base_addr,
        PAGE_SIZE,
        swap_block_entry->offset
      );

      assert_write_succeeded(bytes_written);

      entry->swap_offset = swap_block_entry->offset; // record where in the swap-file the flushed data is located at
    }

    int mprotect_success = mprotect(
      entry->base_addr,  //  addr
      PAGE_SIZE,         //  len
      PROT_NONE          //  prot    
    );

    assert_mprotect_succeeded(mprotect_success);

    int madvise_success = madvise(
      entry->base_addr,
      PAGE_SIZE,
      MADV_DONTNEED
    );

    assert_madvise_succeeded(madvise_success);

    entry->dirty = CLEAN; // update the Page-Table Entry as clean now that its contents are safely stored in the swap file

  } else { /* if the page is CLEAN */
      int mprotect_success = mprotect(
        entry->base_addr,  //  addr
        PAGE_SIZE,         //  len
        PROT_NONE          //  prot    
      );

      assert_mprotect_succeeded(mprotect_success);

      int madvise_success = madvise(
        entry->base_addr,
        PAGE_SIZE,
        MADV_DONTNEED
      );

      assert_madvise_succeeded(madvise_success);      
  }

  entry->present = NOT_PRESENT; // update the Page-Table Entry as NON-MEMORY RESIDENT now that its PROT_NONE
  RM_IN_USE -= PAGE_SIZE;       // update the new number of bytes of resident-memory in use now that this page has been evicted
}

/* ---- FUNCTION-DEFINITIONS for PAGE-TABLE ------------------------------------------------------------ */
void add_PTE(pid_t pid, int page_number, void *base_addr, size_t len) {
  // initialize a new Page-Table Entry
  PTE *entry = malloc(sizeof(PTE));
  entry->pid=pid; entry->page_number=page_number; entry->base_addr=base_addr; entry->len=len; 
  entry->present=NOT_PRESENT; entry->dirty=CLEAN; entry->swap_offset=-1; entry->backing_offset=-1;

  // initialise the Page-Table if it doesn't exist
  if (page_table_p == NULL) {
    LIST_INIT(&page_table);
    page_table_p = &page_table;
  }

  // insert the Page-Table Entry into the Page-Table
  LIST_INSERT_HEAD(&page_table, entry, page_table_entries);
}

void add_PTE_mapped(pid_t pid, int page_number, void *base_addr, size_t len, int backing_fd, off_t offset) {
  // initialize a new Page-Table Entry
  PTE *entry = malloc(sizeof(PTE));
  entry->pid=pid; entry->page_number=page_number; entry->base_addr=base_addr; 
  entry->len=len; entry->present=NOT_PRESENT; entry->dirty=CLEAN; entry->swap_offset=-1; 
  entry->backing_fd=backing_fd; entry->backing_offset=offset;

  // initialise the Page-Table if it doesn't exist
  if (page_table_p == NULL) {
    LIST_INIT(&page_table);
    page_table_p = &page_table;
  }

  // insert the Page-Table Entry into the Page-Table
  LIST_INSERT_HEAD(&page_table, entry, page_table_entries);
}

// gets the Page-Table Entry that contains the page which holds the given faulting-address
//    return NULL
PTE *get_PTE_faulting_addr(void *fault_addr) {
  long int fault_addr_decimal = (long int)(fault_addr);
  long int base_addr_decimal;
  long int max_addr_decimal;

  PTE *entry = LIST_FIRST(&page_table);

  while (entry != NULL) {
    base_addr_decimal = (long int)(entry->base_addr);
    max_addr_decimal = base_addr_decimal + PAGE_SIZE;
    
    if ( (base_addr_decimal <= fault_addr_decimal) && (fault_addr_decimal < max_addr_decimal) ) { // if its within range for a certain PTE, then this is the correct Page-Table Entry!
      return entry;
    } else {
      entry = LIST_NEXT(entry, page_table_entries);
    }
  }

  // printf("%ld | %ld <---> %ld\n", fault_addr_decimal, base_addr_decimal, max_addr_decimal); /* <--- FOR DEBUGGING ---> */

  return NULL;
}

// TEARDOWN: deletes all Page-Table Entries that belong to the given process
void delete_PTE_for_process(pid_t pid) {
  PTE *entry = LIST_FIRST(&page_table);
  PTE *next_entry;

  while (entry != NULL) {
    next_entry = LIST_NEXT(entry, page_table_entries);

    if (entry->pid == pid) {
      // if the page is mapped to a section of a BACKING file, write any unsaved changes back into it
      if (entry->backing_offset != -1 && entry->dirty == DIRTY) {
        int bytes_written = pwrite(
          entry->backing_fd,
          entry->base_addr,
          PAGE_SIZE,
          entry->backing_offset
        );

        if (bytes_written == -1) {
          fprintf(stderr, "data could not be written!\n");
          exit(EXIT_FAILURE);
        }
      }

      LIST_REMOVE(entry, page_table_entries);
      // printf("[TEARDOWN] Removed Page-Table Entry for Process %d\n", pid); /* <--- FOR DEBUGGING ---> */
      
      int munmap_success = munmap(entry->base_addr, PAGE_SIZE);

      assert_munmap_succeeded(munmap_success);

      free(entry);
    }

    entry = next_entry;
  }
}

void assert_munmap_succeeded(int success) {
  if (success != 0) {
    fprintf(stderr, "munmap encountered an error: %d\n", errno);
    exit(EXIT_FAILURE);
  }
}

/* ---- FUNCTION-DEFINITIONS for RESIDENT-PAGE TABLE ------------------------------------------------------------ */
// adds pointer to the Page-Table Entry that was made memory-resident
//    to the BACK of the linked-list (FIFO)
void enqueue_resident_page(PTE *entry) {
  // initialise the Resident-Page Table if it doesn't exist
  if (resident_page_table_p == NULL) {
    TAILQ_INIT(&resident_page_table);
    resident_page_table_p = &resident_page_table;
  }

  RPE *new_rp = malloc(sizeof(RPE));
  new_rp->entry = entry;

  TAILQ_INSERT_TAIL(&resident_page_table, new_rp, resident_page_entries);

  // printf("[+] page %d of Process %d made resident!\n", new_rp->entry->page_number, entry->pid); /* <------ FOR DEBUGGING -------> */
}

// gets the Page-Table Entry that was made memory-resident
//    the earliest (from the FRONT of the linked-list - FIFO)
PTE *dequeue_resident_page() {
  RPE *first_RP = TAILQ_FIRST(&resident_page_table);
  PTE *entry = first_RP->entry;

  TAILQ_REMOVE(&resident_page_table, first_RP, resident_page_entries);
  free(first_RP);

  // printf("[-] Page %d of Process %d will be evicted to make space for an incoming page!\n", entry->page_number, entry->pid); /* <------ FOR DEBUGGING -------> */
  
  return entry;
}

// TEARDOWN: removes all resident pages for the given Process
void remove_resident_pages(pid_t pid) {
  RPE *entry = TAILQ_FIRST(&resident_page_table);
  RPE *next_entry;

  while (entry != NULL) {
    next_entry = TAILQ_NEXT(entry, resident_page_entries);

    if (entry->entry->pid == pid) {
      // printf("[TEARDOWN] Resident-Page Entry for Page %d of Process %d deleted!\n", entry->entry->page_number, entry->entry->pid); /* <------ FOR DEBUGGING -------> */

      free(entry);
    }

    entry = next_entry;
  }
}


/* ---- FUNCTION-DEFINITIONS for SWAP-FILE TABLE ------------------------------------------------------------ */
// initialises a new swap file or opens one if it already exists
//    and returns the fd of the file
//    note: each process has a unique swap-file
int create_swap_file(pid_t pid) {
    // format filename to the format: "<pid>.swap"
    char pid_string[37];
    sprintf(pid_string, "%d.swap", pid);

    int fd = open(
      pid_string,         // find OR create a swap file with the format: "<pid>.swap"
      O_RDWR  | O_CREAT | O_TRUNC,   // open the swap file in read and write mode; create it if no swap file exists; if a swap file with the same name exists, overwrite it
      S_IRUSR | S_IWUSR   //    if a swap file is to be created, create it such that ONLY the USER has READ and WRITE permissions
    );

	// printf("Created swap-file for process %d, which can be accessed through fd: %d!\n", pid, fd); /* <----- FOR DEBUGGING ----->*/

    return fd;
}

// init a new Swap-File Entry for the given process
SFE *add_SFE(pid_t pid, int fd) {
  if (swap_file_table_p == NULL) { // initialise the Swap-File Entry Table if it doesn't exist
    LIST_INIT(&swap_file_table);
    swap_file_table_p = &swap_file_table;
  }

  // initialize a new Swap-File Entry
  SFE *entry = malloc(sizeof(SFE));
  entry->pid=pid; entry->fd=fd;

  // insert the Swap-File Entry into the table
  LIST_INSERT_HEAD(&swap_file_table, entry, swap_file_entries);

  return entry;
}

// gets the Swap-File Entry for the given process
//		and returns NULL if not found
SFE *get_SFE(pid_t pid) {
	SFE *entry;

	LIST_FOREACH(entry, &swap_file_table, swap_file_entries) {
		if (entry->pid == pid) {
			// printf("Swap-File Entry for process %d FOUND! (Swap-file can be accessed through fd: %d)\n", pid, entry->fd); /* <----- FOR DEBUGGING ----->*/

			return entry;
		}
	}

	// printf("Swap-File Entry for process %d COULD NOT BE FOUND!\n", pid); /* <----- FOR DEBUGGING ----->*/

	return NULL;
}

// TEARDOWN: delete Swap-File Entry for the given process
//      NOTE: the Swap-File will still exist -> not deleted
void delete_SFE(pid_t pid) {
  SFE *entry = get_SFE(pid);

  if (entry != NULL) {
    LIST_REMOVE(entry, swap_file_entries);

	// printf("[TEARDOWN] Swap-File Entry for process %d DELETED!\n", pid); /* <----- FOR DEBUGGING ----->*/

    free(entry);
  }
}

// gets a free block from the swap-file at the given Swap-File Entry
//		creates a new block if not found
SBE *get_free_block(SFE *swap_file_entry) { // initialise the Swap-Block Entry Table if it doesn't exist for the given Swap-File
	if (swap_file_entry->swap_block_table_p == NULL) {
		TAILQ_INIT(&swap_file_entry->swap_block_table);
    	swap_file_entry->swap_block_table_p = &swap_file_entry->swap_block_table;
	}

	SBE *entry;
	TAILQ_FOREACH(entry, &swap_file_entry->swap_block_table, swap_block_entries) {
		if (entry->free == FREE) {
			// printf("Found an empty block inside swap-file for process %d at offset: %ld\n", swap_file_entry->pid, entry->offset); /* <--- FOR DEBUGGING ---> */			
			occupy_block(entry);

			return entry;
		}
	}

	// printf("No empty blocks inside swap-file for process %d found!\n", swap_file_entry->pid); /* <--- FOR DEBUGGING ---> */
	entry = create_block(swap_file_entry);

	occupy_block(entry);

	return entry;
}

// creates a new empty block in the swap-file at the given Swap-File Entry
SBE *create_block(SFE *swap_file_entry) {
	SBE *entry = malloc(sizeof(SBE));
	entry->free = FREE;

	SBE *last_block_entry = TAILQ_LAST(&swap_file_entry->swap_block_table, SWAP_BLOCK_TABLE_HEAD);
	if (last_block_entry == NULL) {
		entry->offset = 0;
	} else {
		entry->offset = last_block_entry->offset + PAGE_SIZE;
	}

  TAILQ_INSERT_TAIL(&swap_file_entry->swap_block_table, entry, swap_block_entries);

  return entry;
}

// frees an occupied block in the swap-file at the given Swap-File Entry
void free_block(SBE *entry) {
	entry->free = FREE;

	// printf("Block at offset %ld is now FREE\n", entry->offset); /* <--- FOR DEBUGGING ---> */
}

void free_block_at_offset(SFE *swap_file_entry, off_t offset) {
	SBE *entry;

	TAILQ_FOREACH(entry, &swap_file_entry->swap_block_table, swap_block_entries) {
		if (entry->offset == offset) {
			free_block(entry);
		}
	}
}

// occupies a free block in the swap-file at the given Swap-File Entry
void occupy_block(SBE *entry) {
	entry->free = OCCUPIED;

	// printf("Block at offset %ld is now OCCUPIED\n", entry->offset); /* <--- FOR DEBUGGING ---> */
}

// TEARDOWN: deletes the entire Swap-Block Table at the given Swap-File Entry
void delete_swap_block_table(SFE *swap_file_entry) {
	SBE *entry = TAILQ_FIRST(&swap_file_entry->swap_block_table);
	SBE *next_entry;

	while(entry != NULL) {
		next_entry = TAILQ_NEXT(entry, swap_block_entries);

		// printf("[TEARDOWN] Swap-Block Entry with offset %ld deleted!\n", entry->offset); /* <--- FOR DEBUGGING ---> */
		free(entry);

		entry = next_entry;
	}
}




// valgrind --vex-iropt-register-updates=allregs-at-mem-access ./workload_readonly