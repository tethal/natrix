/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

/**
 * \file parser.c
 * \brief Parser implementation.
 */

#include "natrix/parser/parser.h"
#include <assert.h>
#include "natrix/parser/lexer.h"
#ifdef ENABLE_AST_LOGGING
#include "natrix/util/log.h"
#endif

/**
 * \brief Internal state of the parser.
 */
typedef struct Parser {
    Arena *arena;                       //!< memory arena used to allocate the abstract syntax tree nodes
    Source *source;                     //!< source code being parsed
    DiagHandler diag_handler;           //!< diagnostics handler
    void *diag_data;                    //!< private data of the diagnostics handler
    Lexer lexer;                        //!< lexer used to tokenize the source code
    Token current;                      //!< current token
} Parser;

/**
 * \brief Reports an error message at the position of the current token.
 *
 * If the current token is an error token, the error message from the lexer is reported instead.
 * \param parser the parser state
 * \param message the error message
 */
static void error(Parser *parser, const char *message) {
    if (parser->current.type == TOKEN_ERROR) {
        message = lexer_error_message(&parser->lexer);
    }
    parser->diag_handler(parser->diag_data, DIAG_ERROR, parser->source, parser->current.start, parser->current.end, "%s", message);
}

/**
 * \brief Consumes the current token and advances to the next one.
 * \param parser the parser state
 * \return the consumed token
 */
static Token consume(Parser *parser) {
    assert(parser->current.type != TOKEN_ERROR && parser->current.type != TOKEN_EOF);
    Token result = parser->current;
    parser->current = lexer_next_token(&parser->lexer);
    return result;
}

/**
 * \brief Consumes the current token if it matches the given type, otherwise reports an error.
 * \param parser the parser state
 * \param type the expected type of the current token
 * \param message the error message to report if the current token does not match the expected type
 * \return true if the current token matches the expected type, false otherwise
 */
static bool match(Parser *parser, TokenType type, const char *message) {
    if (parser->current.type != type) {
        error(parser, message);
        return false;
    }
    consume(parser);
    return true;
}

static Expr *expression(Parser *parser);

/**
 * \code
 * primary:
 *    INT_LITERAL
 *    | LPAREN expression RPAREN
 * \endcode
 */
static Expr *primary(Parser *parser) {
    if (parser->current.type == TOKEN_INT_LITERAL) {
        Token t = consume(parser);
        return ast_create_expr_int_literal(parser->arena, t.start, t.end);
    }
    if (parser->current.type == TOKEN_LPAREN) {
        consume(parser);
        Expr *expr = expression(parser);
        if (expr && match(parser, TOKEN_RPAREN, "expected closing parenthesis")) {
            return expr;
        }
        return NULL;
    }
    error(parser, "expected expression");
    return NULL;
}

/**
 * \code
 * multiplicative_expr:
 *    multiplicative_expr (STAR | SLASH) primary
 *    | primary
 * \endcode
 */
static Expr *multiplicative_expr(Parser *parser) {
    Expr *result = primary(parser);
    while (result && (parser->current.type == TOKEN_STAR || parser->current.type == TOKEN_SLASH)) {
        BinaryOp op = parser->current.type == TOKEN_STAR ? BINOP_MUL : BINOP_DIV;
        consume(parser);
        Expr *right = primary(parser);
        result = right ? ast_create_expr_binary(parser->arena, result, op, right) : NULL;
    }
    return result;
}

/**
 * \code
 * additive_expr:
 *    additive_expr (PLUS | MINUS) multiplicative_expr
 *    | multiplicative_expr
 * \endcode
 */
static Expr *additive_expr(Parser *parser) {
    Expr *result = multiplicative_expr(parser);
    while (result && (parser->current.type == TOKEN_PLUS || parser->current.type == TOKEN_MINUS)) {
        BinaryOp op = parser->current.type == TOKEN_PLUS ? BINOP_ADD : BINOP_SUB;
        consume(parser);
        Expr *right = multiplicative_expr(parser);
        result = right ? ast_create_expr_binary(parser->arena, result, op, right) : NULL;
    }
    return result;
}

/**
 * \code
 * expression: additive_expr
 * \endcode
 */
static Expr *expression(Parser *parser) {
    return additive_expr(parser);
}

/**
 * \code
 * statement: expression NEWLINE
 * \endcode
 */
static Stmt *statement(Parser *parser) {
    Expr *expr = expression(parser);
    if (expr && match(parser, TOKEN_NEWLINE, "expected end of line")) {
        return ast_create_stmt_expr(parser->arena, expr);
    }
    return NULL;
}

/**
 * \code
 * statements:
 *    statement
 *    | statement statements
 * \endcode
 */
static Stmt *statements(Parser *parser, TokenType sentinel) {
    Stmt *result = statement(parser);
    Stmt *last = result;
    while (last && parser->current.type != sentinel) {
        last->next = statement(parser);
        last = last->next;
    }
    return result;
}

Stmt *parse_file(Arena *arena, Source *source, DiagHandler diag_handler, void *diag_data) {
    Parser parser;
    parser.arena = arena;
    parser.source = source;
    parser.diag_handler = diag_handler;
    parser.diag_data = diag_data;
    lexer_init(&parser.lexer, source->start);
    parser.current = lexer_next_token(&parser.lexer);

    Stmt *result = statements(&parser, TOKEN_EOF);
    assert(result == NULL || parser.current.type == TOKEN_EOF);

#if ENABLE_AST_LOGGING
    StringBuilder sb = sb_init();
    ast_dump(&sb, result);
    const char *ptr = sb.str;
    const char *line = ptr;
    while (*ptr) {
        if (*ptr == '\n') {
            LOG_INFO("%.*s", (int) (ptr - line), line);
            line = ptr + 1;
        }
        ptr++;
    }
    sb_free(&sb);
#endif

    return result;
}
