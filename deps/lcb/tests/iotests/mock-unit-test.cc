/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 *     Copyright 2012 Couchbase, Inc.
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */
#include "config.h"
#include "iotests.h"
#include <map>

#include "internal.h" /* vbucket_* things from lcb_t */
#include <lcbio/iotable.h>
#include "bucketconfig/bc_http.h"

#define LOGARGS(instance, lvl) \
    instance->settings, "tests-MUT", LCB_LOG_##lvl, __FILE__, __LINE__

/**
 * Keep these around in case we do something useful here in the future
 */
void MockUnitTest::SetUp() {
    MockEnvironment::Reset();
}

extern "C" {
    static void bootstrap_callback(lcb_t instance, lcb_error_t err) {
        if (err != LCB_SUCCESS) {
            std::cerr << "Error " << lcb_strerror(instance, err) << std::endl;
        }
        EXPECT_EQ(LCB_SUCCESS, err);
    }
}

void MockUnitTest::createConnection(HandleWrap &handle, lcb_t &instance)
{
    MockEnvironment::getInstance()->createConnection(handle, instance);
    (void)lcb_set_bootstrap_callback(handle.getLcb(), bootstrap_callback);
    ASSERT_EQ(LCB_SUCCESS, lcb_connect(handle.getLcb()));
    lcb_wait(handle.getLcb());
}

void MockUnitTest::createConnection(lcb_t &instance)
{
    MockEnvironment::getInstance()->createConnection(instance);
    (void)lcb_set_bootstrap_callback(instance, bootstrap_callback);
    ASSERT_EQ(LCB_SUCCESS, lcb_connect(instance));
    lcb_wait(instance);
}

void MockUnitTest::createConnection(HandleWrap &handle)
{
    lcb_t instance = NULL;
    createConnection(handle, instance);
}
