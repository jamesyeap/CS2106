#ifndef USERSWAP_H
#define USERSWAP_H

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stddef.h>

void userswap_set_size(size_t size);
void *userswap_alloc(size_t size);
void userswap_free(void *mem);
void *userswap_map(int fd, size_t size);

#endif
