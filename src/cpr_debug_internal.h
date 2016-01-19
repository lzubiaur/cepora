/*
 * cpr_debug_internal.h
 * Copyright (c) 2015 Laurent Zubiaur
 * MIT License (http://opensource.org/licenses/MIT)
 */

#ifndef CPR_DEBUG_INTERNAL_H
#define CPR_DEBUG_INTERNAL_H

/* Raw print log message to `stdout` without additional formating. */
void cpr_log_raw(const char *fmt, ...);

/* This is for internal debugging. Don't use it in client code */
#if defined(CPR_DEBUG_INTERNAL)
/* TODO __func__ is not standard and might not be defined in every compiler */
void cpr__debug_log(const char *file, const char *func, const int line, const char *fmt, ...);
#define CPR__DLOG(...) cpr__debug_log(__FILE__, __func__, __LINE__, __VA_ARGS__)
#else
#define CPR__DLOG(x) do { } while(0)
#endif /* CPR_DEBUG_INTERNAL */

#endif /* CPR_DEBUG_INTERNAL_H */
