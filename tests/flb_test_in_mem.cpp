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

#include <gtest/gtest.h>
#include <fluent-bit.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t result_mutex;
int result;

int callback_test(void* data, size_t size)
{
    if (size > 0) {
        free(data);
        pthread_mutex_lock(&result_mutex);
        result = 1;/* success */
        pthread_mutex_unlock(&result_mutex);
    }
    return 0;
}

TEST(Inputs, flush_5s) 
{
    int           ret    = 0;
    flb_ctx_t    *ctx    = NULL;
    flb_input_t  *input  = NULL;
    flb_output_t *output = NULL;

    /* initialize */
    ret = pthread_mutex_init(&result_mutex, NULL);
    result = 0;
    EXPECT_EQ(ret, 0);

    ctx = flb_create();

    input = flb_input(ctx, (char *) "mem", NULL);
    EXPECT_TRUE(input != NULL);
    flb_input_set(input, "tag", "test", NULL);

    output = flb_output(ctx, (char *) "lib", (void*)callback_test);
    EXPECT_TRUE(output != NULL);
    flb_output_set(output, "match", "test", NULL);

    ret = flb_start(ctx);
    EXPECT_EQ(ret, 0);

    /* start test */
    sleep(2);
    pthread_mutex_lock(&result_mutex);
    ret = result; /* 2sec passed, no data should be flushed */
    pthread_mutex_unlock(&result_mutex);
    EXPECT_EQ(ret, 0);

    sleep(3);
    pthread_mutex_lock(&result_mutex);
    ret = result; /* 5sec passed, data should be flushed */
    pthread_mutex_unlock(&result_mutex);
    EXPECT_EQ(ret, 1);


    /* finalize */
    flb_stop(ctx);
    flb_destroy(ctx);

    ret = pthread_mutex_destroy(&result_mutex);
    EXPECT_EQ(ret, 0);
}
