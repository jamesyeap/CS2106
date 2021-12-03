#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "You should provide your zip file as a parameter and nothing else!"
    exit 1
fi

if ! [[ "$1" =~ ^E[0-9]{7}.zip$ ]]; then
    echo "zip file is wrongly named: it should be <NUSNET ID>.zip"
    exit 1
fi

echo "Unzipping file: $1"
if [ ! -f "$1" ]; then
    echo "File $1 does not exist."
    exit 1
fi

tmp_folder="$(mktemp -d)"

function cleanup {
    cd /tmp
    rm -rf "$tmp_folder"
}
trap cleanup EXIT

zip_path="$(realpath "$1")"

cd "$tmp_folder"
mkdir unzip; cd unzip
if ! unzip "$zip_path" > /dev/null; then
    echo "Unzipping $1 failed; corrupt file?"
    exit 1
fi

if [ ! -f userswap.c ]; then
    echo "userswap.c missing"
    exit 1
fi

if [ -f bonus_userswap.c ] && [ ! -f bonus_userswap.txt ]; then
    echo "bonus_userswap.c present but bonus_userswap.txt missing"
    exit 1
fi

if [ ! -f bonus_userswap.c ] && [ -f bonus_userswap.txt ]; then
    echo "bonus_userswap.txt present but bonus_userswap.c missing"
    exit 1
fi

mkdir ../compile
mv userswap.c bonus_userswap.c bonus_userswap.txt ../compile 2>/dev/null

if find -type f | grep .; then
    echo "Above extraneous files found."
    exit 1
fi

cd ../compile
cat > userswap.h <<EOF
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
EOF

if ! gcc -g -std=c18 -O2 -c userswap.c; then
    echo "userswap.c compilation failed."
    exit 1
fi

if [ -f bonus_userswap.c ] && ! gcc -g -std=c18 -O2 -c bonus_userswap.c; then
    echo "bonus_userswap.c compilation failed."
    exit 1
fi

echo "Success!"
