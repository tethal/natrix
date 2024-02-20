/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
#include "../gc_state.h"

struct Leaf : GcHeader {
    int value;
};

struct Container : GcHeader {
    GcHeader *obj;
};

Leaf *alloc_leaf() {
    return (Leaf *) gc_alloc(sizeof(Leaf), nullptr);
}

void trace_container(void *ptr) {
    gc_visit(((Container *) ptr)->obj);
}

Container *alloc_container() {
    return (Container *) gc_alloc(sizeof(Container), trace_container);
}

TEST(GcTest, NoRoots) {
    GcStateW state;
    void *obj1 = alloc_leaf();
    void *obj2 = alloc_leaf();
    EXPECT_TRUE(state.is_valid(obj1));
    EXPECT_TRUE(state.is_valid(obj2));
    EXPECT_TRUE(state.check_count(2));
    gc_collect();
    EXPECT_FALSE(state.is_valid(obj1));
    EXPECT_FALSE(state.is_valid(obj2));
    EXPECT_TRUE(state.check_count(0));
}

TEST(GcTest, SingleRoot) {
    GcStateW state;
    Leaf *obj1 = alloc_leaf();
    gc_root(obj1);
    Leaf *obj2 = alloc_leaf();
    EXPECT_TRUE(state.is_valid(obj1));
    EXPECT_TRUE(state.is_valid(obj2));
    EXPECT_TRUE(state.check_count(2));
    gc_collect();
    EXPECT_TRUE(state.is_valid(obj1));
    EXPECT_FALSE(state.is_valid(obj2));
    EXPECT_TRUE(state.check_count(1));
    gc_unroot(obj1);
    gc_collect();
    EXPECT_FALSE(state.is_valid(obj1));
    EXPECT_FALSE(state.is_valid(obj2));
    EXPECT_TRUE(state.check_count(0));
}

TEST(GcTest, Threshold) {
    GcStateW state;
    for (size_t i = 0; i < state.threshold(); i++) {
        Leaf *obj = alloc_leaf();
        EXPECT_TRUE(state.is_valid(obj));
    }
    EXPECT_TRUE(state.check_count(100));
    Leaf *obj = alloc_leaf();
    EXPECT_TRUE(state.check_count(1));
    EXPECT_TRUE(state.is_valid(obj));
    gc_collect();
    EXPECT_FALSE(state.is_valid(obj));
    EXPECT_TRUE(state.check_count(0));
}

TEST(GcTest, IndirectlyReachable) {
    GcStateW state;
    Container *list = alloc_container();
    gc_root(list);
    Leaf *obj1 = alloc_leaf();
    list->obj = obj1;
    Leaf *obj2 = alloc_leaf();
    EXPECT_TRUE(state.is_valid(list));
    EXPECT_TRUE(state.is_valid(obj1));
    EXPECT_TRUE(state.is_valid(obj2));
    EXPECT_TRUE(state.check_count(3));
    gc_collect();
    EXPECT_TRUE(state.is_valid(list));
    EXPECT_TRUE(state.is_valid(obj1));
    EXPECT_FALSE(state.is_valid(obj2));
    EXPECT_TRUE(state.check_count(2));
    gc_unroot(list);
    gc_collect();
    EXPECT_FALSE(state.is_valid(list));
    EXPECT_FALSE(state.is_valid(obj1));
    EXPECT_TRUE(state.check_count(0));
}

TEST(GcTest, Cycle) {
    GcStateW state;
    Container *list1 = alloc_container();
    gc_root(list1);
    Container *list2 = alloc_container();
    list1->obj = list2;
    list2->obj = list1;
    gc_collect();
    EXPECT_TRUE(state.is_valid(list1));
    EXPECT_TRUE(state.is_valid(list2));
    EXPECT_TRUE(state.check_count(2));
    gc_unroot(list1);
    gc_collect();
    EXPECT_FALSE(state.is_valid(list1));
    EXPECT_FALSE(state.is_valid(list2));
    EXPECT_TRUE(state.check_count(0));
}
