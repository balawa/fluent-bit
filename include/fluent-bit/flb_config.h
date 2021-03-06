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

#ifndef FLB_CONFIG_H
#define FLB_CONFIG_H

#include <time.h>
#include <mk_core.h>

#include <fluent-bit/flb_info.h>
#include <fluent-bit/flb_log.h>
#include <fluent-bit/flb_task_map.h>

#ifdef FLB_HAVE_TLS
#include <fluent-bit/flb_io_tls.h>
#endif

#define FLB_FLUSH_UCONTEXT      0
#define FLB_FLUSH_PTHREADS      1

#define FLB_CONFIG_FLUSH_SECS   5
#define FLB_CONFIG_HTTP_PORT    "2020"
#define FLB_CONFIG_DEFAULT_TAG  "fluent_bit"

/* Property configuration: key/value for an input/output instance */
struct flb_config_prop {
    char *key;
    char *val;
    struct mk_list _head;
};

/* Main struct to hold the configuration of the runtime service */
struct flb_config {
    struct mk_event ch_event;

    int flush;          /* Flush timeout                  */
    int flush_fd;       /* Timer FD associated to flush   */
    int flush_method;   /* Flush method set at build time */

    int daemon;         /* Run as a daemon ?              */
    int shutdown_fd;    /* Shutdown FD, 5 seconds         */

#ifdef FLB_HAVE_STATS
    int stats_fd;       /* Stats FD, 1 second             */
    struct flb_stats *stats_ctx;
#endif

    int verbose;        /* Verbose mode (default OFF)     */
    time_t init_time;   /* Time when Fluent Bit started   */

    /* Used in library mode */
    pthread_t worker;   /* worker tid */
    int ch_data[2];     /* pipe to communicate caller with worker */
    int ch_manager[2];  /* channel to administrate fluent bit     */
    int ch_notif[2];    /* channel to receive notifications       */

    /* Channel event loop (just for ch_notif) */
    struct mk_event_loop *ch_evl;

    struct mk_rconf *file;

    /* Event */
    struct mk_event event_flush;
    struct mk_event event_shutdown;

    /* Collectors */
    struct mk_list collectors;

    /* Input and Output plugins */
    struct mk_list in_plugins;
    struct mk_list out_plugins;

    /* Inputs instances */
    struct mk_list inputs;

    /* Outputs instances */
    struct mk_list outputs;             /* list of output plugins   */
    struct flb_output_plugin *output;   /* output plugin in use     */
    struct mk_event_loop *evl;          /* the event loop (mk_core) */

    /* Kernel info */
    struct flb_kernel *kernel;

    /* Logging */
    struct flb_log *log;

    /* HTTP Server */
#ifdef FLB_HAVE_HTTP
    int http_server;
    char *http_port;
    void *http_ctx;
#endif

#ifdef FLB_HAVE_BUFFERING
    struct flb_buffer *buffer_ctx;
    int buffer_workers;
    char *buffer_path;
#endif

    struct mk_list sched_requests;
    struct flb_task_map tasks_map[2048];
};

struct flb_config *flb_config_init();
void flb_config_exit(struct flb_config *config);
char *flb_config_prop_get(char *key, struct mk_list *list);

#endif
