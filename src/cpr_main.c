/*
 * cpr_main.c
 * Copyright (c) 2015 Laurent Zubiaur
 * MIT License (http://opensource.org/licenses/MIT)
 */

/* because cpr_config.h selects feature selection defines (e.g. _POSIX_C_SOURCE)
 * it must be included before any system headers are included */
#include "cpr_config.h"

#include <stdio.h>
#include <stdlib.h> /* getenv */
#ifdef _WIN32
#include <windows.h> /* WinMain */
#include <direct.h> /* _chdir */
#else
#include <unistd.h> /* chdir */
#endif
#include <string.h> /* strcmp */
#include <dlfcn.h>  /* dlopen... */
#include <libgen.h> /* basename */

#include "duktape.h"
#include "cpr_macros.h"
#include "cpr_error.h"
#include "cpr_sys_tools.h"
#include "cpr_mod_coffee.h"
#include "cpr_loadlib.h"

#ifdef _WIN32
#define chdir(p) (_chdir(p))
#define getcwd(d, s) (_getcwd(d, s))
#endif

#define CPR_VERSION_STRING "v0.10.99"
#define CPR_PATH_SEPARATOR ';'

/* logging file */
static FILE *stream = NULL;

void cpr_log_raw(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stdout, fmt, ap);
  va_end(ap);
  fflush(stdout);
}

#define BUF_SIZE 256
void cpr_log_perror(const char *fmt, ...) {
  char buf[BUF_SIZE];
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(buf, BUF_SIZE, fmt, ap);
  va_end(ap);
  perror(buf);
}

duk_ret_t cpr_search_path(duk_context *ctx) {
  if (duk_is_null_or_undefined(ctx, -1)) {
    duk_push_undefined(ctx);
    return 1;
  }
  duk_get_global_string(ctx, "paths");
  duk_enum(ctx, -1, DUK_ENUM_ARRAY_INDICES_ONLY);
  while (duk_next(ctx, -1, 1)) {
    duk_push_string(ctx, "/");
    duk_dup(ctx, 0);
    duk_concat(ctx, 3);
    if (cpr_file_exists(duk_get_string(ctx, -1))) {
      DBG(ctx, "File found '%s'", duk_get_string(ctx, -1));
      return 1;
    }
    WRN(ctx, "File NOT found '%s'", duk_get_string(ctx, -1));
    duk_pop_2(ctx); /* pop key and value */
  }
  duk_pop(ctx); /* enum */
  duk_pop(ctx); /* paths */
  ERR(ctx, "File NOT found '%s'", duk_get_string(ctx, 0));
  return 1;
}

/* Helper function to set logging level for C API. This can be changed in
 * javascript by updating `Duktape.Logger.clog.l` to 'TRC', 'DBG', 'INF'...
 */
void cpr_set_c_log_level(duk_context *ctx, const char *level) {
  duk_get_global_string(ctx, "Duktape");  /* [ Duktape ] */
  duk_get_prop_string(ctx, -1, "Logger"); /* [ Duktape Logger ] */
  duk_get_prop_string(ctx, -1, "clog");   /* [ Duktape Logger clog ] */
  duk_push_string(ctx, level);            /* [ Duktape Logger clog "level" ] */
  duk_put_prop_string(ctx, -2, "l");      /* [ Duktape Logger clog ] */
  duk_pop_3(ctx);
}

/* @javascript
 * @params id, require, exports, module
 */
duk_ret_t require_handler(duk_context *ctx) {
  const char *filename = NULL;
  duk_get_global_string(ctx, "search_path");
  duk_dup(ctx, 0);
  duk_call(ctx, 1);
  filename = duk_get_string(ctx, -1);
  /* TODO Lazy file extension check  */
  char *ext = strrchr(duk_get_string(ctx, -1), '.');
  if (ext && strcmp(ext, ".coffee") == 0) {
    INF(ctx, "Load CoffeeScript module '%s'", filename);
    duk_get_global_string(ctx, "coffee");
    duk_push_string(ctx, "compile_coffee");
    duk_push_string(ctx, filename);
    if (duk_pcall_prop(ctx, -3, 1) != DUK_EXEC_SUCCESS) {
      // duk_error(ctx, DUK_ERR_RANGE_ERROR, "argument out of range: %d", (int) argval);
      ERR(ctx, "Cannot compile CoffeeScript '%s'", filename);
      dump_stack_trace(ctx, -1);
    }
  } else if (ext && strcmp(ext, ".so") == 0) {
    INF(ctx, "Load C module id: '%s' filename:'%s'", duk_get_string(ctx, 0), filename);
    duk_push_c_function(ctx, cpr_loadlib, 2);
    duk_push_string(ctx, filename);
    duk_dup(ctx, 0);
    duk_call(ctx, 2);
    /* duk_replace(ctx, 2);*/  /* Replacing the "exports" table doesnt work */
    /* The init function should return (push) an object with the exported
    * functions/properties. Those exported functions are then copied to the
    * `exports` table so they are available outside the C module.
    */
    duk_enum(ctx, -1, DUK_ENUM_INCLUDE_NONENUMERABLE | DUK_ENUM_OWN_PROPERTIES_ONLY | DUK_ENUM_INCLUDE_INTERNAL);
    while (duk_next(ctx, -1 /*enum_index*/, 1 /*get_value*/)) {
      duk_put_prop(ctx, 2); /* `exports` table is the third parameters (at idx 2 on the stack) */
    }
    duk_pop(ctx); /* pop enum object */
    duk_pop(ctx); /* pop module result */
    duk_push_undefined(ctx); /* Return undefined because no source code. */
  } else {
    INF(ctx, "Load Javascript module '%s'", filename);
    duk_push_string_file(ctx, filename);
  }

  return 1;
}

