#ifndef _WARC_H_
#define _WARC_H_

#define BUFFSIZE 70000000
#define min(a,b) a < b ? a : b

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <json-c/json.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include "errors.h"
#include "gzip.h"
#include "dtd.h"
#include "dist.h"
#include "hash.h"
#include "str_hash.h"


typedef enum { warcinfo, response, request, metadata } record_type_t;

typedef struct warc_t {
  record_type_t type;
  size_t buffer_size;
  char *path, *buffer, *tmpbuffer, *token, str_out_dir[500], str_previous[500];
  FILE *file;
  int response;
  int metadata;
  uint32_t threshold;
  int content_json;
  int out_dir;
  int previous;
  hash_t *universe;
  str_hash_t *dict;
  int universe_count;
} warc_t;

typedef struct files_compare{
  char *f1;
  char *f2;
  char *f2_json;
} files_compare;

typedef struct pthread_params{
   int i;
   int i_bound;
   pthread_t thread;
   files_compare *files;
} pthread_params;

void skip_bytes(warc_t *warc, size_t dataToRead);
warc_t *warc_new(int threshold, int content_json, int out_dir, char *path);
void warc_free(warc_t *warc);
int warc_next_response(warc_t *warc);
int warc_next_metadata(warc_t *warc);
void add_key_value_to_json(json_object *jobj, char *token, char *buffer);
void parse_header_first_line(warc_t *warc);
void parse_header(warc_t *warc, json_object *jobj);
void add_xml_dtd_errors(json_object *jobj, char *dir_file, char *dtd_found);
void find_dtd(char *buffer, char **dtd_out);
void check_and_calculate_distance(warc_t *warc, json_object *jobj, char *current_file_path, char *filename);
void calculate_two_days_distance(warc_t *warc);
void update_json_file(char *json, char *key, double value);
void runs_distance(files_compare *files, int size, int n_threads);
void *run_thread(void *p);

#endif
