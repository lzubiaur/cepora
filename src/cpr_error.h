/*
 * cpr_error.h
 * Copyright (c) 2015 Laurent Zubiaur
 * MIT License (http://opensource.org/licenses/MIT)
 */

#ifndef CPR_ERROR_H
#define CPR_ERROR_H

#include "duktape.h"

/* Error code (duktape error code starts at 1) */
#define CPR_INTERNAL_ERROR        1
#define CPR_COFFEE_SCRIPT_ERROR   2

#ifdef __cplusplus
extern "C" {
#endif

void cpr_dump_stack_trace(duk_context *ctx, duk_idx_t idx);

duk_idx_t cpr_push_cause_error_va(duk_context *ctx, duk_idx_t cause_idx, duk_errcode_t err_code, const char *fmt, va_list ap);
duk_idx_t cpr_push_cause_error(duk_context *ctx, duk_idx_t cause_idx, duk_errcode_t err_code, const char *fmt, ...);
duk_ret_t cpr_throw_cause_error(duk_context *ctx, duk_idx_t cause_idx, duk_errcode_t err_code, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* CPR_ERROR_H */
