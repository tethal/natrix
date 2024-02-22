/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
#include "natrix/obj/nx_bool.h"

TEST(NxBoolTest, Basic) {
    EXPECT_EQ(nx_bool_wrap(true), nx_true);
    EXPECT_EQ(nx_bool_wrap(false), nx_false);
    EXPECT_TRUE(nx_bool_is_instance(nx_true));
    EXPECT_TRUE(nx_bool_is_true(nx_true));
    EXPECT_TRUE(nx_bool_is_instance(nx_false));
    EXPECT_FALSE(nx_bool_is_true(nx_false));
}

TEST(NxBoolTest, AsBool) {
    EXPECT_EQ(nxo_as_bool(nx_true), nx_true);
    EXPECT_EQ(nxo_as_bool(nx_false), nx_false);
}
