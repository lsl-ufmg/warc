#ifndef _HASH_H_
#define _HASH_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdint.h>
#include <unistd.h>

#include "khash.h"

KHASH_MAP_INIT_STR(string, uint8_t)

typedef khash_t(string) hash_t;

typedef void (*string_hash_foreach_fn)(const char *, uint8_t, void *);
typedef int (*string_hash_foreach_rm_fn)(const char *, uint8_t, void *);

hash_t* hash_new();
void hash_free(hash_t*);
void hash_insert(hash_t*, char *);
void hash_print(hash_t*);
void hash_foreach(hash_t *, string_hash_foreach_fn, void *);
void hash_foreach_remove(hash_t *, string_hash_foreach_rm_fn, void *);
uint8_t hash_get(hash_t *, char *);
int hash_read_file(hash_t *, const char*);

#endif
