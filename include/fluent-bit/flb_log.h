/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*  Fluent Bit
 *  ==========
 *  Copyright (C) 2015-2016 Treasure Data Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef FLB_LOG_H
#define FLB_LOG_H

#include <inttypes.h>

#include <fluent-bit/flb_info.h>
#include <fluent-bit/flb_macros.h>

#ifdef FLB_HAVE_C_TLS
#define FLB_TLS_SET(key, val)        key=val
#define FLB_TLS_GET(key)             key
#define FLB_TLS_INIT()               do {} while (0)
extern __thread struct flb_log *flb_log_ctx;
#else
pthread_key_t flb_log_ctx;
#define FLB_TLS_SET(key, val)  pthread_setspecific(key, (void *) val)
#define FLB_TLS_GET(key)       pthread_getspecific(key)
#define FLB_TLS_INIT()                                  \
    pthread_key_create(&flb_log_ctx, NULL);
#endif

/* Message types */
#define FLB_LOG_OFF     0
#define FLB_LOG_ERROR   1
#define FLB_LOG_WARN    2
#define FLB_LOG_INFO    3  /* default */
#define FLB_LOG_DEBUG   4
#define FLB_LOG_TRACE   5

/* Logging outputs */
#define FLB_LOG_STDERR   0  /* send logs to STDERR         */
#define FLB_LOG_FILE     1  /* write logs to a file        */
#define FLB_LOG_SOCKET   2  /* write logs to a unix socket */


/* Logging main context */
struct flb_log {
    uint16_t type;          /* log type */
    uint16_t level;         /* level    */
    char *out;              /* FLB_LOG_FILE or FLB_LOG_SOCKET */
};

/*
int flb_log_check(int level) {
    struct flb_log *lc = FLB_TLS_GET(flb_log_ctx);
    if (lc->level < level)
        return FLB_FALSE;
    else
        return FLB_TRUE;
}
*/

#ifdef FLB_HAVE_C_TLS
#define flb_log_check(l)                                            \
    (FLB_TLS_GET(flb_log_ctx)->level < l) ? FLB_FALSE: FLB_TRUE
#else
int flb_log_check(int level);
#endif

struct flb_log *flb_log_init(int type, int level, char *out);
int flb_log_stop(struct flb_log *log);
void flb_log_print(int type, const char *file, int line, const char *fmt, ...);

/* Logging macros */
#define flb_error(fmt, ...)                                          \
    if (flb_log_check(FLB_LOG_ERROR))                                \
        flb_log_print(FLB_LOG_ERROR, NULL, 0, fmt, ##__VA_ARGS__)

#define flb_warn(fmt, ...)                                           \
    if (flb_log_check(FLB_LOG_WARN))                                 \
        flb_log_print(FLB_LOG_WARN, NULL, 0, fmt, ##__VA_ARGS__)

#define flb_info(fmt, ...)                                           \
    if (flb_log_check(FLB_LOG_INFO))                                 \
        flb_log_print(FLB_LOG_INFO, NULL, 0, fmt, ##__VA_ARGS__)

#define flb_debug(fmt, ...)                                         \
    if (flb_log_check(FLB_LOG_DEBUG))                               \
        flb_log_print(FLB_LOG_DEBUG, NULL, 0, fmt, ##__VA_ARGS__)

#ifdef FLB_HAVE_TRACE
#define flb_trace(fmt, ...)                                             \
    if (flb_log_check(FLB_LOG_TRACE))                                   \
        flb_log_print(FLB_LOG_TRACE, __FILE__, __LINE__,                \
                      fmt, ##__VA_ARGS__)
#else
#define flb_trace(fmt, ...)  do {} while(0)
#endif


#endif
