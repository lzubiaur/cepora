/*
 * cpr_cepora.h
 * Copyright (c) 2015 Laurent Zubiaur
 * MIT License (http://opensource.org/licenses/MIT)
 */

#ifndef CPR_CEPORA_H
#define CPR_CEPORA_H

#include "cpr_config.h"
#include "duktape.h"

CPR_API_EXTERN void cpr_set_default_log_level(duk_context *ctx, unsigned short level);
CPR_API_EXTERN int cpr_start(int argc, char *argv[]);

#endif /* CPR_CEPORA_H */
