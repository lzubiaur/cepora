/*
 * cpr_loadlib.h
 * Copyright (c) 2015 Laurent Zubiaur
 * MIT License (http://opensource.org/licenses/MIT)
 */

#ifndef CPR_LOADLIB_H
#define CPR_LOADLIB_H

#include "duktape.h"
#include "cpr_config.h"

#ifdef __cplusplus
extern "C" {
#endif

CPR_API_EXTERN duk_ret_t dukopen_loadlib(duk_context *ctx);
CPR_API_EXTERN duk_ret_t cpr_loadlib(duk_context *ctx);

#ifdef __cplusplus
}
#endif

#endif /* CPR_LOADLIB_H */