/* Usage inspired from Node.js */
void cpr_usage() {
  cpr_log_raw("Usage: cepora [options] [script.js | script.coffee] [arguments]\n");
  cpr_log_raw("\n");
  cpr_log_raw("Options:\n");
  cpr_log_raw("  -v, --version    print version\n");
  cpr_log_raw("  -h, --help       print this message\n");
  cpr_log_raw("\n");
  cpr_log_raw("Environment variables:\n");
  cpr_log_raw("CPR_PATH           directory prefixed to the module search path. If not set the \n");
  cpr_log_raw("                   executable path is used as default modules root directory.\n");
  cpr_log_raw("\n");
  exit(EXIT_SUCCESS);
}

void cpr_version() {
  cpr_log_raw("Cepora %s - Git commit %s\n", CPR_VERSION_STRING, CPR_GIT_COMMIT);
  cpr_log_raw("Duktape %s\n", DUK_GIT_DESCRIBE);
  exit(EXIT_SUCCESS);
}

void fatal_handler(duk_context *ctx, duk_errcode_t code, const char *msg) {
  FTL(ctx, "Fatal error: %s [code: %d]", msg, code);
  dump_stack_trace(ctx, -1);
  /* Fatal handler should not return. */
  exit(EXIT_FAILURE);
}

duk_ret_t cpr__log_to_file_raw(duk_context *ctx) {
  duk_size_t size;
  void *data = NULL;
  data = duk_get_buffer_data(ctx, -1, &size);
  fwrite(data, size, 1, stream);
  fwrite("\n", 1, 1, stream);
  // fflush(stream);
  return 0;
}

