/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
#include "natrix/parser/source.h"

TEST(SourceTest, NormalizeString) {
    Source source = source_from_string("<string>", "a\nb\rc\r\r\nd");
    EXPECT_STREQ(source.filename, "<string>");
    EXPECT_STREQ(source.start, "a\nb\nc\n\nd\n");
    EXPECT_STREQ(source.end, source.start + 9);
    EXPECT_EQ(*source.end, '\0');
    source_free(&source);
}

TEST(SourceTest, EmptyString) {
    Source source = source_from_string("<string>", "");
    EXPECT_STREQ(source.start, "\n");
    EXPECT_STREQ(source.end, source.start + 1);
    source_free(&source);
}

TEST(SourceTest, TrailingNl) {
    Source source = source_from_string("<string>", "abc\n");
    EXPECT_STREQ(source.start, "abc\n");
    EXPECT_STREQ(source.end, source.start + 4);
    source_free(&source);
}

TEST(SourceTest, TrailingCr) {
    Source source = source_from_string("<string>", "abc\r");
    EXPECT_STREQ(source.start, "abc\n");
    EXPECT_STREQ(source.end, source.start + 4);
    source_free(&source);
}

TEST(SourceTest, FromFile) {
    Source source = source_from_file("test_source_1.ntx");
    EXPECT_STREQ(source.filename, "test_source_1.ntx");
    EXPECT_STREQ(source.start, "abc\n");
    EXPECT_STREQ(source.end, source.start + 4);
    EXPECT_EQ(*source.end, '\0');
    EXPECT_EQ(source.line_starts, nullptr);
    EXPECT_EQ(source.line_count, 2);
    source_free(&source);
}

TEST(SourceTest, FromFileFail) {
    Source source = source_from_file("no_such_file.ntx");
    EXPECT_EQ(source.filename, nullptr);
    EXPECT_EQ(source.start, nullptr);
    EXPECT_EQ(source.end, nullptr);
    EXPECT_EQ(source.line_starts, nullptr);
    EXPECT_EQ(source.line_count, 0);
    source_free(&source);
}

TEST(SourceTest, GetLineNumber) {
    Source source = source_from_string("<string>", "\na\nb\n\nc\n");
    EXPECT_EQ(source.line_count, 6);
    EXPECT_EQ(source_get_line_number(&source, source.start + 0), 1);
    EXPECT_EQ(source_get_line_number(&source, source.start + 1), 2);
    EXPECT_EQ(source_get_line_number(&source, source.start + 2), 2);
    EXPECT_EQ(source_get_line_number(&source, source.start + 3), 3);
    EXPECT_EQ(source_get_line_number(&source, source.start + 4), 3);
    EXPECT_EQ(source_get_line_number(&source, source.start + 5), 4);
    EXPECT_EQ(source_get_line_number(&source, source.start + 6), 5);
    EXPECT_EQ(source_get_line_number(&source, source.start + 7), 5);
    EXPECT_EQ(source_get_line_number(&source, source.start + 8), 6);
    source_free(&source);
}

TEST(SourceTest, GetLineStartEnd) {
    Source source = source_from_string("<string>", "\na\nb\n\nc\n");
    EXPECT_EQ(source_get_line_start(&source, 1), source.start + 0);
    EXPECT_EQ(source_get_line_end(&source, 1), source.start + 0);
    EXPECT_EQ(source_get_line_start(&source, 2), source.start + 1);
    EXPECT_EQ(source_get_line_end(&source, 2), source.start + 2);
    EXPECT_EQ(source_get_line_start(&source, 3), source.start + 3);
    EXPECT_EQ(source_get_line_end(&source, 3), source.start + 4);
    EXPECT_EQ(source_get_line_start(&source, 4), source.start + 5);
    EXPECT_EQ(source_get_line_end(&source, 4), source.start + 5);
    EXPECT_EQ(source_get_line_start(&source, 5), source.start + 6);
    EXPECT_EQ(source_get_line_end(&source, 5), source.start + 7);
    EXPECT_EQ(source_get_line_start(&source, 6), source.start + 8);
    EXPECT_EQ(source_get_line_end(&source, 6), source.start + 8);
    source_free(&source);
}
