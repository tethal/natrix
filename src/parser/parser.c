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
static Stmt *block(Parser *parser);

/**
 * \code
 * expression_list: expression (COMMA expression)* COMMA?
 * \endcode
 */
static Expr *expression_list(Parser *parser, TokenType sentinel) {
    Expr *result = NULL;
    Expr *last = NULL;
    while (1) {
        Expr *expr = expression(parser);
        if (!expr) {
            return NULL;
        }
        if (last) {
            last->next = expr;
        } else {
            result = expr;
        }
        last = expr;
        if (parser->current.type == TOKEN_COMMA) {
            consume(parser);
        }
        if (parser->current.type == sentinel) {
            return result;
        }
    }
}

/**
 * \code
 * primary:
 *    INT_LITERAL
 *    | STRING_LITERAL
 *    | IDENTIFIER
 *    | LPAREN expression RPAREN
 *    | LBRACKET expression_list? RBRACKET
 * \endcode
 */
static Expr *primary(Parser *parser) {
    if (parser->current.type == TOKEN_INT_LITERAL) {
        Token t = consume(parser);
        return ast_create_expr_int_literal(parser->arena, t.start, t.end);
    }
    if (parser->current.type == TOKEN_STRING_LITERAL) {
        Token t = consume(parser);
        return ast_create_expr_str_literal(parser->arena, t.start, t.end);
    }
    if (parser->current.type == TOKEN_IDENTIFIER) {
        Token t = consume(parser);
        return ast_create_expr_name(parser->arena, t.start, t.end);
    }
    if (parser->current.type == TOKEN_LPAREN) {
        consume(parser);
        Expr *expr = expression(parser);
        if (expr && match(parser, TOKEN_RPAREN, "expected closing parenthesis")) {
            return expr;
        }
        return NULL;
    }
    if (parser->current.type == TOKEN_LBRACKET) {
        const char *start = consume(parser).start;
        const char *end;
        Expr *expr = NULL;
        if (parser->current.type == TOKEN_RBRACKET) {
            end = consume(parser).end;
        } else {
            expr = expression_list(parser, TOKEN_RBRACKET);
            end = parser->current.end;
            if (!expr || !match(parser, TOKEN_RBRACKET, "expected closing bracket")) {
                return NULL;
            }
        }
        return ast_create_expr_list_literal(parser->arena, start, end, expr);
    }
    error(parser, "expected expression");
    return NULL;
}

/**
 * \code
 * postfix_expr: primary (LBRACKET expression RBRACKET)*
 * \endcode
 */
static Expr *postfix_expr(Parser *parser) {
    Expr *expr = primary(parser);
    while (parser->current.type == TOKEN_LBRACKET) {
        consume(parser);
        Expr *index = expression(parser);
        const char *end = parser->current.end;
        if (!index || !match(parser, TOKEN_RBRACKET, "expected closing bracket")) {
            return NULL;
        }
        expr = ast_create_expr_subscript(parser->arena, expr, index, end);
    }
    return expr;
}

/**
 * \code
 * multiplicative_expr:
 *    multiplicative_expr (STAR | SLASH) postfix_expr
 *    | postfix_expr
 * \endcode
 */
