/*
 * cpr_package.h
 * Copyright (c) 2015 Laurent Zubiaur
 * MIT License (http://opensource.org/licenses/MIT)
 */

#ifndef CPR_PACKAGE_H
#define CPR_PACKAGE_H

#include "duktape.h"
#include "cpr_config.h"

#define CPR_PACKAGE_NAME "module"

#ifdef __cplusplus
extern "C" {
#endif

CPR_API_EXTERN duk_ret_t dukopen_package(duk_context *ctx);

#ifdef __cplusplus
}
#endif

#endif /* CPR_PACKAGE_H */
