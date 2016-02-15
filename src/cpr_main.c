/*
 * cpr_main.c
 * Copyright (c) 2015 Laurent Zubiaur
 * MIT License (http://opensource.org/licenses/MIT)
 */

/* because cpr_config.h selects feature selection defines (e.g. _POSIX_C_SOURCE)
 * it must be included before any system headers are included */
#include "cpr_config.h"

#include <stdio.h>
#include <errno.h>
#include <string.h> /* strcmp */

#if defined(CPR_BUILD_WINDOWS)
#include <windows.h> /* WindMain */
#endif

#include "duktape.h"
#include "cpr_debug_internal.h"
#include "cpr_macros.h"
#include "cpr_error.h"
#include "cpr_sys_tools.h"
#include "cpr_package.h"
#include "cpr_loadlib.h"

#define CPR_VERSION_STRING "v0.10.99"
#define CPR__COFFEE_SCRIPT_PATH "js/lib/coffee-script.js"

/* Helper function to set logging level for both C and Javascript API.
 * Note that this only set the *DEFAULT* javascript logging level and will not
 * change Logger objects already created.
 * C logging level can be changed in javascript by updating `Duktape.Logger.clog.l`.
 * Default Javascript level can be changed in `Duktape.Logger.prototype.l`.
 */
void cpr_set_default_log_level(duk_context *ctx, unsigned short level) {
  CPR__DLOG("Set log level to %d", level);
  duk_get_global_string(ctx, "Duktape");      /* [ Duktape ] */
  duk_get_prop_string(ctx, -1, "Logger");     /* [ Duktape Logger ] */
  /* Set c logger level */
  duk_get_prop_string(ctx, -1, "clog");       /* [ Duktape Logger clog ] */
  duk_push_int(ctx, level);                   /* [ Duktape Logger clog "level" ] */
  duk_put_prop_string(ctx, -2, "l");          /* [ Duktape Logger clog ] */
  duk_pop(ctx);                               /* [ Duktape Logger ] */
  /* Set Javascript default logger level */
  duk_get_prop_string(ctx, -1, "prototype");  /* [ Duktape Logger prototype ] */
  duk_push_int(ctx, level);                   /* [ Duktape Logger clog "level" ] */
  duk_put_prop_string(ctx, -2, "l");          /* [ duktape Logger prototype ] */
  duk_pop_3(ctx);
}

/* Usage inspired from Node.js */
static void cpr__usage() {
  cpr_log_raw("Usage: cepora [options] [-o filename] [-l level] [script.js | script.coffee] [arguments]\n");
  cpr_log_raw("\n");
  cpr_log_raw("Options:\n");
  cpr_log_raw("  -v, --version    print version\n");
  cpr_log_raw("  -h, --help       print this message\n");
  cpr_log_raw("  -o               redirect logging to file\n");
  cpr_log_raw("  -l               set default logging level (0-5)\n");
  cpr_log_raw("\n");
  cpr_log_raw("Environment variables:\n");
  cpr_log_raw("CPR_PATH           semi-colon separated directories list to seach for module and scripts.");
  cpr_log_raw("\n");
  exit(EXIT_SUCCESS);
}

static void cpr__version() {
  /* TODO */
  cpr_log_raw("Cepora %s - Git commit %s\n", CPR_VERSION_STRING, CPR_GIT_COMMIT);
  cpr_log_raw("Duktape %s\n", DUK_GIT_DESCRIBE);
  exit(EXIT_SUCCESS);
}

static void cpr__fatal_handler(duk_context *ctx, duk_errcode_t code, const char *msg) {
  FTL(ctx, "Fatal error: %s [code: %d]", msg, code);
  /* Fatal handler should not return. */
  exit(EXIT_FAILURE);
}

/* Load the core module into the global environment */
static duk_ret_t cpr__open_core_modules(duk_context *ctx) {
  /* Load the `package` module */
  duk_push_c_function(ctx, dukopen_package, 0);
  duk_call(ctx, 0);
  duk_put_global_string(ctx, CPR_PACKAGE_NAME);

  /* Load the `package` module */
  duk_push_c_function(ctx, dukopen_loadlib, 0);
  duk_call(ctx, 0);
  duk_put_global_string(ctx, "lib");

  return 0;
}

