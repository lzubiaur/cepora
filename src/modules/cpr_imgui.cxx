/*
 * cpr_imgui.c
 * Copyright (c) 2015 Laurent Zubiaur
 * MIT License (http://opensource.org/licenses/MIT)
 */

#include "cpr_imgui.h"
#include <imgui.h>

static duk_ret_t cpr_imgui_get_io(duk_context *ctx)
{
  ImGui::GetIO();
  return 0;
}

duk_ret_t dukopen_imgui(duk_context *ctx) {
  const duk_function_list_entry module_funcs[] = {
    { "getIO", cpr_imgui_get_io, 1 },
    { NULL, NULL, 0 }
  };

  const duk_number_list_entry module_consts[] = {
    { "BAR", (double)(-1) },
    { NULL, 0.0 }
  };

  duk_push_object(ctx);
  duk_put_function_list(ctx, -1, module_funcs);
  duk_put_number_list(ctx, -1, module_consts);

  return 1;
}
