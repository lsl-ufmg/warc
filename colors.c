#include "colors.h"

void printf_red(const char *fmt, ...) {
  va_list ap;

  printf(ANSI_COLOR_RED);

  va_start(ap, fmt);
  vprintf(fmt, ap);
  va_end(ap);

  printf(ANSI_COLOR_RESET);
}


void printf_green(const char *fmt, ...) {
  va_list ap;

  printf(ANSI_COLOR_GREEN);

  va_start(ap, fmt);
  vprintf(fmt, ap);
  va_end(ap);

  printf(ANSI_COLOR_RESET);
}

void printf_yellow(const char *fmt, ...) {
  va_list ap;

  printf(ANSI_COLOR_YELLOW);

  va_start(ap, fmt);
  vprintf(fmt, ap);
  va_end(ap);

  printf(ANSI_COLOR_RESET);
}

void printf_blue(const char *fmt, ...) {
  va_list ap;

  printf(ANSI_COLOR_BLUE);

  va_start(ap, fmt);
  vprintf(fmt, ap);
  va_end(ap);

  printf(ANSI_COLOR_RESET);
}

void printf_magenta(const char *fmt, ...) {
  va_list ap;

  printf(ANSI_COLOR_MAGENTA);

  va_start(ap, fmt);
  vprintf(fmt, ap);
  va_end(ap);

  printf(ANSI_COLOR_RESET);
}

void printf_cyan(const char *fmt, ...) {
  va_list ap;

  printf(ANSI_COLOR_CYAN);

  va_start(ap, fmt);
  vprintf(fmt, ap);
  va_end(ap);

  printf(ANSI_COLOR_RESET);
}
