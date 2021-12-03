# Q1
## a. What is a file?
An abstract data type that supports a set of operations; depending on the file's type.

A file contains 2 main things: 
- data: information that is structured in a certain way.
- metadata: additional information about the file: name, size, protection-rights, etc...

## b. Name and describe some classifications of files.
ASCII files, examples:
	- text
	- programming source code

Binary files, examples:
	- executable
	- Java class file
	- pdf


## c. Distinguish between a file type and a file extension.
A file's type determines how its data is stored (using a predefined internal structure), which can only be processed by a specific program; eg JVM for Java class file, a PDF reader for PDF file, etc...

A file's extension merely indicates to some OS what the file's type is (only for Windows; Unix does not care about this) -> example: XXX.pdf (pdf document)

```
head filename

file filename

stat filename
```


## d. What does it mean to open and close a file?
Opening a file:
- ONLY IF IT DOESN'T EXIST, creates a V-node in the System-wide V-node Table; which points to the I-node of the file
- creates an entry in the System-wide Open File Table; which points to the V-node of the file
	- set the offset to the START (offset=0)
- creates a new entry in the Per-process FD Table, which points to the entry in the System-wide Open File Table

Closing a file:
- ONLY IF Ref-count of V-node is 1, removes the V-node from the System-wide V-node Table
	- else, keeps it around as there are still other references to it
- REMOVES the entry in the System-wide Open File Table; even if the Ref-Count is >1

- REMOVES the entry in the Per-process FD Table 

## e. What does it mean to truncate a file?
Suppose a file is truncated to 8 bytes, and currently holds 50 bytes of data, all the data from byte-positions [8, 49] will be deleted.

# Q2

NOTE (NOT COVERED BY THE QUESTION, BUT IMPORTANT): 
To `cd filedir`, `filedir` must have EXECUTE permission in order for you to use it as your current working directory.

## NormDir
### a. Perform "ls –l DDDD".
```
total 8
-rw-------@ 1 jamesyeap1  staff  180 Nov  8  2015 file.txt
```
### b. Change into the directory using "cd DDDD".
OK.
### c. Perform "ls –l".
```
total 8
-rw-------@ 1 jamesyeap1  staff  180 Nov  8  2015 file.txt
```
### d. Perform "cat file.txt" to read the file content.
```
The quick brown fox jumps over the lazy dog.
The quick brown fox jumps over the lazy dog.
The quick brown fox jumps over the lazy dog.
The quick brown fox jumps over the lazy dog.
```
### e. Perform "touch file.txt" to modify the file.
OK.
### f. Perform "touch newfile.txt" to create a new file.
OK.

## ReadExeDir
### a. Perform "ls –l DDDD".
```
total 8
-rw-------@ 1 jamesyeap1  staff  180 Nov  8  2015 file.txt
```
### b. Change into the directory using "cd DDDD".
OK.
### c. Perform "ls –l".
```
total 8
-rw-------@ 1 jamesyeap1  staff  180 Nov  8  2015 file.txt
```
### d. Perform "cat file.txt" to read the file content.
```
The quick brown fox jumps over the lazy dog.
The quick brown fox jumps over the lazy dog.
The quick brown fox jumps over the lazy dog.
The quick brown fox jumps over the lazy dog.
```
### e. Perform "touch file.txt" to modify the file.
OK.
### f. Perform "touch newfile.txt" to create a new file.
```
touch: newfile.txt: Permission denied
```

## WriteExeDir
### a. Perform "ls –l DDDD".
```
ls: : Permission denied
```
### b. Change into the directory using "cd DDDD".
OK.
### c. Perform "ls –l".
```
ls: : Permission denied
```
### d. Perform "cat file.txt" to read the file content.
```
The quick brown fox jumps over the lazy dog.
The quick brown fox jumps over the lazy dog.
The quick brown fox jumps over the lazy dog.
The quick brown fox jumps over the lazy dog.
```
### e. Perform "touch file.txt" to modify the file.
OK.
### f. Perform "touch newfile.txt" to create a new file.
OK.

