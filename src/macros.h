#ifndef CEPORA_MACROS_H
#define CEPORA_MACROS_H

/* Logging macros */
#define TRC(ctx, ...) duk_log(ctx, DUK_LOG_TRACE, __VA_ARGS__)
#define DBG(ctx, ...) duk_log(ctx, DUK_LOG_DEBUG, __VA_ARGS__)
#define INF(ctx, ...) duk_log(ctx, DUK_LOG_INFO,  __VA_ARGS__)
#define WRN(ctx, ...) duk_log(ctx, DUK_LOG_WARN,  __VA_ARGS__)
#define ERR(ctx, ...) duk_log(ctx, DUK_LOG_ERROR, __VA_ARGS__)
#define FTL(ctx, ...) duk_log(ctx, DUK_LOG_FATAL, __VA_ARGS__)

#endif /* CEPORA_MACROS_H */
