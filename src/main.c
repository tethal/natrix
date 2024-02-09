/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

/**
 * \file main.c
 * \brief Entry point of the interpreter.
 */

#include <assert.h>
#include <stdio.h>
#include "natrix/parser/diag.h"
#include "natrix/parser/parser.h"
#include "natrix/util/panic.h"

/**
 * \brief Evaluates the given binary operation.
 * \param left left operand
 * \param op binary operation
 * \param right right operand
 * \return the result of the operation
 */
static int64_t eval_binop(int64_t left, BinaryOp op, int64_t right) {
    switch (op) {
        case BINOP_ADD:
            return left + right;
        case BINOP_SUB:
            return left - right;
        case BINOP_MUL:
            return left * right;
        case BINOP_DIV:
            if (right == 0) {
                PANIC("Division by zero");
            }
            return left / right;
        default:
            assert(0);
    }
}

/**
 * \brief Evaluates the given expression.
 * \param expr the expression to evaluate
 * \return the result of the expression
 */
static int64_t eval_expr(const Expr *expr) {
    switch (expr->kind) {
        case EXPR_INT_LITERAL: {
            int64_t value = 0;
            for (const char *ptr = expr->literal.start; ptr < expr->literal.end; ptr++) {
                value = value * 10 + (*ptr - '0');
                if (value < 0) {
                    PANIC("Integer literal too large");
                }
            }
            return value;
        }
        case EXPR_BINARY:
            return eval_binop(eval_expr(expr->binary.left), expr->binary.op, eval_expr(expr->binary.right));
        default:
            assert(0);
    }
}

/**
 * \brief Executes the given statement.
 * \param stmt the statement
 */
static void exec_stmt(const Stmt *stmt) {
    switch (stmt->kind) {
        case STMT_EXPR:
            printf("%ld\n", eval_expr(stmt->expr));
            break;
        default:
            assert(0);
    }
}

/**
 * \brief Parses and executes the given source code.
 * \param source the source code
 */
static void run(Source *source) {
    Arena arena = arena_init();
    Stmt *stmt = parse_file(&arena, source, diag_default_handler, NULL);
    while (stmt) {
        exec_stmt(stmt);
        stmt = stmt->next;
    }
    arena_free(&arena);
}

/**
 * \brief Entry point of the interpreter.
 * \return 0 if successful, 1 otherwise
 */
int main(const int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }
    Source source = source_from_file(argv[1]);
    if (!source.start) {
        fprintf(stderr, "Unable to read file %s\n", argv[1]);
        return 1;
    }
    run(&source);
    source_free(&source);
}
