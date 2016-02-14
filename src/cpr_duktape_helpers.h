/*
 * cpr_duktape_helpers.h
 * Copyright (c) 2015 Laurent Zubiaur
 * MIT License (http://opensource.org/licenses/MIT)
 */

#ifndef CPR_DUKTAPE_HELPERS_H
#define CPR_DUKTAPE_HELPERS_H

#include "duktape.h"
#include "cpr_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/* HACK Duktape v 1.5.0 should provied a more flexible `duk_put_function_list`.
 * https://github.com/svaarala/duktape/issues/130 */

struct cpr_function_list_magic_entry {
  const char *key;
  duk_c_function value;
  duk_idx_t nargs;
  duk_int_t magic;
};

typedef struct cpr_function_list_magic_entry cpr_function_list_magic_entry;
CPR_API_EXTERN void cpr_put_function_list_magic(duk_context *ctx, duk_idx_t obj_index, const cpr_function_list_magic_entry *funcs);

#ifdef __cplusplus
}
#endif

#endif /* CPR_DUKTAPE_HELPERS_H */
