/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
#include <filesystem>
#include <iostream>
#include <string>
#include <fstream>
#include "natrix/parser/parser.h"

static void diag_handler(void *data, DiagKind kind, Source *source, const char *start, const char *end, const char *fmt, ...) {
    size_t line = source_get_line_number(source, start);
    size_t column = start - source_get_line_start(source, line) + 1;
    size_t length = end - start;

    StringBuilder sb = sb_init();
    sb_append_formatted(&sb, "%s: %zu:%zu-%zu: ", kind == DIAG_WARNING ? "warning" : "error", line, column, length);

    va_list args;
    va_start(args, fmt);
    sb_append_formatted_v(&sb, fmt, args);
    va_end(args);

    auto records = (std::vector<std::string> *) data;
    records->emplace_back(sb.str);
    sb_free(&sb);
}

static std::vector<std::string> parse_and_capture_diags(const char *source) {
    std::vector<std::string> diags;
    Source src = source_from_string("<string>", source);
    Arena arena = arena_init();
    EXPECT_EQ(parse_file(&arena, &src, diag_handler, &diags), nullptr);
    arena_free(&arena);
    source_free(&src);
    return diags;
}

static std::string parse_and_capture_diag(const char *source) {
    std::vector<std::string> diags = parse_and_capture_diags(source);
    EXPECT_EQ(diags.size(), 1);
    return diags[0];
}

TEST(ParserTest, InvalidToken) {
    std::string diag = parse_and_capture_diag("(10 - 3) ` 6\n1");
    EXPECT_EQ(diag, "error: 1:10-1: unexpected character");
}

TEST(ParserTest, MissingParen) {
    std::string diag = parse_and_capture_diag("\n(10 - 3   # comment\n");
    EXPECT_EQ(diag, "error: 2:11-10: expected closing parenthesis");
}

TEST(ParserTest, ExpectedExpression) {
    std::string diag = parse_and_capture_diag("\n(10 -\n");
    EXPECT_EQ(diag, "error: 2:6-1: expected expression");
}

TEST(ParserTest, InvalidLhsOfAssignment) {
    std::string diag = parse_and_capture_diag("a + 3 = 1");
    EXPECT_EQ(diag, "error: 1:1-5: cannot assign to expression here");
}

TEST(ParserTest, InvalidRhsOfAssignment) {
    std::string diag = parse_and_capture_diag("a = )");
    EXPECT_EQ(diag, "error: 1:5-1: expected expression");
}

TEST(ParserTest, InvalidRhsOfMultiplication) {
    std::string diag = parse_and_capture_diag("a * 4 / /");
    EXPECT_EQ(diag, "error: 1:9-1: expected expression");
}

TEST(ParserTest, BlockNoNL) {
    std::string diag = parse_and_capture_diag("while a: 1");
    EXPECT_EQ(diag, "error: 1:10-1: newline expected");
}

TEST(ParserTest, BlockNoIndent) {
    std::string diag = parse_and_capture_diag("while a:\n1");
    EXPECT_EQ(diag, "error: 2:1-1: indent expected");
}

TEST(ParserTest, BlockErrBody) {
    std::string diag = parse_and_capture_diag("while a:\n  print");
    EXPECT_EQ(diag, "error: 2:8-1: expected '('");
}

TEST(ParserTest, WhileNoCond) {
    std::string diag = parse_and_capture_diag("while:\n  print(1)");
    EXPECT_EQ(diag, "error: 1:6-1: expected expression");
}

TEST(ParserTest, WhileNoColon) {
    std::string diag = parse_and_capture_diag("while 1\n  print(1)");
    EXPECT_EQ(diag, "error: 1:8-1: expected ':'");
}

TEST(ParserTest, PrintNoRParen) {
    std::string diag = parse_and_capture_diag("print(1");
    EXPECT_EQ(diag, "error: 1:8-1: expected ')'");
}

TEST(ParserTest, PrintNoExpr) {
    std::string diag = parse_and_capture_diag("print()");
    EXPECT_EQ(diag, "error: 1:7-1: expected expression");
}

TEST(ParserTest, ElseNoColon) {
    std::string diag = parse_and_capture_diag("if a:\n  print(1)\nelse\n  print(2)");
    EXPECT_EQ(diag, "error: 3:5-1: expected ':'");
}

TEST(ParserTest, ElseNoIndent) {
    std::string diag = parse_and_capture_diag("if a:\n  print(1)\nelse:\nprint(2)");
    EXPECT_EQ(diag, "error: 4:1-5: indent expected");
}

TEST(ParserTest, ElifNoCond) {
    std::string diag = parse_and_capture_diag("if a:\n  print(1)\nelif:\n  print(2)");
    EXPECT_EQ(diag, "error: 3:5-1: expected expression");
}

TEST(ParserTest, ElifNoColon) {
    std::string diag = parse_and_capture_diag("if a:\n  print(1)\nelif b\n  print(2)");
    EXPECT_EQ(diag, "error: 3:7-1: expected ':'");
}

TEST(ParserTest, ElifNoIndent) {
    std::string diag = parse_and_capture_diag("if a:\n  print(1)\nelif b:\nprint(2)");
    EXPECT_EQ(diag, "error: 4:1-5: indent expected");
}

TEST(ParserTest, GoldenFiles) {
    for (const auto & entry : std::filesystem::directory_iterator("parser")) {
        std::filesystem::path path = entry.path();
        if (path.extension() == ".ntx") {
            Source src = source_from_file(path.c_str());
            Arena arena = arena_init();
            Stmt *stmt = parse_file(&arena, &src, diag_default_handler, nullptr);
            StringBuilder sb = sb_init();
            ast_dump(&sb, stmt);
            Source expected = source_from_file(path.replace_extension("out").c_str());
            if (expected.start) {
                EXPECT_STREQ(expected.start, sb.str);
                source_free(&expected);
            } else {
                std::ofstream out(path.replace_extension("out"));
                out << sb.str;
                out.close();
            }
            sb_free(&sb);
            arena_free(&arena);
            source_free(&src);
        }
    }
}
