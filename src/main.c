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
#include "natrix/obj/nx_int.h"
#include "natrix/obj/nx_list.h"
#include "natrix/obj/nx_str.h"
#include "natrix/parser/diag.h"
#include "natrix/parser/parser.h"
#include "natrix/util/panic.h"

/**
 * \brief Represents a variable in the environment.
 */
typedef struct Variable Variable;
struct Variable {
    GcHeader gc_header;                 //!< header for garbage collector
    const char *name_start;             //!< start of the name
    size_t name_len;                    //!< length of the name
    NxObject *value;                    //!< value of the variable
    Variable *next;                     //!< next variable in the list
};

/**
 * \brief Represents the environment, i.e. the mapping of variable names to their values.
 *
 * Currently, the environment is implemented as a linked list of variables to keep things simple.
 * It will be replaced with a more efficient data structure in the future.
 */
typedef struct Env {
    GcHeader gc_header;                 //!< header for garbage collector
    Variable *head;                     //!< head of the list of variables
} Env;

static void variable_gc_trace(void *ptr) {
    Variable *var = (Variable *) ptr;
    gc_visit(&var->value->gc_header);
    gc_visit(&var->next->gc_header);
}

static void env_gc_trace(void *ptr) {
    Env *env = (Env *) ptr;
    gc_visit(&env->head->gc_header);
}

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
static void env_set(Env *env, const char *name_start, size_t name_len, NxObject *value) {
    Variable *var = env_find(env, name_start, name_len);
    if (!var) {
        nxo_root(value);
        var = (Variable *) gc_alloc(sizeof(Variable), variable_gc_trace);
        nxo_unroot(value);
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
static NxObject *env_get(Env *env, const char *name_start, size_t name_len) {
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
static NxObject *int_from_str(const char *str, size_t len) {
    int64_t value = 0;
    for (size_t i = 0; i < len; i++) {
        assert(str[i] >= '0' && str[i] <= '9');
        value = value * 10 + (str[i] - '0');
        if (value < 0) {
            PANIC("Integer literal too large");
        }
    }
    return nx_int_create(value);
}

/**
 * \brief Evaluates the given binary operation on integers.
 * \param left left operand
 * \param op binary operation
 * \param right right operand
 * \return the result of the operation
 */
static int64_t eval_binop_int(int64_t left, BinaryOp op, int64_t right) {
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
 * \brief Evaluates the given binary operation.
 * \param env the environment for allocation
 * \param left left operand
 * \param op binary operation
 * \param right right operand
 * \return the result of the operation
 */
static NxObject *eval_binop(Env *env, NxObject *left, BinaryOp op, NxObject *right) {
    if (nx_int_is_instance(left) && nx_int_is_instance(right)) {
        int64_t result = eval_binop_int(nx_int_get_value(left), op, nx_int_get_value(right));
        return nx_int_create(result);
    }
    if (op == BINOP_ADD && nx_str_is_instance(left) && nx_str_is_instance(right)) {
        nxo_root(right);
        NxObject *result = nx_str_concat(left, right);
        nxo_unroot(right);
        return result;
    }
    PANIC("Operands must be integers");
}

/**
 * \brief Evaluates the given expression.
 * \param env the environment for variable lookup
 * \param expr the expression to evaluate
 * \return the result of the expression
 */
static NxObject *eval_expr(Env *env, const Expr *expr) {
    switch (expr->kind) {
        case EXPR_INT_LITERAL:
            return int_from_str(expr->literal.start, expr->literal.end - expr->literal.start);
        case EXPR_STR_LITERAL: {
            assert(expr->literal.start[0] == '"' && expr->literal.end[-1] == '"');
            int64_t len = expr->literal.end - expr->literal.start - 2;
            return nx_str_create(expr->literal.start + 1, len);
        }
        case EXPR_LIST_LITERAL: {
            int64_t cnt = 0;
            Expr *e = expr->literal.head;
            while (e) {
                cnt++;
                e = e->next;
            }
            NxObject *result = nx_list_create(cnt);
            nxo_root(result);
            e = expr->literal.head;
            for (size_t i = 0; i < cnt; i++) {
                NxObject *value = eval_expr(env, e);
                nxo_root(value);
                nx_list_append(result, value);
                nxo_unroot(value);
                e = e->next;
            }
            nxo_unroot(result);
            return result;
        }
        case EXPR_NAME:
            return env_get(env, expr->identifier.start, expr->identifier.end - expr->identifier.start);
        case EXPR_BINARY: {
            NxObject *left = eval_expr(env, expr->binary.left);
            nxo_root(left);
            NxObject *right = eval_expr(env, expr->binary.right);
            NxObject *res = eval_binop(env, left, expr->binary.op, right);
            nxo_unroot(left);
            return res;
        }
        case EXPR_SUBSCRIPT: {
            NxObject *receiver = eval_expr(env, expr->subscript.receiver);
            if (!nx_list_is_instance(receiver)) {
                PANIC("Subscripted value must be a list");
            }
            nxo_root(receiver);
            NxObject *index = eval_expr(env, expr->subscript.index);
            if (!nx_int_is_instance(index)) {
                PANIC("Index must be an integer");
            }
            int64_t i = nx_int_get_value(index);
            if (i < 0 || (size_t) i >= nx_list_get_length(receiver)) {
                PANIC("Index out of range");
            }
            NxObject *res = ((NxList *) receiver)->items->data[i];
            nxo_unroot(receiver);
            return res;
        }
        default:
            assert(0);
    }
}

static void exec_stmts(Env *env, const Stmt *stmt);

/**
 * \brief Evaluates an expression, checks that it is an integer, and returns the result as a boolean.
 * \param env the environment for variable lookup
 * \param expr the condition expression
 * \return the result of the condition
 */
static bool eval_cond(Env *env, const Expr *expr) {
    NxObject *value = eval_expr(env, expr);
    if (!nx_int_is_instance(value)) {
        PANIC("Condition must be an integer");
    }
    return nx_int_get_value(value) != 0;
}

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
            if (stmt->assignment.left->kind == EXPR_NAME) {
                NxObject *rhs = eval_expr(env, stmt->assignment.right);
                const char *start = stmt->assignment.left->identifier.start;
                const char *end = stmt->assignment.left->identifier.end;
                env_set(env, start, end - start, rhs);
            } else if (stmt->assignment.left->kind == EXPR_SUBSCRIPT) {
                NxObject *receiver = eval_expr(env, stmt->assignment.left->subscript.receiver);
                if (!nx_list_is_instance(receiver)) {
                    PANIC("Subscripted value must be a list");
                }
                nxo_root(receiver);
                NxObject *index = eval_expr(env, stmt->assignment.left->subscript.index);
                if (!nx_int_is_instance(index)) {
                    PANIC("Index must be an integer");
                }
                int64_t i = nx_int_get_value(index);
                if (i < 0 || (size_t) i >= nx_list_get_length(receiver)) {
                    PANIC("Index out of range");
                }
                ((NxList *) receiver)->items->data[i] = eval_expr(env, stmt->assignment.right);
                nxo_unroot(receiver);
            } else {
                assert(0);
            }
            break;
        }
        case STMT_WHILE:
            while (eval_cond(env, stmt->while_stmt.condition)) {
                exec_stmts(env, stmt->while_stmt.body);
            }
            break;
        case STMT_IF:
            if (eval_cond(env, stmt->if_stmt.condition)) {
                exec_stmts(env, stmt->if_stmt.then_body);
            } else if (stmt->if_stmt.else_body) {
                exec_stmts(env, stmt->if_stmt.else_body);
            }
            break;
        case STMT_PASS:
            break;
        case STMT_PRINT: {
            NxObject *value = eval_expr(env, stmt->expr);
            if (nx_int_is_instance(value)) {
                printf("%ld\n", nx_int_get_value(value));
            } else if (nx_str_is_instance(value)) {
                printf("%s\n", nx_str_get_cstr(value));
            } else {
                PANIC("Unexpected value type in print()");
            }
            break;
        }
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
static void run(Source *source, NxObject *arg) {
    Env env = {
            .gc_header = {.next = NULL, .trace_fn = env_gc_trace},
            .head = NULL,
    };
    gc_root(&env.gc_header);
    Arena arena = arena_init();
    Stmt *stmt = parse_file(&arena, source, diag_default_handler, NULL);
    env_set(&env, "arg", 3, arg);
    exec_stmts(&env, stmt);
    arena_free(&arena);
    gc_unroot(&env.gc_header);
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
    NxObject *arg;
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
    } else {
        arg = nx_int_create(0);
    }
    gc_root(&arg->gc_header);
    Source source = source_from_file(argv[1]);
    if (!source.start) {
        fprintf(stderr, "Unable to read file %s\n", argv[1]);
        return 1;
    }
    run(&source, arg);
    gc_unroot(&arg->gc_header);
    gc_collect();
    source_free(&source);
}
