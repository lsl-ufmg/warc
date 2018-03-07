#include "str_hash.h"

str_hash_t *
str_hash_new() {
  str_hash_t *str_hash;

  str_hash = kh_init(str);

  return str_hash;
}

int
str_hash_internal_free(const char *key, char *value, void *data) {
  (void) key;
  (void) value;
  (void) data;
  return 1;
}

void
str_hash_internal_remove(str_hash_t *str_hash, const char *key, char *value) {
  khint_t k;

  k = kh_get(str, str_hash, key);
  kh_del(str, str_hash, k);
  free((char *) key);
  free(value);
}

void
str_hash_foreach_remove(str_hash_t *str_hash, hash_foreach_rm_fn fn, void *data) {
  const char *key;
  char *value;

  kh_foreach(
      str_hash,
      key,
      value,
      if(fn(key, value, data))
      str_hash_internal_remove(str_hash, key, value)
      );
}


void
str_hash_free(str_hash_t *str_hash) {
  str_hash_foreach_remove(str_hash, str_hash_internal_free, NULL);

  kh_destroy(str, str_hash);
}

void
str_hash_insert(str_hash_t *str_hash, char *key, char *value) {
  int absent;
  khint_t k;

  k = kh_put(str, str_hash, key, &absent);

  if(absent) {
    kh_key(str_hash, k) = strdup(key);
    kh_value(str_hash, k) = strdup(value);
  }
}

char *
str_hash_get(str_hash_t *str_hash, char *key) {
  khint_t k;

  k = kh_get(str, str_hash, key);
  return (k == kh_end(str_hash) ? NULL : kh_val(str_hash, k));
}

void
str_hash_print_pair(const char *key, char *value, void *data) {
  (void) data;
  printf("  %s => %s\n", key, value);
}

void
str_hash_foreach(str_hash_t *str_hash, hash_foreach_fn fn, void *data) {
  const char *key;
  char *value;

  kh_foreach(str_hash, key, value, fn(key, value, data));
}

void
str_hash_print(str_hash_t *str_hash) {
  printf("{\n");
  str_hash_foreach(str_hash, str_hash_print_pair, NULL);
  printf("}\n");
}
