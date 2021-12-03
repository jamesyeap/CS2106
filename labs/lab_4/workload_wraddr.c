#include "userswap.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;
  const _Bool use_userswap = 1;
  const size_t memory_size = 10 * 1024 * 1024;

  volatile uintptr_t *const mem = use_userswap ? userswap_alloc(memory_size) : malloc(memory_size);
  if (!mem) {
    return 1;
  }

  for (size_t i = 0; i < memory_size / sizeof(uintptr_t); ++i) {
    mem[i] = (uintptr_t)(mem + i);
  }

  _Bool failed = 0;
  for (size_t i = 0; i < memory_size / sizeof(uintptr_t); ++i) {
    if (mem[i] != (uintptr_t)(mem + i)) {
      printf("Failed at %p: %" PRIxPTR "\n", mem + i, mem[i]);
      failed = 1;
    }
  }

  if (use_userswap) {
    userswap_free((void *)mem);
  } else {
    free((void *)mem);
  }

  return failed;
}
