/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
#include "natrix/parser/source.h"

TEST(SourceTest, NormalizeString) {
    Source source;
    source_from_string(&source, "<string>", "a\nb\rc\r\r\nd");
    EXPECT_STREQ(source.filename, "<string>");
    EXPECT_STREQ(source.start, "a\nb\nc\n\nd\n");
    EXPECT_STREQ(source.end, source.start + 9);
    EXPECT_EQ(*source.end, '\0');
    source_free(&source);
}

TEST(SourceTest, EmptyString) {
    Source source;
    source_from_string(&source, "<string>", "");
    EXPECT_STREQ(source.start, "\n");
    EXPECT_STREQ(source.end, source.start + 1);
    source_free(&source);
}

TEST(SourceTest, TrailingNl) {
    Source source;
    source_from_string(&source, "<string>", "abc\n");
    EXPECT_STREQ(source.start, "abc\n");
    EXPECT_STREQ(source.end, source.start + 4);
    source_free(&source);
}

TEST(SourceTest, TrailingCr) {
    Source source;
    source_from_string(&source, "<string>", "abc\r");
    EXPECT_STREQ(source.start, "abc\n");
    EXPECT_STREQ(source.end, source.start + 4);
    source_free(&source);
}

TEST(SourceTest, FromFile) {
    Source source;
    EXPECT_TRUE(source_from_file(&source, "test_source_1.ntx"));
    EXPECT_STREQ(source.filename, "test_source_1.ntx");
    EXPECT_STREQ(source.start, "abc\n");
    EXPECT_STREQ(source.end, source.start + 4);
    EXPECT_EQ(*source.end, '\0');
    source_free(&source);
}

TEST(SourceTest, FromFileFail) {
    Source source;
    EXPECT_FALSE(source_from_file(&source, "no_such_file.ntx"));
    EXPECT_EQ(source.filename, nullptr);
    EXPECT_EQ(source.start, nullptr);
    EXPECT_EQ(source.end, nullptr);
    source_free(&source);
}