int main(int argc, char *argv[]) {
  duk_context *ctx = NULL;
  int i = 0;
  char *path = NULL, *ptr = NULL, *ptr2 = NULL;
  const char *filename = NULL, *log_filename = NULL;

  i = 1;
  while (i < argc && argv[i][0] == '-') {
    if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0){
      cpr_version();
    } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
      cpr_usage();
    } else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
      if (i + 1 < argc) {
          log_filename = argv[++i];
      } else {
        cpr_log_raw("%s: %s requires an arguments\n", argv[0], argv[i]);
        exit(EXIT_FAILURE);
      }
    }
    ++i;
  }

  /* Run in CLI mode. First argument is the script file to run. */
  filename = i < argc ? argv[i] : "js/main.coffee";

  /* Create duktape VM heap */
  /* TODO investigate memory management implementations like tcmalloc
   * (https://github.com/gperftools/gperftools) and jmalloc
   * (https://github.com/jemalloc/jemalloc).
   */
  ctx = duk_create_heap(NULL, NULL, NULL, NULL, fatal_handler);

  if (!ctx) {
    cpr_log_raw("FATAL: Failed to create a Duktape heap.\n");
    goto fail;
  }

  /* TODO set log level from command line */
  cpr_set_c_log_level(ctx, "TRC");

  if (log_filename) {
    if ((stream = fopen(log_filename, "w")) == NULL) {
      cpr_log_perror("Can't open output log file '%s'", log_filename);
      goto fail;
    }
    duk_get_global_string(ctx, "Duktape");
    duk_get_prop_string(ctx, -1, "Logger");
    duk_get_prop_string(ctx, -1, "prototype");
    duk_push_c_function(ctx, cpr__log_to_file_raw, 1);
    duk_put_prop_string(ctx, -2, "raw");
    duk_dump_context_stdout(ctx);
    duk_pop_3(ctx);
  }

  /* Look for CPR_PATH environment variable. If CPR_PATH is not defined, try to
  * retreive the process executable path.
  */
  i = 0;
  if ((path = getenv("CPR_PATH")) != NULL) {
    cpr_log_raw("CPR_PATH is defined to '%s'\n", path);
    ptr = path;
    duk_push_array(ctx);
    while((ptr2 = strchr(ptr, CPR_PATH_SEPARATOR)) != NULL) {
      if ((ptr2 - ptr) > 0 ) {
        duk_push_lstring(ctx, ptr, (duk_size_t)(ptr2 - ptr));
        duk_put_prop_index(ctx, -2, i++);
      }
      ptr = ptr2 + 1;
    }
    if (strlen(ptr) > 0) {
      duk_push_string(ctx, ptr);
      duk_put_prop_index(ctx, -2, i++);
    }
    duk_put_global_string(ctx, "paths");
  } else if ((path = cpr_get_exec_dir()) != NULL) {
    duk_push_array(ctx);
    duk_push_string(ctx, path);
    duk_put_prop_index(ctx, -2, 0);
    duk_push_string(ctx, path);
    duk_push_string(ctx, "/../Resources");
    duk_concat(ctx, 2);
    duk_put_prop_index(ctx, -2, 1);
    duk_put_global_string(ctx, "paths");
  } else {
    cpr_log_raw("Cannot retrieve executable path. Please set 'CPR_PATH' to the install directory and try again.\n");
    goto finished;
  }


  /* Note regarding function binding parameters.
  * The third argument of `duk_push_c_function` is the number of parameters the
  * C function expects. It can be variable (DUK_VARARGS) or set to a
  * maximum number of parameters.
  * If a maximum of parameters is explicitly defined then the C functions will
  * get a value stack with all the expected parameters with the missing
  * parameters set to `undefined` and the extra parameter ignored.
  * If DUK_VARARGS is used then `duk_get_top` will return the number of
  * parameter passed.
  */
  duk_push_global_object(ctx);
  duk_push_c_function(ctx, cpr_search_path, 1); /* C function with exactly one argument */
  duk_put_prop_string(ctx, -2 /*index of global*/, "search_path");
  duk_pop(ctx);  /* pop global */

  /* Load module coffee */
  duk_push_global_object(ctx);
  duk_push_c_function(ctx, dukopen_coffee, 0);
  duk_call(ctx, 0);
  duk_put_global_string(ctx, "coffee");
  duk_pop(ctx); /*  pop global */

  /* Store command line arguments in the `Duktape` global object. */
  duk_push_global_object(ctx);
  duk_get_prop_string(ctx, -1, "Duktape");
  duk_push_string(ctx, "arguments");
  duk_idx_t arr_idx = duk_push_array(ctx); /* push an empty array */

  for (int i=2; i<argc; ++i) { /* Start at argument 2 (0: proccess, 1: script) */
    duk_push_string(ctx, argv[i]);
    duk_put_prop_index(ctx, arr_idx, i - 2);
  }

  duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE); /* Non writable property */
  duk_pop_2(ctx);

  duk_push_c_function(ctx, dukopen_loadlib, 0);
  duk_call(ctx, 0);
  duk_put_global_string(ctx, "mod");

  duk_get_global_string(ctx, "Duktape");
  duk_push_c_function(ctx, require_handler, 4);
  duk_put_prop_string(ctx, -2, "modSearch");
  duk_pop(ctx);

  /* Get CoffeeScript compiler full path */
  duk_get_global_string(ctx, "search_path");
  duk_push_string(ctx, "js/lib/coffee-script.js");
  duk_pcall(ctx, 1);

  DBG(ctx, "Loading CoffeeScript compiler '%s'", duk_get_string(ctx, -1));
  if (duk_peval_file(ctx, duk_get_string(ctx, -1)) != 0) {
    ERR(ctx, "Error loading CoffeeScript compiler: '%s'", duk_get_string(ctx, -1));
    dump_stack_trace(ctx, -1);
    goto finished;
  }
  duk_pop(ctx); /* pop duk_peval_file resul */
  duk_pop(ctx); /* pop path */

  duk_get_global_string(ctx, "search_path");
  duk_push_string(ctx, filename);
  duk_pcall(ctx, 1);

  duk_get_global_string(ctx, "coffee");
  duk_push_string(ctx, "eval_script");
  duk_dup(ctx, -3);
  if (duk_pcall_prop(ctx, -3, 1) != DUK_EXEC_SUCCESS) {
    /* If duk_safe_call fails the error object is at the top of the context.
     * But we must request at least one return value to actually get the error
     * object on the stack. */
    // ERR(ctx, "Error processing script '%s'", filename); /* Not revelant error message */
    dump_stack_trace(ctx, -1);
    goto finished;
  }
  // TODO how to return code from javasctipt ?
  // INF(ctx, "Script succeed with code: %s\n", duk_safe_to_string(ctx, -1));
  duk_pop(ctx);

  INF(ctx, "Bye!");

finished:
  duk_destroy_heap(ctx);
  fclose(stream);
  return EXIT_SUCCESS;
fail:
  if (stream) {
    fclose(stream); /* Error ignored */
  }
  duk_destroy_heap(ctx); /* No-op if ctx is NULL */
  return EXIT_FAILURE;
}
