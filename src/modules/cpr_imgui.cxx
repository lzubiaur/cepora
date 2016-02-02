/*
 * cpr_imgui.c
 * Copyright (c) 2015 Laurent Zubiaur
 * MIT License (http://opensource.org/licenses/MIT)
 */

#include "cpr_imgui.h"
#include "cpr_debug_internal.h"
#include "cpr_macros.h"
#include <imgui.h>

#define CPR__X_GETTER           1
#define CPR__Y_GETTER           2
#define CPR__X_SETTER           3
#define CPR__Y_SETTER           4
#define CPR__DELTA_TIME_GETTER  5
#define CPR__DELTA_TIME_SETTER  6

static duk_ret_t cpr_imgui_io_getter_setter(duk_context *ctx) {
  duk_int_t magic;
  magic = duk_get_current_magic(ctx);
  CPR__DLOG("Getter/Setter magic : %d", magic);
  switch(magic) {
  case CPR__X_GETTER:
    duk_push_int(ctx, ImGui::GetIO().DisplaySize.x); return 1;
  case CPR__Y_GETTER:
    duk_push_int(ctx, ImGui::GetIO().DisplaySize.y); return 1;
  case CPR__X_SETTER:
    ImGui::GetIO().DisplaySize.x = duk_require_int(ctx, 0); return 0;
  case CPR__Y_SETTER:
    ImGui::GetIO().DisplaySize.y = duk_require_int(ctx, 0); return 0;
  case CPR__DELTA_TIME_GETTER:
    duk_push_number(ctx, ImGui::GetIO().DeltaTime); return 1;
  case CPR__DELTA_TIME_SETTER:
    ImGui::GetIO().DeltaTime = duk_require_number(ctx, 0); return 0;
  default:
    ERR(ctx, "Unknown magic value : %d", magic);
    duk_push_undefined(ctx);
  }
  return 1;
}

/* Bind ImGui::GetIO() to module.getIO(). */
static duk_ret_t cpr_imgui_get_io(duk_context *ctx) {
  duk_push_this(ctx);
  if (duk_get_prop_string(ctx, -1, "ImGuiIO")) {
    CPR__DLOG("ImGuiIO object exists");
    CPR__DUMP_CONTEXT(ctx);
    return 1;
  }
  CPR__DLOG("Create ImGuiIO object");
  duk_pop(ctx); /* pop undefined property `duk_get_prop_string` */
  /* Create the ImGuiIO object */
  duk_push_string(ctx, "ImGuiIO");
  duk_push_object(ctx);
  /* ImGuiIO.DisplaySize */
  duk_push_string(ctx, "displaySize");
  duk_push_object(ctx);
  /* ImGuiIO.DisplaySize.x */
  duk_push_string(ctx, "x");
  duk_push_c_function(ctx, cpr_imgui_io_getter_setter, 0);
  duk_set_magic(ctx, -1, CPR__X_GETTER);
  duk_push_c_function(ctx, cpr_imgui_io_getter_setter, 1);
  duk_set_magic(ctx, -1, CPR__X_SETTER);
  duk_def_prop(ctx, -4, DUK_DEFPROP_HAVE_GETTER | DUK_DEFPROP_HAVE_SETTER); /* Define prop ImGuiIO.DisplaySize.y */
  /* ImGuiIO.DisplaySize.y */
  duk_push_string(ctx, "y");
  duk_push_c_function(ctx, cpr_imgui_io_getter_setter, 0);
  duk_set_magic(ctx, -1, CPR__Y_GETTER);
  duk_push_c_function(ctx, cpr_imgui_io_getter_setter, 1);
  duk_set_magic(ctx, -1, CPR__Y_SETTER);
  duk_def_prop(ctx, -4, DUK_DEFPROP_HAVE_GETTER | DUK_DEFPROP_HAVE_SETTER); /* Define prop ImGuiIO.DisplaySize.y */
  duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE); /* Define prop "displaySize" */
  /* ImGuiIO.DeltaTime */
  duk_push_string(ctx, "deltaTime");
  duk_push_c_function(ctx, cpr_imgui_io_getter_setter, 0);
  duk_set_magic(ctx, -1, CPR__DELTA_TIME_GETTER);
  duk_push_c_function(ctx, cpr_imgui_io_getter_setter, 1);
  duk_set_magic(ctx, -1, CPR__DELTA_TIME_SETTER);
  duk_def_prop(ctx, -4, DUK_DEFPROP_HAVE_GETTER | DUK_DEFPROP_HAVE_SETTER); /* Define prop ImGuiIO.DeltaTime */

  duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE); /* Define prop "module.ImGuiIO" */
  /* Push/return the `ImGuiIO` object */
  duk_get_prop_string(ctx, -1, "ImGuiIO");
  CPR__DUMP_CONTEXT(ctx);
  return 1;
}

static duk_ret_t cpr_imgui_new_frame(duk_context *ctx) {
  ImGui::NewFrame();
  return 0;
}

static duk_ret_t cpr_imgui_render(duk_context *ctx) {
  ImGui::Render();
  return 0;
}

duk_ret_t dukopen_imgui(duk_context *ctx) {
  const duk_function_list_entry module_funcs[] = {
    { "getIO", cpr_imgui_get_io, 1 },
    { "newFrame", cpr_imgui_new_frame, 0 },
    { "render", cpr_imgui_render, 0 },
    { NULL, NULL, 0 }
  };

  const duk_number_list_entry module_consts[] = {
    /* { "BAR", (double)(-1) }, */
    { NULL, 0.0 }
  };

  duk_push_object(ctx);
  duk_put_function_list(ctx, -1, module_funcs);
  duk_put_number_list(ctx, -1, module_consts);

  return 1;
}
