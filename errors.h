#ifndef _ERRORS_H_
#define _ERRORS_H_

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "colors.h"

void handle_error(const char *, ...);
void check_file(char *);
int check_positive_int(char *arg);

#endif
