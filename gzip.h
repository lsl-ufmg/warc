#ifndef GZIP_H_
#define GZIP_H_

#include <stdlib.h>
#include <zlib.h>
#include <string.h>
#include <openssl/md5.h>

void compact(char *buffer, char *out_buffer, size_t out_size);
void extract(char *buffer, char *out_buffer, size_t in_size);
void md5(char *str, char *out, int length);


#endif
