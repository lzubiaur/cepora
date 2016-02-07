/*
 * cpr_imgui.c
 * Copyright (c) 2015 Laurent Zubiaur
 * MIT License (http://opensource.org/licenses/MIT)
 */

#include "cpr_imgui.h"
#include "cpr_debug_internal.h"
#include "cpr_macros.h"
#include "cpr_duktape_helpers.h"
#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>

#define CPR__DISPLAY_SIZE_X_GETTER        1
#define CPR__DISPLAY_SIZE_Y_GETTER        2
#define CPR__DISPLAY_SIZE_X_SETTER        3
#define CPR__DISPLAY_SIZE_Y_SETTER        4
#define CPR__MOUSE_X_GETTER               5
#define CPR__MOUSE_Y_GETTER               6
#define CPR__MOUSE_X_SETTER               7
#define CPR__MOUSE_Y_SETTER               8
#define CPR__DELTA_TIME_GETTER            9
#define CPR__DELTA_TIME_SETTER            10
#define CPR__RENDER_DRAW_LISTS_FN_GETTER  11
#define CPR__RENDER_DRAW_LISTS_FN_SETTER  12

/* Forward declarations */

/* Helpers */
static ImVec2 cpr_imgui_imvec2(duk_context *ctx, duk_idx_t idx);
static ImVec4 cpr_imgui_imvec4(duk_context *ctx, duk_idx_t idx);

/* Binding API */

static duk_ret_t cpr_imgui_io_getter_setter(duk_context *ctx) {
  duk_int_t magic;
  magic = duk_get_current_magic(ctx);
  CPR__DLOG("Getter/Setter magic : %d", magic);
  switch(magic) {
  case CPR__DISPLAY_SIZE_X_GETTER:
    duk_push_int(ctx, ImGui::GetIO().DisplaySize.x); return 1;
  case CPR__DISPLAY_SIZE_Y_GETTER:
    duk_push_int(ctx, ImGui::GetIO().DisplaySize.y); return 1;
  case CPR__DISPLAY_SIZE_X_SETTER:
    ImGui::GetIO().DisplaySize.x = duk_require_int(ctx, 0); return 0;
  case CPR__DISPLAY_SIZE_Y_SETTER:
    ImGui::GetIO().DisplaySize.y = duk_require_int(ctx, 0); return 0;
  case CPR__DELTA_TIME_GETTER:
    duk_push_number(ctx, ImGui::GetIO().DeltaTime); return 1;
  case CPR__DELTA_TIME_SETTER:
    ImGui::GetIO().DeltaTime = duk_require_number(ctx, 0); return 0;
  case CPR__MOUSE_X_GETTER:
    duk_push_number(ctx, ImGui::GetIO().MousePos.x); return 1;
  case CPR__MOUSE_Y_GETTER:
    duk_push_number(ctx, ImGui::GetIO().MousePos.y); return 1;
  case CPR__MOUSE_X_SETTER:
    ImGui::GetIO().MousePos.x = duk_require_number(ctx, 0); return 0;
  case CPR__MOUSE_Y_SETTER:
    ImGui::GetIO().MousePos.y = duk_require_number(ctx, 0); return 0;
  default:
    ERR(ctx, "Unknown magic value : %d", magic);
    duk_push_undefined(ctx);
  }
  return 1;
}

#define CPR__IO_GETTER_SETTER(__name__, __idx__, __getter__, __setter__) do { \
  duk_push_string(ctx, __name__); \
  duk_push_c_function(ctx, cpr_imgui_io_getter_setter, 0); \
  duk_set_magic(ctx, -1, __getter__); \
  duk_push_c_function(ctx, cpr_imgui_io_getter_setter, 1); \
  duk_set_magic(ctx, -1, __setter__); \
  duk_def_prop(ctx, __idx__, DUK_DEFPROP_HAVE_GETTER | DUK_DEFPROP_HAVE_SETTER); \
} while(0)

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
  CPR__IO_GETTER_SETTER("x", -4, CPR__DISPLAY_SIZE_X_GETTER, CPR__DISPLAY_SIZE_X_SETTER);
  /* ImGuiIO.DisplaySize.y */
  CPR__IO_GETTER_SETTER("y", -4, CPR__DISPLAY_SIZE_Y_GETTER, CPR__DISPLAY_SIZE_Y_SETTER);
  duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE); /* Define prop "ImGuiIO.displaySize" */
  /* ImGuiIO.DeltaTime */
  CPR__IO_GETTER_SETTER("deltaTime", -4, CPR__DELTA_TIME_GETTER, CPR__DELTA_TIME_SETTER);
  /* ImGuiIO.MousePos */
  duk_push_string(ctx, "mousePos");
  duk_push_object(ctx);
  /* ImGuiIO.MousePos.x */
  CPR__IO_GETTER_SETTER("x", -4, CPR__MOUSE_X_GETTER, CPR__MOUSE_X_SETTER);
  /* ImGuiIO.MousePos.y */
  CPR__IO_GETTER_SETTER("y", -4, CPR__MOUSE_Y_GETTER, CPR__MOUSE_Y_SETTER);
  duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE); /* Define prop "ImGuiIO.mousePos" */
  /* ImGuiIO.RenderDrawListsFn */
  CPR__IO_GETTER_SETTER("renderDrawListsFn", -4, CPR__RENDER_DRAW_LISTS_FN_GETTER, CPR__RENDER_DRAW_LISTS_FN_SETTER);

  duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE); /* Define prop "module.ImGuiIO" */
  /* Push/return the `ImGuiIO` object */
  duk_get_prop_string(ctx, -1, "ImGuiIO");
  CPR__DUMP_CONTEXT(ctx);
  return 1;
}

