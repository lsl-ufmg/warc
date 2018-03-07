#ifndef _DTD_H_
#define _DTD_H_

#include <libxml/parser.h>
#include <libxml/catalog.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "errors.h"

//headers for w3c validation
void not_used_cb(void *ctx, const char *msg, ...);
void dtd_cb(void *ctx, const char *msg, ...);
void w3c_validation_errors(char *file, char *dtd_str, int *w3c_errors, int *xml_errors);
int parse_xml_file(const char *filename);
void my_xml_error(void *user_data, const char *msg, ...);


struct ParserState {
  int return_val;
};

#endif
