#ifndef _STR_HASH_H_
#define _STR_HASH_H_

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "khash.h"

KHASH_MAP_INIT_STR(str, char *)

typedef khash_t(str) str_hash_t;

typedef void (*hash_foreach_fn)(const char *, char *, void *);
typedef int (*hash_foreach_rm_fn)(const char *, char *, void *);

str_hash_t *str_hash_new();

void str_hash_free(str_hash_t *);
void str_hash_insert(str_hash_t *, char *, char *);
void str_hash_print(str_hash_t *);
char *str_hash_get(str_hash_t *, char *);

#endif
