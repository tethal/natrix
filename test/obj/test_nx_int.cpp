/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
#include "natrix/obj/nx_bool.h"
#include "natrix/obj/nx_int.h"

TEST(NxIntTest, Cached) {
    NxObject *a = nx_int_create(42);
    NxObject *b = nx_int_create(42);
    NxObject *c = nx_int_create(43);
    EXPECT_EQ(a, b);
    EXPECT_NE(a, c);
    EXPECT_TRUE(nx_int_is_instance(a));
    EXPECT_EQ(nx_int_get_value(a), 42);
}

TEST(NxIntTest, Uncached) {
    NxObject *a = nx_int_create(-1234);
    nxo_root(a);
    NxObject *b = nx_int_create(-1234);
    nxo_unroot(a);
    EXPECT_NE(a, b);
    EXPECT_TRUE(nx_int_is_instance(a));
    EXPECT_EQ(nx_int_get_value(a), -1234);
}

TEST(NxIntTest, AsBool) {
    NxObject *a = nx_int_create(0);
    NxObject *b = nx_int_create(42);
    EXPECT_EQ(nxo_as_bool(a), nx_false);
    EXPECT_EQ(nxo_as_bool(b), nx_true);
}