static duk_ret_t cpr_imgui_new_frame(duk_context *ctx) {
  // ImGui::NewFrame();
  ImGui_ImplGlfwGL3_NewFrame();
  return 0;
}

static duk_ret_t cpr_imgui_shutdown(duk_context *ctx) {
  // ImGui::Shutdown();
  ImGui_ImplGlfwGL3_Shutdown();
  return 0;
}

static duk_ret_t cpr_imgui_render(duk_context *ctx) {
  ImGui::Render();
  return 0;
}

static duk_ret_t cpr_imgui_begin(duk_context *ctx) {
  ImGui::Begin(duk_require_string(ctx, 0));
  return 0;
}

static duk_ret_t cpr_imgui_text(duk_context *ctx) {
  /* TODO handle variable arguments */
  int i = 0, args = 0;
  duk_int_t magic, top = 0;
  magic = duk_get_current_magic(ctx);
  top = duk_get_top(ctx);
  if (magic == 2) {
    /* TextColored first 4 parameters are the RGBA color */
    args = 4;
  }
  CPR__DLOG("top %d magic %d idx %d", top, magic, i);
  for (i = args; i < top; ++i) {
    duk_safe_to_string(ctx, i);
  }
  duk_concat(ctx, top - args);
  switch(duk_get_current_magic(ctx)) {
    /* Use a formated literal string to avoid warning */
    case 1: ImGui::Text("%s", duk_get_string(ctx, -1)); break;
    case 2: ImGui::TextColored(cpr_imgui_imvec4(ctx, 3), "%s", duk_require_string(ctx, -1)); break;
    case 3: ImGui::TextDisabled("%s", duk_get_string(ctx, -1)); break;
    case 4: ImGui::TextWrapped("%s", duk_get_string(ctx, -1)); break;
    default: CPR__DLOG("Unknown magic %d", duk_get_current_magic(ctx)); break;
  }

  return 0;
}

static duk_ret_t cpr_imgui_end(duk_context *ctx) {
  ImGui::End();
  return 0;
}

static duk_ret_t cpr_imgui_show_user_guide(duk_context *ctx) {
  ImGui::ShowUserGuide();
  return 0;
}

static duk_ret_t cpr_imgui_show_test_window(duk_context *ctx) {
  bool opened = true;
  ImGui::ShowTestWindow(&opened);
  duk_push_boolean(ctx, (int)opened);
  return 1;
}

/* Initialize ImGui. Must be called before any Imgui API calls but after the OpenGL
 * context is created and current (GLFW) and the OpenGL core profile is loaded (GL3W
 * init).
 */
static duk_ret_t cpr_imgui_init(duk_context *ctx) {
  duk_push_boolean(ctx, ImGui_ImplGlfwGL3_Init(
    (GLFWwindow*)duk_require_pointer(ctx, 0), duk_require_boolean(ctx, 1)));
  return 1;
}

/* Helpers */

/* ImVec4 */
static ImVec4 cpr_imgui_imvec4(duk_context *ctx, duk_idx_t idx) {
  return ImVec4(
    duk_require_number(ctx, idx - 3),
    duk_require_number(ctx, idx - 2),
    duk_require_number(ctx, idx - 1),
    duk_require_number(ctx, idx));
}

/* ImVec4 */
static ImVec2 cpr_imgui_imvec2(duk_context *ctx, duk_idx_t idx) {
  return ImVec2(
    duk_require_number(ctx, idx - 1),
    duk_require_number(ctx, idx));
}

duk_ret_t dukopen_imgui(duk_context *ctx) {
  const cpr_function_list_magic_entry module_funcs[] = {
    { "init",                     cpr_imgui_init,                       2, 0 },
    { "shutdown",                 cpr_imgui_shutdown,                   0, 0 },
    { "getIO",                    cpr_imgui_get_io,                     1, 0 },
    { "newFrame",                 cpr_imgui_new_frame,                  0, 0 },
    { "render",                   cpr_imgui_render,                     0, 0 },
    { "end",                      cpr_imgui_end,                        0, 0 },
    { "begin",                    cpr_imgui_begin,                      1, 0 },
    { "showUserGuide",            cpr_imgui_show_user_guide,            0, 0 },
    { "showTestWindow",           cpr_imgui_show_test_window,           0, 0 },
    { "text",                     cpr_imgui_text,                       DUK_VARARGS, 1 },
    { "textColored",              cpr_imgui_text,                       DUK_VARARGS, 2 },
    { "textDisabled",             cpr_imgui_text,                       DUK_VARARGS, 3 },
    { "textWrapped",              cpr_imgui_text,                       DUK_VARARGS, 4 },
    { NULL, NULL, 0 }
  };

  const duk_number_list_entry module_consts[] = {
    /* { "BAR", (double)(-1) }, */
    { NULL, 0.0 }
  };

  duk_push_object(ctx);
  cpr_put_function_list_magic(ctx, -1, module_funcs);
  duk_put_number_list(ctx, -1, module_consts);

  return 1;
}
