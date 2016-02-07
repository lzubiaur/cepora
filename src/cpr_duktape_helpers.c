/*
 * cpr_duktap_helpers.c
 * Copyright (c) 2015 Laurent Zubiaur
 * MIT License (http://opensource.org/licenses/MIT)
 */

#include "cpr_duktape_helpers.h"

void cpr_put_function_list_magic(duk_context *ctx, duk_idx_t obj_index, const cpr_function_list_magic_entry *funcs) {
  const cpr_function_list_magic_entry *ent = funcs;

  obj_index = duk_require_normalize_index(ctx, obj_index);

  if (ent != NULL) {
    while (ent->key != NULL) {
      duk_push_c_function(ctx, ent->value, ent->nargs);
      if (ent->magic != 0) {
        duk_set_magic(ctx, -1, ent->magic);
      }
      duk_put_prop_string(ctx, obj_index, ent->key);
      ent++;
    }
  }
}
