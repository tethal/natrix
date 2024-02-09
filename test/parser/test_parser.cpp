/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
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

TEST(ParserTest, SimpleExpression) {
    Source src = source_from_string("<string>", "(10 - 3) * 6\n1");
    Arena arena = arena_init();
    Stmt *stmt = parse_file(&arena, &src, diag_default_handler, nullptr);
    StringBuilder sb = sb_init();
    ast_dump(&sb, stmt);
    EXPECT_STREQ(sb.str,
                 "AST dump:\n"
                 "  STMT_EXPR\n"
                 "    EXPR_BINARY {op: MUL}\n"
                 "      left: EXPR_BINARY {op: SUB}\n"
                 "        left: EXPR_INT_LITERAL {literal: \"10\"}\n"
                 "        right: EXPR_INT_LITERAL {literal: \"3\"}\n"
                 "      right: EXPR_INT_LITERAL {literal: \"6\"}\n"
                 "  STMT_EXPR\n"
                 "    EXPR_INT_LITERAL {literal: \"1\"}\n"
    );
    sb_free(&sb);
    arena_free(&arena);
    source_free(&src);
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
