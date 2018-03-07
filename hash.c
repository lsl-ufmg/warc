#include "hash.h"

hash_t *
hash_new() {
  return kh_init(string);
}

int
hash_internal_free(const char *key, uint8_t present, void *data) {
  (void) key;
  (void) present;
  (void) data;
  return 1;
}

void
hash_free(hash_t *hash) {
  hash_foreach_remove(hash, hash_internal_free, NULL);

  kh_destroy(string, hash);
}

void
hash_insert(hash_t *hash, char *key) {
  int absent;
  khint_t k;

  k = kh_put(string, hash, key, &absent);

  if(absent) {
    kh_key(hash, k) = strdup(key);
    kh_value(hash, k) = 1;
  }
}

uint8_t
hash_get(hash_t *hash, char *key) {
  khint_t k;

  k = kh_get(string, hash, key);
  return (k == kh_end(hash) ? 0 : 1);
}

void
hash_print_pair(const char *key, uint8_t present, void *data) {
  (void) data;
  printf("  %s => %u\n", key, present);
}

void
hash_internal_remove(hash_t *hash, const char *key, uint8_t present) {
  khint_t k;
  (void) present;

  k = kh_get(string, hash, key);
  kh_del(string, hash, k);
  free((char *) key);
}

void
hash_foreach_remove(hash_t *hash, string_hash_foreach_rm_fn fn, void *data) {
  uint8_t present;
  const char *key;

  kh_foreach(
      hash,
      key,
      present,
      if(fn(key, present, data))
      hash_internal_remove(hash, key, present)
      );
}

void
hash_foreach(hash_t *hash, string_hash_foreach_fn fn, void *data) {
  uint8_t present;
  const char *key;

  kh_foreach(hash, key, present, fn(key, present, data));
}

size_t
hash_size(hash_t *hash) {
  return kh_size(hash);
}

void
hash_print(hash_t *hash) {
  printf("{\n");
  hash_foreach(hash, hash_print_pair, NULL);
  printf("}\n");
}

int
hash_read_file(hash_t *hash, const char* filename) {
  FILE *file;
  char buffer[1000];
  int count = 0;

  if(access(filename, R_OK) == -1) {
    printf("File '%s' does not exist\n", filename);
    exit(0);
  }

  file = fopen(filename, "r");

  while(fscanf(file, "%999s", buffer) != EOF) {
    hash_insert(hash, buffer);
    count++;
  }

  fclose(file);
  return count;
}
