#ifndef _COLORS_H_
#define _COLORS_H_

#include <stdio.h>
#include <stdarg.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

void printf_red(const char *, ...);
void printf_green(const char *, ...);
void printf_yellow(const char *, ...);
void printf_blue(const char *, ...);
void printf_magenta(const char *, ...);
void printf_cyan(const char *, ...);

#endif
