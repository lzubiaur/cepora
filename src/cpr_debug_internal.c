/*
 * cpr_debug_internal.c
 * Copyright (c) 2015 Laurent Zubiaur
 * MIT License (http://opensource.org/licenses/MIT)
 */
#include "cpr_debug_internal.h"

#include <stdio.h>
#include <stdarg.h> /* va_list... */
#include <string.h> /* strerror */

void cpr_log_raw(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stdout, fmt, ap);
  va_end(ap);
  fflush(stdout);
}

#if defined(CPR_DEBUG_INTERNAL)

#define CPR__DEBUG_LOG_BUF_SIZE 4098L
static char debug_log_buf[CPR__DEBUG_LOG_BUF_SIZE];

void cpr__debug_log(const char *file, const int line, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(debug_log_buf, CPR__DEBUG_LOG_BUF_SIZE, fmt, ap);
  va_end(ap);
  fprintf(stdout, "[DEBUG] %s:%ld : %s\n", file, (long)line, debug_log_buf);
  fflush(stdout);
}

#endif /* CPR_DEBUG_INTERNAL */
