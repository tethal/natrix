/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
#include "natrix/util/sb.h"

TEST(StringBuilderTest, Empty) {
    StringBuilder sb;
    sb_init(&sb);
    EXPECT_STREQ(sb.str, "");
    EXPECT_EQ(sb.length, 0);
    EXPECT_EQ(sb.capacity, 16);
    sb_free(&sb);
}

TEST(StringBuilderTest, AppendChar) {
    StringBuilder sb;
    sb_init_with_capacity(&sb, 1);
    sb_append_char(&sb, 'a');
    EXPECT_STREQ(sb.str, "a");
    EXPECT_EQ(sb.length, 1);
    EXPECT_EQ(sb.capacity, 2);
    sb_free(&sb);
}

TEST(StringBuilderTest, AppendStr) {
    StringBuilder sb;
    sb_init_with_capacity(&sb, 1);
    sb_append_str(&sb, "hello");
    EXPECT_STREQ(sb.str, "hello");
    EXPECT_EQ(sb.length, 5);
    EXPECT_EQ(sb.capacity, 6);
    sb_free(&sb);
}

TEST(StringBuilderTest, AppendEscapedStr) {
    StringBuilder sb;
    sb_init(&sb);
    sb_append_escaped_str(&sb, "hello\\\"world\"\n");
    EXPECT_STREQ(sb.str, "hello\\\\\\\"world\\\"\\n");
    EXPECT_EQ(sb.length, 18);
    EXPECT_EQ(sb.capacity, 24);
    sb_free(&sb);
}

TEST(StringBuilderTest, AppendFormatted) {
    StringBuilder sb;
    sb_init(&sb);
    sb_append_formatted(&sb, "hello %d %s", 42, "world");
    EXPECT_STREQ(sb.str, "hello 42 world");
    EXPECT_EQ(sb.length, 14);
    EXPECT_EQ(sb.capacity, 16);
    sb_free(&sb);
}

TEST(StringBuilderTest, AppendAll) {
    StringBuilder sb;
    sb_init_with_capacity(&sb, 1);
    sb_append_str(&sb, "hello");
    EXPECT_EQ(sb.length, 5);
    EXPECT_EQ(sb.capacity, 6);
    sb_append_char(&sb, ' ');
    EXPECT_EQ(sb.length, 6);
    EXPECT_EQ(sb.capacity, 9);
    sb_append_formatted(&sb, "%d ", 42);
    EXPECT_EQ(sb.length, 9);
    EXPECT_EQ(sb.capacity, 13);
    sb_append_escaped_str(&sb, "world\n");
    EXPECT_STREQ(sb.str, "hello 42 world\\n");
    EXPECT_EQ(sb.length, 16);
    EXPECT_EQ(sb.capacity, 19);
    sb_free(&sb);
}