static Expr *multiplicative_expr(Parser *parser) {
    Expr *result = postfix_expr(parser);
    while (result && (parser->current.type == TOKEN_STAR || parser->current.type == TOKEN_SLASH)) {
        BinaryOp op = parser->current.type == TOKEN_STAR ? BINOP_MUL : BINOP_DIV;
        consume(parser);
        Expr *right = postfix_expr(parser);
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
 * relational_expr:
 *     relational_expr (EQ | NE | GT | GE | LT | LE) additive_expr
 *     | additive_expr
 * \endcode
 */
static Expr* relational_expr(Parser *parser) {
    Expr *result = additive_expr(parser);
    if (!result) {
        return NULL;
    }
    BinaryOp op;
    switch (parser->current.type) {
        case TOKEN_EQ:
            op = BINOP_EQ;
            break;
        case TOKEN_NE:
            op = BINOP_NE;
            break;
        case TOKEN_GT:
            op = BINOP_GT;
            break;
        case TOKEN_GE:
            op = BINOP_GE;
            break;
        case TOKEN_LT:
            op = BINOP_LT;
            break;
        case TOKEN_LE:
            op = BINOP_LE;
            break;
        default:
            return result;
    }
    consume(parser);
    Expr *right = additive_expr(parser);
    return right ? ast_create_expr_binary(parser->arena, result, op, right) : NULL;
}

/**
 * \code
 * expression: relational_expr
 * \endcode
 */
static Expr *expression(Parser *parser) {
    return relational_expr(parser);
}

/**
 * \code
 * simple_statement:
 *     KW_PRINT LPAREN expression RPAREN
 *     | KW_PASS
 *     | expression EQUALS expression
 *     | expression
 * \endcode
 */
static Stmt *simple_statement(Parser *parser) {
    if (parser->current.type == TOKEN_KW_PRINT) {
        consume(parser);
        if (match(parser, TOKEN_LPAREN, "expected '('")) {
            Expr *expr = expression(parser);
            if (expr && match(parser, TOKEN_RPAREN, "expected ')'")) {
                return ast_create_stmt_print(parser->arena, expr);
            }
        }
        return NULL;
    }
    if (parser->current.type == TOKEN_KW_PASS) {
        consume(parser);
        return ast_create_stmt_pass(parser->arena);
    }
    Expr *expr = expression(parser);
    if (!expr) {
        return NULL;
    }
    if (parser->current.type != TOKEN_EQUALS) {
        return ast_create_stmt_expr(parser->arena, expr);
    }
    if (expr->kind != EXPR_NAME && expr->kind != EXPR_SUBSCRIPT) {
        parser->diag_handler(parser->diag_data, DIAG_ERROR, parser->source, ast_get_expr_start(expr),
                             ast_get_expr_end(expr), "cannot assign to expression here");
        return NULL;
    }
    consume(parser);
    Expr *right = expression(parser);
    return right ? ast_create_stmt_assignment(parser->arena, expr, right) : NULL;
}

/**
 * \code
 * else_block: KW_ELSE COLON block
 * \endcode
 */
static Stmt *else_block(Parser *parser) {
    assert(parser->current.type == TOKEN_KW_ELSE);
    consume(parser);
    if (!match(parser, TOKEN_COLON, "expected ':'")) {
        return NULL;
    }
    return block(parser);
}

/**
 * \code
 * elif_block: KW_ELIF expression COLON block (elif_block | else_block)?
 * \endcode
 * \note This function is also used to parse the `if` statement, only the keyword is different.
 */
static Stmt *elif_block(Parser *parser) {
    assert(parser->current.type == TOKEN_KW_ELIF || parser->current.type == TOKEN_KW_IF);
    consume(parser);
    Expr *cond = expression(parser);
    if (!(cond && match(parser, TOKEN_COLON, "expected ':'"))) {
        return NULL;
    }
    Stmt *then_body = block(parser);
    if (!then_body) {
        return NULL;
    }
    Stmt *else_body;
    if (parser->current.type == TOKEN_KW_ELSE) {
        else_body = else_block(parser);
    } else if (parser->current.type == TOKEN_KW_ELIF) {
        else_body = elif_block(parser);
    } else {
        else_body = ast_create_stmt_pass(parser->arena);
    }
    return else_body ? ast_create_stmt_if(parser->arena, cond, then_body, else_body) : NULL;
}

/**
 * \code
 * statement:
 *     KW_WHILE expression COLON block
 *     | KW_IF expression COLON block (elif_block | else_block)?
 *     | simple_statement NEWLINE
 * \endcode
 */
static Stmt *statement(Parser *parser) {
    switch (parser->current.type) {
        case TOKEN_KW_WHILE: {
            consume(parser);
            Expr *cond = expression(parser);
            if (!(cond && match(parser, TOKEN_COLON, "expected ':'"))) {
                return NULL;
            }
            Stmt *body = block(parser);
            return body ? ast_create_stmt_while(parser->arena, cond, body) : NULL;
        }
        case TOKEN_KW_IF:
            // `if` differs from `elif` only in the keyword, we can use the same function to parse both
            return elif_block(parser);
        default: {
            Stmt *stmt = simple_statement(parser);
            if (stmt && match(parser, TOKEN_NEWLINE, "expected end of line")) {
                return stmt;
            }
            return NULL;
        }
    }
}

/**
 * \code
 * statements: statement+
 * \endcode
 */
static Stmt *statements(Parser *parser, TokenType sentinel) {
    Stmt *result = statement(parser);
    Stmt *last = result;
    while (last && parser->current.type != sentinel) {
        last->next = statement(parser);
        last = last->next;
    }
    return last ? result : NULL;
}

/**
 * \code
 * block: NEWLINE INDENT statements DEDENT
 * \endcode
 */
static Stmt *block(Parser *parser) {
    if (!match(parser, TOKEN_NEWLINE, "newline expected")) {
        return NULL;
    }
    if (!match(parser, TOKEN_INDENT, "indent expected")) {
        return NULL;
    }
    Stmt *result = statements(parser, TOKEN_DEDENT);
    if (result) {
        assert(parser->current.type == TOKEN_DEDENT);
        consume(parser);
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
