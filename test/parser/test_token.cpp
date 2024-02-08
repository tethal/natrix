/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
#include "natrix/parser/token.h"

TEST(TokenTest, GetTypeName) {
    EXPECT_STREQ(token_get_type_name(TOKEN_EOF), "EOF");
    EXPECT_STREQ(token_get_type_name(TOKEN_LPAREN), "LPAREN");
    EXPECT_STREQ(token_get_type_name(static_cast<TokenType>(-1)), "UNKNOWN");
    EXPECT_STREQ(token_get_type_name(static_cast<TokenType>(4321)), "UNKNOWN");
}

TEST(TokenTest, ToString) {
    const char *src = " 42 ";
    Token token = {
        .type = TOKEN_INT_LITERAL,
        .start = src + 1,
        .end = src + 3
    };
    StringBuilder sb = sb_init();
    token_to_string(&token, &sb);
    EXPECT_STREQ(sb.str, "INT_LITERAL(\"42\")");
    sb_free(&sb);
}
