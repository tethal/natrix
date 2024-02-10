/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
#include "natrix/parser/ast.h"

TEST(AstTest, EmptySource) {
    Arena arena = arena_init();
    const char *src = " 1 + abc ";
    Expr *expr_1 = ast_create_expr_int_literal(&arena, src + 1, src + 2);
    Expr *expr_abc = ast_create_expr_int_literal(&arena, src + 5, src + 8);
    Expr *expr = ast_create_expr_binary(&arena, expr_1, BINOP_ADD, expr_abc);
    EXPECT_EQ(ast_get_expr_start(expr), src + 1);
    EXPECT_EQ(ast_get_expr_end(expr), src + 8);
    arena_free(&arena);
}
