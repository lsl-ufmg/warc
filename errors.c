#include "errors.h"

void handle_error(const char *fmt, ...){
  va_list ap;

  printf(ANSI_COLOR_RED);

  va_start(ap, fmt);
  vprintf(fmt, ap);
  va_end(ap);

  printf(ANSI_COLOR_RESET);

  printf("\n");

  exit(-1);
}

int check_positive_int(char *arg){
  char *endptr;
  int ret;
  ret = (int) strtol(arg, &endptr, 10);

  if(*endptr != '\0' || ret < 0){
    return -1;
  }

  return ret;
}

void check_file(char *file){
  if(access(file, R_OK) == -1) {
    handle_error("File '%s' does not exist", file);
  }
}
