/*
 * cpr_loadlib.h
 * Copyright (c) 2015 Laurent Zubiaur
 * MIT License (http://opensource.org/licenses/MIT)
 */

#ifndef CPR_LOADLIB_H
#define CPR_LOADLIB_H

#include "duktape.h"

duk_ret_t dukopen_loadlib(duk_context *ctx);
duk_ret_t cpr_loadlib(duk_context *ctx);

#endif /* CPR_LOADLIB_H */
