/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
#include "natrix/obj/nx_bool.h"
#include "natrix/obj/nx_str.h"

TEST(NxStrTest, CreateAddsNullTerminator) {
    NxObject *str = nx_str_create("Abcd", 3);
    EXPECT_TRUE(nx_str_is_instance(str));
    EXPECT_EQ(((NxStr *) str)->length, 3);
    EXPECT_EQ(((NxStr *) str)->data[3], '\0');
    EXPECT_STREQ(((NxStr *) str)->data, "Abc");
}

TEST(NxStrTest, Concat) {
    NxObject *str1 = nx_str_create("Abc", 3);
    nxo_root(str1);
    NxObject *str2 = nx_str_create("Def", 3);
    nxo_root(str2);
    NxObject *result = nx_str_concat(str1, str2);
    EXPECT_TRUE(nx_str_is_instance(result));
    EXPECT_EQ(((NxStr *) result)->length, 6);
    EXPECT_EQ(((NxStr *) result)->data[6], '\0');
    EXPECT_STREQ(((NxStr *) result)->data, "AbcDef");
    nxo_unroot(str2);
    nxo_unroot(str1);
}

TEST(NxStrTest, AsBool) {
    NxObject *str1 = nx_str_create("", 0);
    EXPECT_EQ(nxo_as_bool(str1), nx_false);
    NxObject *str2 = nx_str_create("Abc", 3);
    EXPECT_EQ(nxo_as_bool(str2), nx_true);
}
