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

  volatile size_t *const mem = use_userswap ? userswap_alloc(memory_size) : malloc(memory_size);
  if (!mem) {
    return 1;
  }

  size_t scratch = 0;
  for (size_t i = 0; i < memory_size / sizeof(size_t); ++i) {
    scratch += mem[i];
  }

  if (use_userswap) {
    userswap_free((void *)mem);
  } else {
    free((void *)mem);
  }

  printf("%zu\n", scratch);

  return 0;
}
