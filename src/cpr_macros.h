/*
 * cpr_macros.h
 * Copyright (c) 2015 Laurent Zubiaur
 * MIT License (http://opensource.org/licenses/MIT)
 */

#ifndef CPR_MACROS_H
#define CPR_MACROS_H

#include "duktape.h"

/* Logging macros */
#define TRC(__ctx__, ...) duk_log(__ctx__, DUK_LOG_TRACE, __VA_ARGS__)
#define DBG(__ctx__, ...) duk_log(__ctx__, DUK_LOG_DEBUG, __VA_ARGS__)
#define INF(__ctx__, ...) duk_log(__ctx__, DUK_LOG_INFO,  __VA_ARGS__)
#define WRN(__ctx__, ...) duk_log(__ctx__, DUK_LOG_WARN,  __VA_ARGS__)
#define ERR(__ctx__, ...) duk_log(__ctx__, DUK_LOG_ERROR, __VA_ARGS__)
#define FTL(__ctx__, ...) duk_log(__ctx__, DUK_LOG_FATAL, __VA_ARGS__)

#endif /* CPR_MACROS_H */
