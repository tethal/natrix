/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
#include "natrix/obj/nx_bool.h"
#include "natrix/obj/nx_int.h"
#include "natrix/obj/nx_list.h"
#include "../gc_state.h"

TEST(NxListTest, Append) {
    GcStateW gc_state;
    NxObject *list = nx_list_create(1);
    nxo_root(list);
    EXPECT_TRUE(nx_list_is_instance(list));
    EXPECT_EQ(((NxList *) list)->items->size, 1);
    EXPECT_EQ(((NxList *) list)->items->data[0], nullptr);
    EXPECT_EQ(nx_list_get_length(list), 0);
    NxObject *obj = nx_int_create(1234);
    nxo_root(obj);
    nx_list_append(list, obj);
    nxo_unroot(obj);
    EXPECT_EQ(nx_list_get_length(list), 1);
    nx_list_append(list, list);
    EXPECT_EQ(nx_list_get_length(list), 2);
    EXPECT_EQ(((NxList *) list)->items->size, 3);
    EXPECT_EQ(((NxList *) list)->items->data[0], obj);
    EXPECT_EQ(((NxList *) list)->items->data[1], list);
    EXPECT_TRUE(gc_state.check_count(4));
    gc_collect();
    EXPECT_TRUE(gc_state.is_valid(list));
    EXPECT_TRUE(gc_state.is_valid(((NxList *) list)->items));
    EXPECT_TRUE(gc_state.is_valid(obj));
    EXPECT_TRUE(gc_state.check_count(3));
    nxo_unroot(list);
    gc_collect();
    EXPECT_FALSE(gc_state.is_valid(list));
    EXPECT_FALSE(gc_state.is_valid(obj));
    EXPECT_TRUE(gc_state.check_count(0));
}

TEST(NxListTest, AsBool) {
    NxObject *list = nx_list_create(1);
    nxo_root(list);
    EXPECT_EQ(nxo_as_bool(list), nx_false);
    NxObject *obj = nx_int_create(1234);
    nxo_root(obj);
    nx_list_append(list, obj);
    nxo_unroot(obj);
    EXPECT_EQ(nxo_as_bool(list), nx_true);
    nxo_unroot(list);
}