#ifdef _WIN32
int CALLBACK WinMain(
   HINSTANCE hInstance,
   HINSTANCE hPrevInstance,
   LPSTR     lpCmdLine,
   int       nCmdShow) {
#else
int main(int argc, char *argv[]) {
#endif
  duk_context *ctx = NULL;
  int i = 0, argsConsumed = 0;
  int  log_level = 4; /* Default log level to ERROR */
  const char *filename = NULL, *log_path = NULL;

  /* Arguments are parsed using a while loop because to "consume" unused options.
   * WARNING: when opening an application on MacOS using the `open` command
   * without explicit arguments (--args option) then the first argument is the PSN
   * id (-psn_xxxx).
   */
  i = 1;
  while (i < argc && argv[i][0] == '-') {
    CPR__DLOG("argument %d : %s", i, argv[i]);
    if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0){
      cpr__version();
    } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
      cpr__usage();
    } else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
      if (i + 1 < argc) {
          log_path = argv[++i];
      } else {
        cpr_log_raw("%s: %s requires an arguments\n", argv[0], argv[i]);
        exit(EXIT_FAILURE);
      }
    } else if (strcmp(argv[i], "-l") == 0) {
      if (i + 1 < argc) {
        log_level = strtol(argv[++i], NULL, 10);
      } else {
        cpr_log_raw("%s: %s requires an arguments\n", argv[0], argv[i]);
        exit(EXIT_FAILURE);
      }
    }
    ++i;
  }
  argsConsumed = i;

  /* Run in CLI mode. First argument is the script file to run. */
  if (argsConsumed < argc) {
    filename = argv[argsConsumed];
    argsConsumed++;
  } else {
    filename = "js/main.coffee";
  }
  CPR__DLOG(filename);

  /* Create duktape VM heap */
  /* TODO investigate memory management implementations like tcmalloc
   * (https://github.com/gperftools/gperftools) and jmalloc
   * (https://github.com/jemalloc/jemalloc).
   */
  ctx = duk_create_heap(NULL, NULL, NULL, NULL, cpr__fatal_handler);

  if (!ctx) {
    cpr_log_raw("FATAL: Failed to create a Duktape heap.\n");
    goto finished;
  }

  cpr_set_default_log_level(ctx, log_level);

  /* Redirect the logger ouput to a file stream */
  if (log_path) {
    CPR__DLOG("Redirect log stream to file '%s'", log_path);
    if (freopen(log_path, "w", stdout) == NULL || freopen(log_path, "w", stderr) == NULL) {
      WRN(ctx, "Can't redirect log stream to file '%s' : %s", log_path, strerror(errno));
    }
  }

  if (duk_safe_call(ctx, cpr__open_core_modules, 0, 1)) {
    FTL(ctx, "Can't open core modules.");
    cpr_dump_stack_trace(ctx, -1);
    goto finished;
  }
  duk_pop(ctx); /* result */

  duk_get_global_string(ctx, "Duktape");
  duk_push_string(ctx, DUK_USE_OS_STRING);
  duk_put_prop_string(ctx, -2, "os");
  duk_push_string(ctx, DUK_USE_ARCH_STRING);
  duk_put_prop_string(ctx, -2, "arch");
  duk_pop(ctx); /* Duktape */

  /* Store command line arguments in the `Duktape` global object. */
  duk_push_global_object(ctx);
  duk_get_prop_string(ctx, -1, "Duktape");
  duk_push_string(ctx, "arguments");
  duk_idx_t arr_idx = duk_push_array(ctx); /* push an empty array */

  for (i = argsConsumed; i < argc; ++i) {
    duk_push_string(ctx, argv[i]);
    duk_put_prop_index(ctx, arr_idx, i - argsConsumed);
  }

  duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE); /* Non writable property */
  duk_pop_2(ctx);

  /* Get CoffeeScript compiler full path */
  duk_get_global_string(ctx, CPR_PACKAGE_NAME);
  duk_get_prop_string(ctx, -1, "searchPath");
  duk_push_string(ctx, CPR__COFFEE_SCRIPT_PATH);
  duk_pcall(ctx, 1);
  if (duk_is_null_or_undefined(ctx, -1)) {
    FTL(ctx, "Can't find CoffeeScript compiler : " CPR__COFFEE_SCRIPT_PATH);
    goto finished;
  }

  /* Load CoffeeScript compiler into the global environment. The CoffeeScript
   * compiler will be available in the global variable `CoffeeScript`.
   */
  DBG(ctx, "Loading CoffeeScript compiler '%s'", duk_get_string(ctx, -1));
  if (duk_peval_file(ctx, duk_get_string(ctx, -1)) != 0) {
    FTL(ctx, "Error loading CoffeeScript compiler: '%s'", duk_get_string(ctx, -1));
    cpr_dump_stack_trace(ctx, -1);
    goto finished;
  }
  duk_pop_3(ctx); /* [package] [searchPath] [result] */

  duk_get_global_string(ctx, CPR_PACKAGE_NAME);
  duk_get_prop_string(ctx, -1, "searchPath");
  duk_push_string(ctx, filename);
  duk_pcall(ctx, 1);
  /* If no file is found then `undefined` is pushed */
  if (duk_is_null_or_undefined(ctx, -1)) {
    FTL(ctx, "Can't find script : '%s'", filename);
    goto finished;
  }
  CPR__DLOG("main script : '%s'", filename);

  /* Get the CoffeeScript global object */
  duk_get_global_string(ctx, "CoffeeScript");
  duk_push_string(ctx, "compile");
  /* Push the content of the file on the top of the stack */
  duk_push_string_file(ctx, duk_get_string(ctx, -3));
  /* Compile the coffee script in "safe" mode */
  if (duk_pcall_prop(ctx, -3, 1) != DUK_EXEC_SUCCESS) {
    /* If duk_safe_call fails the error object is at the top of the context.
     * But we must request at least one return value to actually get the error
     * object on the stack. */
    FTL(ctx, "Can't compile script '%s' : %s", filename, duk_safe_to_string(ctx, -1));
    goto finished;
  }
  /* Insert the compiled CoffeeScript script before in the stack so we can
   * clean up (pop) before evaluating the script */
  duk_insert(ctx, -4);
  duk_pop_3(ctx); /* [package] [searchPath] [CoffeeScript] */
  CPR__DUMP_CONTEXT(ctx); /* Stack should only contain the script source code */
  if (duk_peval(ctx)) {
    cpr_dump_stack_trace(ctx, -1);
  }

  INF(ctx, "Bye!");

finished:
  duk_destroy_heap(ctx); /* No-op if ctx is NULL */
  return EXIT_SUCCESS;
}
