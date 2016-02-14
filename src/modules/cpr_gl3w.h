/*
 * cpr_gl3w.h
 * Copyright (c) 2015 Laurent Zubiaur
 * MIT License (http://opensource.org/licenses/MIT)
 */

#ifndef CPR_GL3W_H
#define CPR_GL3W_H

#include "duktape.h"
#include "cpr_config.h"

#ifdef __cplusplus
extern "C" {
#endif

CPR_API_EXTERN duk_ret_t dukopen_gl3w(duk_context *ctx);

#ifdef __cplusplus
}
#endif

#endif /* CPR_GL3W_H */
