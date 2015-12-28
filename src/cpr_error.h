#ifndef CPR_ERROR_H
#define CPR_ERROR_H

#include "duktape.h"

/* Error code (duktape error code starts at 1) */
#define CPR_COFFEE_SCRIPT_ERROR 1

void dump_stack_trace(duk_context *ctx, duk_idx_t idx);

duk_idx_t push_cause_error_va(duk_context *ctx, duk_idx_t cause_idx, duk_errcode_t err_code, const char *fmt, va_list ap);
duk_idx_t push_cause_error(duk_context *ctx, duk_idx_t cause_idx, duk_errcode_t err_code, const char *fmt, ...);
duk_ret_t throw_cause_error(duk_context *ctx, duk_idx_t cause_idx, duk_errcode_t err_code, const char *fmt, ...);

#endif /* CPR_ERROR_H */
