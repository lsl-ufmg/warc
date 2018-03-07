#ifndef _MAIN_H_
#define _MAIN_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include "warc.h"
#include "errors.h"

typedef struct args_t {
  int dir;
  int threshold;
  int json;
  int distance_warc;
  char *path;
  int meta;
} args_t;


void parse_args(int argc, char **argv, args_t *args);
void args_free(args_t *args);
void usage();
#endif