## ExeOnlyDir
### a. Perform "ls –l DDDD".
```
ls: : Permission denied
```
- understandable because `ls` is kinda like a READ operation, so obviously denied because this directory is EXEC-only.

### b. Change into the directory using "cd DDDD".
OK.

### c. Perform "ls –l".
```
ls: : Permission denied
```
- same as point a.

### d. Perform "cat file.txt" to read the file content.
```
The quick brown fox jumps over the lazy dog.
The quick brown fox jumps over the lazy dog.
The quick brown fox jumps over the lazy dog.
The quick brown fox jumps over the lazy dog.
```
- is okay to read the file since `file.txt` had READ-permission enabled.

### e. Perform "touch file.txt" to modify the file.
OK.

- because you're not modifying the directory -> only the contents of the file that the directory holds the Symbolic Link to.
### f. Perform "touch newfile.txt" to create a new file.
```
touch: newfile.txt: Permission denied
```
- because this directory doesn't have WRITE permission -> hence cannot create a new file within it.


# Q3
Because need to create an entry in the System-wide Open File Table to:
- keep track of the current offset of the PROCESS's file-pointer 
	- if not, if the process stops writing, and then starts writing again -> if you simply start from byte-position 0, the process WILL OVERRIDE EVERYTHING THAT IT HAS PREVIOUSLY WRITTEN!

- if not, incur additional cost of permission checking and path resolution for EVERY read/write

- does not generalise well to file... (pipes and sockets)


# Q4
## a. 
`import java.io.*` in Java.

## b.
The trigger-value must be at least 4096 before the values are printed on screen.

This implies that `printf` implements some form of buffering; only writes to stdout when its internal-buffer is full. This implies that the size of the buffer used by `printf` is 4096 bytes.

When a newline character '\n' is added to the printf() stateement, values are printed on screen regardless of the trigger-value. This implies that the buffer is "flushed" (ie its values written to stdout) whenever a newline character '\n' is encountered.

## c.

1. SET file-pointer to byte-offset 0 // start from the beginning of file
2. INIT bytes_read <- 0

3. WHILE (file-pointer != EOF) && (bytes_read < arraySize):
	3.1 outputArray[bytes_read] <- data at file-pointer
	3.2 bytes_read++;
	3.3 advance the file-pointer to 1 byte-position forward

# Q5

## a.
Non-deterministic string of characters is printed; for example:
RUN 1
```
lc asbgnigt e eytrdo stngbherite  teak,adf avn othAiew einn ogtvr ie fiti y  ssronh bn n ohigning to
```
RUN 2
```
lc a bgnigt g vy idofsing herite  teak,adf avn othAiewseinn oetertre  ttiby  ssronh bn n ohigning to
```

Process A and Process B share the same entry in the System-wide Open File Table; every READ operation advances the file-pointer for the other Process too. 

Combined with the fact that `printf` writes to a buffer internally, the characters read by each Process will only be printed AFTER 50 characters are read.

Try:
```
setbuffer(stdout, NULL); // makes the stdout have no buffer -> this would make the buffer print correctly.
```

## b.
The following output was observed:
```
Alice was beginning to get very tired of sitting bAlice was beginning to get very tired of sitting b
```

Try:
```
setbuffer(stdout, NULL); // makes the stdout have no buffer -> this EXPOSES the interleaving.
```

This makes sense as both Process A and Process B have their OWN entry in the System-wide Open File Table; thus, a READ operation for Process A does not affect the file-pointer for Process B. Hence, the READ order remains intact.


## c.
The following output was observed:
```
Alice was beginning to get very tired of sitting bAlice was beginning to get very tired of sitting b
```

### i.
The following output was observed:
```
y her sister on the bank, and of having nothing tAlice was beginning to get very tired of sitting (base) 
```














































