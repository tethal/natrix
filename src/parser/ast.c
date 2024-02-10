/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

/**
 * \file ast.c
 * \brief Abstract syntax tree implementation.
 */

#include <assert.h>
#include "natrix/parser/ast.h"

/**
 * \brief Names of binary operators.
 */
static const char *BINOP_NAMES[] = {
        [BINOP_ADD] = "ADD",
        [BINOP_SUB] = "SUB",
        [BINOP_MUL] = "MUL",
        [BINOP_DIV] = "DIV",
};

Expr *ast_create_expr_int_literal(Arena *arena, const char *start, const char *end) {
    assert(start < end);
    Expr *expr = arena_alloc(arena, sizeof(Expr));
    expr->kind = EXPR_INT_LITERAL;
    expr->literal.start = start;
    expr->literal.end = end;
    return expr;
}

Expr *ast_create_expr_name(Arena *arena, const char *start, const char *end) {
    assert(start < end);
    Expr *expr = arena_alloc(arena, sizeof(Expr));
    expr->kind = EXPR_NAME;
    expr->identifier.start = start;
    expr->identifier.end = end;
    return expr;
}

Expr *ast_create_expr_binary(Arena *arena, Expr *left, const BinaryOp op, Expr *right) {
    assert(left != NULL && right != NULL);
    Expr *expr = arena_alloc(arena, sizeof(Expr));
    expr->kind = EXPR_BINARY;
    expr->binary.left = left;
    expr->binary.op = op;
    expr->binary.right = right;
    return expr;
}

Stmt *ast_create_stmt_assignment(Arena *arena, Expr *left, Expr *right) {
    assert(left != NULL && right != NULL);
    assert(left->kind == EXPR_NAME);
    Stmt *stmt = arena_alloc(arena, sizeof(Stmt));
    stmt->kind = STMT_ASSIGNMENT;
    stmt->next = NULL;
    stmt->assignment.left = left;
    stmt->assignment.right = right;
    return stmt;
}

Stmt *ast_create_stmt_expr(Arena *arena, Expr *expr) {
    assert(expr != NULL);
    Stmt *stmt = arena_alloc(arena, sizeof(Stmt));
    stmt->kind = STMT_EXPR;
    stmt->next = NULL;
    stmt->expr = expr;
    return stmt;
}

const char *ast_get_expr_start(const Expr *expr) {
    switch (expr->kind) {
        case EXPR_INT_LITERAL:
            return expr->literal.start;
        case EXPR_NAME:
            return expr->identifier.start;
        case EXPR_BINARY:
            return ast_get_expr_start(expr->binary.left);
        default:
            assert(0);
    }
}

const char *ast_get_expr_end(const Expr *expr) {
    switch (expr->kind) {
        case EXPR_INT_LITERAL:
            return expr->literal.end;
        case EXPR_NAME:
            return expr->identifier.end;
        case EXPR_BINARY:
            return ast_get_expr_end(expr->binary.right);
        default:
            assert(0);
    }
}

/**
 * \brief Dumps the given expression to the given string builder.
 * \param sb the string builder
 * \param expr the expression to dump
 * \param indent the indentation level
 * \param label the label to print before the expression
 */
static void ast_dump_expr(StringBuilder *sb, const Expr *expr, int indent, const char *label) {
    sb_append_formatted(sb, "%*s", indent, "");
    if (label) {
        sb_append_formatted(sb, "%s: ", label);
    }
    switch (expr->kind) {
        case EXPR_INT_LITERAL:
            sb_append_formatted(sb, "EXPR_INT_LITERAL {literal: \"%.*s\"}\n", (int) (expr->literal.end - expr->literal.start), expr->literal.start);
            break;
        case EXPR_NAME:
            sb_append_formatted(sb, "EXPR_NAME {identifier: \"%.*s\"}\n", (int) (expr->identifier.end - expr->identifier.start), expr->identifier.start);
            break;
        case EXPR_BINARY:
            assert(expr->binary.op >= 0 && expr->binary.op < BINOP_COUNT);
            sb_append_formatted(sb, "EXPR_BINARY {op: %s}\n", BINOP_NAMES[expr->binary.op]);
            ast_dump_expr(sb, expr->binary.left, indent + 2, "left");
            ast_dump_expr(sb, expr->binary.right, indent + 2, "right");
            break;
        default:
            assert(0 && "Invalid ExprKind");
    }
}

/**
 * \brief Dumps the given statement to the given string builder.
 * \param sb the string builder
 * \param stmt the statement to dump
 * \param indent the indentation level
 */
static void ast_dump_stmt(StringBuilder *sb, const Stmt *stmt, int indent) {
    sb_append_formatted(sb, "%*s", indent, "");
    switch (stmt->kind) {
        case STMT_EXPR:
            sb_append_str(sb, "STMT_EXPR\n");
            ast_dump_expr(sb, stmt->expr, indent + 2, NULL);
            break;
        case STMT_ASSIGNMENT:
            sb_append_str(sb, "STMT_ASSIGNMENT\n");
            ast_dump_expr(sb, stmt->assignment.left, indent + 2, "left");
            ast_dump_expr(sb, stmt->assignment.right, indent + 2, "right");
            break;
        default:
            assert(0 && "Invalid StmtKind");
    }
}

void ast_dump(StringBuilder *sb, const Stmt *stmt) {
    sb_append_str(sb, "AST dump:\n");
    while (stmt) {
        ast_dump_stmt(sb, stmt, 2);
        stmt = stmt->next;
    }
}
