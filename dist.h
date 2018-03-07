#ifndef _DIST_H
#define _DIST_H

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "errors.h"

#define MAX2(A, B) ((A)>(B) ? (A) : (B))
#define MIN2(A, B) ((A)<(B) ? (A) : (B))
#define MIN3( A, B, C ) ((A) < (B) ? MIN2(A, C) : MIN2(B, C))

int levenshtein(char *s1, char *s2, size_t s1len, size_t s2len);
double calculate_dist_fragments(int file1, int file2, size_t s1, size_t s2, float begin, float end);
int remove_spaces_breaklines(char *in, char **out, size_t s);
double sift4(char *s, char *t, int ss, int st);

#endif
