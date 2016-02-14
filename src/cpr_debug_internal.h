/*
 * cpr_debug_internal.h
 * Copyright (c) 2015 Laurent Zubiaur
 * MIT License (http://opensource.org/licenses/MIT)
 */

#ifndef CPR_DEBUG_INTERNAL_H
#define CPR_DEBUG_INTERNAL_H

#include "cpr_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Raw print log message to `stdout` without additional formating. */
CPR_API_EXTERN void cpr_log_raw(const char *fmt, ...);

/* This is for internal debugging. Don't use it in client code */
#if defined(CPR_DEBUG_INTERNAL)
#include "duktape.h"
/* TODO __func__ is not standard and might not be defined in every compiler */
CPR_API_EXTERN void cpr__debug_log(const char *file, const char *func, const int line, const char *fmt, ...);
CPR_API_EXTERN void cpr__dump_context(const char *file, const char *func, const int line, duk_context *ctx);
#define CPR__DLOG(...) cpr__debug_log(__FILE__, __func__, __LINE__, __VA_ARGS__)
#define CPR__DUMP_CONTEXT(__ctx__) cpr__dump_context(__FILE__, __func__, __LINE__, __ctx__)
#else
#define CPR__DLOG(...) do { } while(0)
#define CPR__DUMP_CONTEXT(__ctx__) do { } while(0)
#endif /* CPR_DEBUG_INTERNAL */

#ifdef __cplusplus
}
#endif

#endif /* CPR_DEBUG_INTERNAL_H */
