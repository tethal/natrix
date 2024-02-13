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
#include <string.h>
#include "natrix/parser/diag.h"
#include "natrix/parser/parser.h"
#include "natrix/util/panic.h"

/**
 * \brief Represents a variable in the environment.
 */
typedef struct Variable Variable;
struct Variable {
    const char *name_start;             //!< start of the name
    size_t name_len;                    //!< length of the name
    int64_t value;                      //!< value of the variable
    Variable *next;                     //!< next variable in the list
};

/**
 * \brief Represents the environment, i.e. the mapping of variable names to their values.
 *
 * Currently, the environment is implemented as a linked list of variables to keep things simple.
 * It will be replaced with a more efficient data structure in the future.
 */
typedef struct Env {
    Arena arena;                        //!< arena for memory allocation
    Variable *head;                     //!< head of the list of variables
} Env;

/**
 * \brief Finds a variable in the environment.
 * \param env the environment
 * \param name_start start of the name
 * \param name_len length of the name
 * \return the variable if found, otherwise NULL
 */
static Variable *env_find(Env *env, const char *name_start, size_t name_len) {
    for (Variable *var = env->head; var; var = var->next) {
        if (var->name_len == name_len && memcmp(var->name_start, name_start, name_len) == 0) {
            return var;
        }
    }
    return NULL;
}

/**
 * \brief Sets the value of the given variable in the environment.
 * \param env the environment
 * \param name_start start of the name
 * \param name_len length of the name
 * \param value the value to set
 */
static void env_set(Env *env, const char *name_start, size_t name_len, int64_t value) {
    Variable *var = env_find(env, name_start, name_len);
    if (!var) {
        var = arena_alloc(&env->arena, sizeof(Variable));
        var->name_start = name_start;
        var->name_len = name_len;
        var->next = env->head;
        env->head = var;
    }
    var->value = value;
}

/**
 * \brief Gets the value of the given variable in the environment.
 *
 * If the variable is not found, the program panics.
 * \param env the environment
 * \param name_start start of the name
 * \param name_len length of the name
 * \return the value of the variable
 */
static int64_t env_get(Env *env, const char *name_start, size_t name_len) {
    Variable *var = env_find(env, name_start, name_len);
    if (!var) {
        PANIC("Undefined variable: %.*s", (int) name_len, name_start);
    }
    return var->value;
}

/**
 * \brief Converts the given string to an integer.
 * \param str the string, must contain only digits
 * \param len the length of the string
 * \return the integer value
 */
static int64_t int_from_str(const char *str, size_t len) {
    int64_t value = 0;
    for (size_t i = 0; i < len; i++) {
        assert(str[i] >= '0' && str[i] <= '9');
        value = value * 10 + (str[i] - '0');
        if (value < 0) {
            PANIC("Integer literal too large");
        }
    }
    return value;
}

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
        case BINOP_EQ:
            return left == right;
        case BINOP_NE:
            return left != right;
        case BINOP_LT:
            return left < right;
        case BINOP_LE:
            return left <= right;
        case BINOP_GT:
            return left > right;
        case BINOP_GE:
            return left >= right;
        default:
            assert(0);
    }
}

/**
 * \brief Evaluates the given expression.
 * \param env the environment for variable lookup
 * \param expr the expression to evaluate
 * \return the result of the expression
 */
static int64_t eval_expr(Env *env, const Expr *expr) {
    switch (expr->kind) {
        case EXPR_INT_LITERAL:
            return int_from_str(expr->literal.start, expr->literal.end - expr->literal.start);
        case EXPR_NAME:
            return env_get(env, expr->identifier.start, expr->identifier.end - expr->identifier.start);
        case EXPR_BINARY:
            return eval_binop(eval_expr(env, expr->binary.left), expr->binary.op, eval_expr(env, expr->binary.right));
        default:
            assert(0);
    }
}

static void exec_stmts(Env *env, const Stmt *stmt);

/**
 * \brief Executes the given statement.
 * \param env the environment for variable lookup
 * \param stmt the statement
 */
static void exec_stmt(Env *env, const Stmt *stmt) {
    switch (stmt->kind) {
        case STMT_EXPR:
            eval_expr(env, stmt->expr);
            break;
        case STMT_ASSIGNMENT: {
            assert(stmt->assignment.left->kind == EXPR_NAME);
            int64_t rhs = eval_expr(env, stmt->assignment.right);
            const char *start = stmt->assignment.left->identifier.start;
            const char *end = stmt->assignment.left->identifier.end;
            env_set(env, start, end - start, rhs);
            break;
        }
        case STMT_WHILE:
            while (eval_expr(env, stmt->while_stmt.condition)) {
                exec_stmts(env, stmt->while_stmt.body);
            }
            break;
        case STMT_IF:
            if (eval_expr(env, stmt->if_stmt.condition)) {
                exec_stmts(env, stmt->if_stmt.then_body);
            } else if (stmt->if_stmt.else_body) {
                exec_stmts(env, stmt->if_stmt.else_body);
            }
            break;
        case STMT_PASS:
            break;
        case STMT_PRINT:
            printf("%ld\n", eval_expr(env, stmt->expr));
            break;
        default:
            assert(0);
    }
}

/**
 * \brief Executes the given list of statements.
 * \param env the environment for variable lookup
 * \param stmt the first statement in the list
 */
static void exec_stmts(Env *env, const Stmt *stmt) {
    while (stmt) {
        exec_stmt(env, stmt);
        stmt = stmt->next;
    }
}

/**
 * \brief Parses and executes the given source code.
 * \param source the source code
 * \param arg the argument to the program
 */
static void run(Source *source, int64_t arg) {
    Env env = {
        .arena = arena_init(),
        .head = NULL,
    };
    Stmt *stmt = parse_file(&env.arena, source, diag_default_handler, NULL);
    env_set(&env, "arg", 3, arg);
    exec_stmts(&env, stmt);
    arena_free(&env.arena);
}

/**
 * \brief Entry point of the interpreter.
 * \return 0 if successful, 1 otherwise
 */
int main(const int argc, char **argv) {
    if (argc != 2 && argc != 3) {
        fprintf(stderr, "Usage: %s <filename> [arg]\n", argv[0]);
        return 1;
    }
    int64_t arg = 0;
    if (argc == 3) {
        const char *ptr = argv[2];
        while (*ptr) {
            if (*ptr < '0' || *ptr > '9') {
                fprintf(stderr, "Invalid argument: %s\n", argv[2]);
                return 1;
            }
            ptr++;
        }
        arg = int_from_str(argv[2], strlen(argv[2]));
    }
    Source source = source_from_file(argv[1]);
    if (!source.start) {
        fprintf(stderr, "Unable to read file %s\n", argv[1]);
        return 1;
    }
    run(&source, arg);
    source_free(&source);
}
