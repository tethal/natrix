/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

/**
 * \file ast.h
 * \brief Definition of the abstract syntax tree, its nodes and operations.
 *
 * The abstract syntax tree (AST) is a tree representation of the source code which is easy to manipulate and analyze.
 * All nodes and related structures in the tree are allocated from an arena allocator, which allows deallocation of
 * the whole tree with a single call to the arena destructor, therefore the arena must outlive the AST.
 */

#ifndef AST_H
#define AST_H
#ifdef __cplusplus
extern "C" {
#endif

#include "natrix/util/arena.h"
#include "natrix/util/sb.h"

/**
 * \brief Determines the concrete kind of an expression node.
 */
typedef enum {
    EXPR_INT_LITERAL,       //!< Integer literal
    EXPR_NAME,              //!< Identifier
    EXPR_BINARY,            //!< Binary operation
} ExprKind;

/**
 * \brief Determines the concrete kind of a statement node.
 */
typedef enum {
    STMT_EXPR,              //!< Expression statement, e.g. function call
    STMT_ASSIGNMENT,        //!< Assignment statement
    STMT_WHILE,             //!< While loop
    STMT_IF,                //!< If statement
    STMT_PASS,              //!< Empty statement
    STMT_PRINT,             //!< Print statement, to be removed once function calls and built-ins are implemented
} StmtKind;

/**
 * \brief Binary operators.
 */
typedef enum {
    BINOP_ADD,              //!< Addition
    BINOP_SUB,              //!< Subtraction
    BINOP_MUL,              //!< Multiplication
    BINOP_DIV,              //!< Division
    BINOP_COUNT             //!< Number of binary operators
} BinaryOp;

typedef struct Expr Expr;
typedef struct Stmt Stmt;

/**
 * \brief Attributes of the `EXPR_INT_LITERAL` AST node.
 */
typedef struct {
    const char *start;              //!< Pointer to the start of the integer literal in the source code
    const char *end;                //!< Pointer to the character after the end of the integer literal
} ExprLiteral;

/**
 * \brief Attributes of the `EXPR_Name` AST node.
 */
typedef struct {
    const char *start;              //!< Pointer to the start of the identifier in the source code
    const char *end;                //!< Pointer to the character after the end of the identifier
} ExprName;

/**
 * \brief Attributes of the `EXPR_BINARY` AST node.
 */
typedef struct {
    Expr *left;                     //!< Left operand
    BinaryOp op;                    //!< Binary operator
    Expr *right;                    //!< Right operand
} ExprBinary;

/**
 * \brief AST node representing an expression.
 */
struct Expr {
    ExprKind kind;                  //!< Kind of the expression, determines which field of the union is active
    /**
     * \brief Union of all possible kinds of expression nodes.
     */
    union {
        ExprLiteral literal;        //!< Value of a literal, active when `kind` is `EXPR_INT_LITERAL`
        ExprName identifier;        //!< Identifier, active when `kind` is `EXPR_NAME`
        ExprBinary binary;          //!< Binary operation, active when `kind` is `EXPR_BINARY`
    };
};

/**
 * \brief AST node representing an assignment statement.
 */
typedef struct {
    Expr *left;                     //!< Left-hand side of the assignment
    Expr *right;                    //!< Right-hand side of the assignment
} StmtAssignment;

/**
 * \brief AST node representing a `while` statement.
 */
typedef struct {
    Expr *condition;                //!< Condition of the loop
    Stmt *body;                     //!< Body of the loop
} StmtWhile;

/**
 * \brief AST node representing an `if` statement.
 */
typedef struct {
    Expr *condition;                //!< Condition of the `if` statement
    Stmt *then_body;                //!< True branch of the `if` statement
    Stmt *else_body;                //!< False branch of the `if` statement
} StmtIf;

/**
 * \brief AST node representing a statement.
 */
struct Stmt {
    StmtKind kind;                  //!< Kind of the statement, determines which field of the union is active
    Stmt *next;                     //!< Pointer to the next statement in the sequence
    /**
     * \brief Union of all possible kinds of statement nodes.
     */
    union {
        Expr *expr;                 //!< Expression statement, active when `kind` is `STMT_EXPR` or `STMT_PRINT`
        StmtAssignment assignment;  //!< Assignment statement, active when `kind` is `STMT_ASSIGNMENT`
        StmtWhile while_stmt;       //!< While loop, active when `kind` is `STMT_WHILE`
        StmtIf if_stmt;             //!< If statement, active when `kind` is `STMT_IF`
    };
};

/**
 * \brief Creates a new node representing an integer literal.
 * \param arena arena allocator from which the node will be allocated
 * \param start pointer to the start of the integer literal in the source code
 * \param end pointer to the character after the end of the integer literal
 * \return the newly allocated node
 */
Expr *ast_create_expr_int_literal(Arena *arena, const char *start, const char *end);

/**
 * \brief Creates a new node representing a name.
 * \param arena arena allocator from which the node will be allocated
 * \param start pointer to the start of the identifier in the source code
 * \param end pointer to the character after the end of the identifier
 * \return the newly allocated node
 */
Expr *ast_create_expr_name(Arena *arena, const char *start, const char *end);

/**
 * \brief Creates a new node representing a binary operation.
 * \param arena arena allocator from which the node will be allocated
 * \param left left operand
 * \param op binary operator
 * \param right right operand
 * \return the newly allocated node
 */
Expr *ast_create_expr_binary(Arena *arena, Expr *left, BinaryOp op, Expr *right);

/**
 * \brief Creates a new node representing an expression statement.
 * \param arena arena allocator from which the node will be allocated
 * \param expr expression to be used as the statement
 * \return the newly allocated node
 */
Stmt *ast_create_stmt_expr(Arena *arena, Expr *expr);

/**
 * \brief Creates a new node representing an assignment statement.
 * \param arena arena allocator from which the node will be allocated
 * \param left the left-hand side of the assignment
 * \param right the right-hand side of the assignment
 * \return the newly allocated node
 */
Stmt *ast_create_stmt_assignment(Arena *arena, Expr *left, Expr *right);

/**
 * \brief Creates a new node representing a `while` statement.
 * \param arena arena allocator from which the node will be allocated
 * \param condition the condition of the loop
 * \param body the body of the loop
 * \return the newly allocated node
 */
Stmt *ast_create_stmt_while(Arena *arena, Expr *condition, Stmt *body);

/**
 * \brief Creates a new node representing an `if` statement.
 * \param arena arena allocator from which the node will be allocated
 * \param condition the condition of the `if` statement
 * \param then_body the true branch of the `if` statement
 * \param else_body the false branch of the `if` statement, use `STMT_PASS` if there is no `else` branch
 * \return the newly allocated node
 */
Stmt *ast_create_stmt_if(Arena *arena, Expr *condition, Stmt *then_body, Stmt *else_body);

/**
 * \brief Creates a new node representing an empty statement.
 * \param arena arena allocator from which the node will be allocated
 * \return the newly allocated node
 */
Stmt *ast_create_stmt_pass(Arena *arena);

/**
 * \brief Creates a new node representing a `print` statement.
 * \param arena arena allocator from which the node will be allocated
 * \param expr expression to be printed
 * \return the newly allocated node
 */
Stmt *ast_create_stmt_print(Arena *arena, Expr *expr);

/**
 * \brief Returns the start position of the given expression node in the source code.
 * \param expr the expression node
 * \return pointer to the start of the expression in the source code
 */
const char *ast_get_expr_start(const Expr *expr);

/**
 * \brief Returns the end position of the given expression node in the source code.
 * \param expr the expression node
 * \return pointer to the character after the end of the expression in the source code
 */
const char *ast_get_expr_end(const Expr *expr);

/**
 * \brief Dumps the AST tree to a string builder.
 * \param sb string builder to which the AST will be dumped
 * \param stmt root of the AST tree
 */
void ast_dump(StringBuilder *sb, const Stmt *stmt);

#ifdef __cplusplus
}
#endif
#endif //AST_H
