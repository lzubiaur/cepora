/*
 * cpr_dummy.h
 * Copyright (c) 2015 Laurent Zubiaur
 * MIT License (http://opensource.org/licenses/MIT)
 */

#ifndef CPR_DUMMY_H
#define CPR_DUMMY_H

#include "duktape.h"

#ifdef __cplusplus
extern "C" {
#endif

duk_ret_t dukopen_dummy(duk_context *ctx);

#ifdef __cplusplus
}
#endif

#endif /* CPR_DUMMY_H */
