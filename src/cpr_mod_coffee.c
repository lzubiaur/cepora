/*
 * cpr_mod_coffee.c
 * Copyright (c) 2015 Laurent Zubiaur
 * MIT License (http://opensource.org/licenses/MIT)
 */

#include "cpr_mod_coffee.h"
#include "cpr_error.h"
#include "cpr_macros.h"

/* Compile a CoffeeScript file into JavaScript. Result is pushed at the top of
 * the context.
 * Throw an error on IO file access (e.g. file not found) or on compilation
 * error (e.g. syntax error).
 */
static duk_ret_t cpr__compile_coffee(duk_context *ctx) {
  const char *filename = duk_to_string(ctx, -1);
  if (duk_get_global_string(ctx, "CoffeeScript")) {
    duk_push_string(ctx, "compile");
    duk_push_string_file(ctx, filename);
    duk_call_prop(ctx, -3, 1);
  } else {
    duk_error(ctx, CPR_COFFEE_SCRIPT_ERROR, "Can't find global CoffeeScript compiler object.");
  }
  return 1;
}

/* Compile and eval a CoffeeScript file. Throw an error on IO and compilation
 * errors (e.g. file not found, syntax error).
 */
static duk_ret_t cpr__eval_coffee(duk_context *ctx) {
  // duk_get_global_string(ctx, "compile_coffee"); /* Get the CoffeeScript global compiler */
  // duk_insert(ctx, -2); /* Insert the compiler before the filename on the stack */
  // duk_call(ctx, 1); /* call the compiler on the CoffeeScript source */
  cpr__compile_coffee(ctx);
  duk_eval(ctx); /* Run the compiled JavaScript code */
  return 0;
}

/* Load and run JavaScript and CoffeeScript file.  */
static duk_ret_t cpr__eval_script(duk_context *ctx) {
  const char *filename = duk_to_string(ctx, -1);
  DBG(ctx, "Loading script '%s'", filename);
  /* TODO Lazy file extension check  */
  char *dot = strrchr(filename, '.');
  if (dot && !strcmp(dot, ".coffee")) {
    // duk_get_global_string(ctx, "eval_coffee");
    duk_push_string(ctx, filename);
    // duk_call(ctx, 1);
    cpr__eval_coffee(ctx);
  } else {
    duk_eval_file(ctx, filename);
  }
  return 0;
}

static const duk_function_list_entry module_funcs[] = {
    { "evalScript",     cpr__eval_script,     1 },
    { "evalCoffee",     cpr__eval_coffee,     1 },
    { "compileCoffee",  cpr__compile_coffee,  1 },
    { NULL, NULL, 0 }
};

duk_ret_t dukopen_coffee(duk_context *ctx) {
  duk_push_object(ctx);  /* module result */
  duk_put_function_list(ctx, -1, module_funcs);

  return 1;  /* return module value */
}
