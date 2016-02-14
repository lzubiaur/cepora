/*
 * cpr_gl3w.c
 * Copyright (c) 2015 Laurent Zubiaur
 * MIT License (http://opensource.org/licenses/MIT)
 */

#include "cpr_gl3w.h"
#include "GL/gl3w.h"
#include "cpr_gl.h"

CPR_API_INTERN duk_ret_t cpr_gl3w_init(duk_context *ctx) {
  duk_push_boolean(ctx, gl3wInit() == 0);
  return 1;
}

CPR_API_INTERN duk_ret_t cpr_gl3w_is_supported(duk_context *ctx) {
  duk_push_boolean(ctx, gl3wIsSupported(duk_require_int(ctx, 0), duk_require_int(ctx, 1)));
  return 1;
}

/* Implemented for completeness but not tested */
CPR_API_INTERN duk_ret_t cpr_gl3w_get_proc_address(duk_context *ctx) {
#if defined(__GNUC__)
  duk_push_pointer(ctx, (__extension__ (void *)gl3wGetProcAddress(duk_require_string(ctx, 0))));
#else
  duk_push_pointer(ctx, gl3wGetProcAddress(duk_require_string(ctx, 0)));
#endif
  return 1;
}

CPR_API_EXTERN duk_ret_t dukopen_gl3w(duk_context *ctx) {
  const duk_function_list_entry module_funcs[] = {
    { "init",           cpr_gl3w_init,              0 },
    { "isSupported",    cpr_gl3w_is_supported,      2 },
    { "getProcAddress", cpr_gl3w_get_proc_address,  1 },
    { NULL, NULL, 0 }
  };

  /* The OpenGL (gl) module is loaded into this module */
  /* XXX load OpenGL into its own object? */
  duk_push_c_function(ctx, dukopen_gl, 1);

  duk_push_object(ctx);  /* module result */
  duk_put_function_list(ctx, -1, module_funcs);

  /* Open the gl module */
  duk_call(ctx, 1);

  return 1;  /* return module value */
}
