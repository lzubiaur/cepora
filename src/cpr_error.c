#include "cpr_error.h"
#include "cpr_macros.h"

/* Log the error stack trace. Check if index `idx` is valid and the object
 * inherits from `error`.
 */
void dump_stack_trace(duk_context *ctx, duk_idx_t idx)
{
  if (duk_is_error(ctx, idx)) {
    if(duk_get_prop_string(ctx, idx, "stack")) {
      ERR(ctx, duk_safe_to_string(ctx, -1));
      if (duk_get_prop_string(ctx, idx, "cause")) {
        ERR(ctx, "Caused by:");
        dump_stack_trace(ctx, -1);
      }
      duk_pop(ctx);
    } else {
      ERR(ctx, duk_safe_to_string(ctx, idx));
    }
    duk_pop(ctx);
  } else {
    ERR(ctx, duk_safe_to_string(ctx, idx));
  }
}

/* Push an error object on the stack top with an associated cause error.
 * @param ctx the duktape context.
 * @param cause_idx is the index in the stack of cause associated to this error.
 * This value is set to the `cause` property of the main error object.
 * @param err_code error code associated with this error (valid range is [1,16777215]).
 * @param fmt the message format string.
 * @param ap variable argument list used with the format string.
 */
duk_idx_t push_cause_error_va(duk_context *ctx, duk_idx_t cause_idx, duk_errcode_t err_code, const char *fmt, va_list ap)
{
  duk_idx_t err_idx, norm_cause_idx;

  /* normalize index so reversed index is supported. */
  norm_cause_idx = duk_normalize_index(ctx, cause_idx);
  /* Create the error object */
  err_idx = duk_push_error_object_va(ctx, err_code, fmt, ap);

  duk_dup(ctx, norm_cause_idx);
  duk_put_prop_string(ctx, err_idx, "cause");

  return err_idx;
}

duk_idx_t push_cause_error(duk_context *ctx, duk_idx_t cause_idx, duk_errcode_t err_code, const char *fmt, ...)
{
  duk_idx_t err_idx;
  va_list ap;

  va_start(ap, fmt);
  err_idx = push_cause_error_va(ctx, cause_idx, err_code, fmt, ap);
  va_end(ap);

  return err_idx;
}

duk_ret_t throw_cause_error(duk_context *ctx, duk_idx_t cause_idx, duk_errcode_t err_code, const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  push_cause_error_va(ctx, cause_idx, err_code, fmt, ap);
  va_end(ap);

  duk_throw(ctx);

  return 0;
}
