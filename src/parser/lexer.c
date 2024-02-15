/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

/**
 * \file lexer.c
 * \brief Implementation of the lexer.
 */

#include "natrix/parser/lexer.h"
#include <assert.h>
#include <ctype.h>

#if ENABLE_TOKEN_LOGGING
#include "natrix/util/log.h"
#endif

/**
 * \brief Creates a token of the given type with the start and end pointers determined from the lexer state.
 * \param lexer pointer to the lexer
 * \param type type of the token
 * \return the token
 */
static Token make_token(const Lexer *lexer, TokenType type) {
    Token token = (Token) {type, lexer->start, lexer->current};
#if ENABLE_TOKEN_LOGGING
    StringBuilder sb = sb_init();
    token_to_string(&token, &sb);
    LOG_INFO("%s", sb.str);
    sb_free(&sb);
#endif
    return token;
}

/**
 * \brief Skips whitespace and comments.
 *
 * This function is called after line indentation has been handled and before the next token is parsed.
 * It also sets the start position of the token.
 * If a comment is encountered, the lexer skips characters until the end of the line. However, the start
 * of the comment is remembered as the position of the NEWLINE token, so that error messages are reported
 * correctly. Consider the following example:
 * \code
 *     x = a * (b - 1     # this is a comment
 *                        ^-- the error 'expected closing parenthesis' is reported here, not at the end of the line
 * \endcode
 * \param lexer pointer to the lexer
 */
static void skip_whitespace(Lexer *lexer) {
    while (*lexer->current == ' ') {
        lexer->current++;
    }
    lexer->start = lexer->current;      // this is where the token starts
    if (*lexer->current == '#') {
        // skip characters until the end of the line
        while (*lexer->current != '\n') {
            assert(*lexer->current != '\0' && "source code must end with a newline");
            lexer->current++;
        }
    }
}

/**
 * \brief Handles indentation and dedentation.
 *
 * This function is called after the lexer has skipped all spaces at the beginning of the line.
 * It compares the current indentation level with the last value on the indentation stack and
 * generates INDENT or DEDENT tokens as necessary. Since more than one DEDENT token can be produced
 * on a single line, only the first DEDENT token is returned immediately and the rest are stored
 * in the `pending_dedents` variable.
 * \param lexer pointer to the lexer
 * \param indent the current indentation level, which must be different from the last indentation level on the stack
 * \return the type of the token, either `TOKEN_INDENT` or `TOKEN_DEDENT`
 */
static TokenType handle_indentation_change(Lexer *lexer, size_t indent) {
    const size_t last_indent = lexer->indent_stack[lexer->indent_stack_size - 1];
    if (indent > last_indent) {
        // new indentation level
        lexer->start = lexer->current - indent + last_indent;
        if (lexer->indent_stack_size == MAX_INDENT_STACK) {
            lexer->error_message = "too many indentation levels";
            return TOKEN_ERROR;
        }
        lexer->indent_stack[lexer->indent_stack_size++] = indent;
        return TOKEN_INDENT;
    } else {
        assert(indent < last_indent);
        assert(lexer->pending_dedents == 0);
        // dedent
        lexer->start = lexer->current - indent;
        while (lexer->indent_stack_size > 1 && indent < lexer->indent_stack[lexer->indent_stack_size - 1]) {
            lexer->pending_dedents++;
            lexer->indent_stack_size--;
        }
        if (indent != lexer->indent_stack[lexer->indent_stack_size - 1]) {
            lexer->error_message = "unindent does not match any outer indentation level";
            return TOKEN_ERROR;
        }
        assert(lexer->pending_dedents > 0);
        lexer->pending_dedents--;
        return TOKEN_DEDENT;
    }
}

/**
 * \brief Compares the given keyword with the token and returns the appropriate token type.
 * \param ptr pointer to the start of the token
 * \param end pointer to the end of the token
 * \param keyword the keyword to compare
 * \param type the type of the token if the keyword matches
 * \return the type of the token, either `type` or `TOKEN_IDENTIFIER`
 */
static TokenType check_keyword(const char *ptr, const char *end, const char *keyword, TokenType type) {
    while (ptr < end && *keyword != '\0' && *ptr == *keyword) {
        ptr++;
        keyword++;
    }
    return ptr == end && *keyword == '\0' ? type : TOKEN_IDENTIFIER;
}

/**
 * \brief Determines whether the identifier is a keyword and returns the appropriate token type.
 * \param start pointer to the start of the token
 * \param end pointer to the end of the token
 * \return the type of the token
 */
static TokenType handle_identifier(const char *start, const char *end) {
    assert(start < end);
    switch (*start) {
        case 'e':
            if (start + 2 < end && start[1] == 'l') {
                switch (start[2]) {
                    case 's':
                        return check_keyword(start, end, "else", TOKEN_KW_ELSE);
                    case 'i':
                        return check_keyword(start, end, "elif", TOKEN_KW_ELIF);
                }
            }
            break;
        case 'i':
            return check_keyword(start, end, "if", TOKEN_KW_IF);
        case 'p':
            if (start + 1 < end) {
                switch (start[1]) {
                    case 'a':
                        return check_keyword(start, end, "pass", TOKEN_KW_PASS);
                    case 'r':
                        return check_keyword(start, end, "print", TOKEN_KW_PRINT);
                }
            }
            break;
        case 'w':
            return check_keyword(start, end, "while", TOKEN_KW_WHILE);
    }
    return TOKEN_IDENTIFIER;
}

/**
 * \brief Parses the next token from the source code.
 *
 * This function is called after the lexer has reported any pending dedents.
 * First, it checks for indentation changes if at the beginning of the line.
 * Then whitespace and comments are skipped.
 * Finally, it determines the type of the next token, advances the current pointer to the end of the token,
 * and returns the type of the token.
 * \param lexer pointer to the lexer
 * \return the type of the token
 */
static TokenType parse_token(Lexer *lexer) {
    assert(lexer->pending_dedents == 0);
    if (lexer->empty_line) {
        // handle start of the line
        size_t indent = 0;
        while (*lexer->current == ' ') {
            lexer->current++;
            indent++;
        }
        if (*lexer->current != '#' && *lexer->current != '\n' && indent != lexer->indent_stack[lexer->indent_stack_size - 1]) {
            return handle_indentation_change(lexer, indent);
        }
    }

    skip_whitespace(lexer);
    if (isdigit(*lexer->current)) {
        while (isdigit(*lexer->current)) {
            lexer->current++;
        }
        return TOKEN_INT_LITERAL;
    }
    if (isalpha(*lexer->current) || *lexer->current == '_') {
        while (isalnum(*lexer->current) || *lexer->current == '_') {
            lexer->current++;
        }
        return handle_identifier(lexer->start, lexer->current);
    }
    switch (*lexer->current++) {
        case '\0':
            // rewind the current pointer so that the EOF token is returned on subsequent calls
            lexer->current--;
            return TOKEN_EOF;
        case '\n':
            return TOKEN_NEWLINE;
        case '+':
            return TOKEN_PLUS;
        case '-':
            return TOKEN_MINUS;
        case '*':
            return TOKEN_STAR;
        case '/':
            return TOKEN_SLASH;
        case '(':
            return TOKEN_LPAREN;
        case ')':
            return TOKEN_RPAREN;
        case '[':
            return TOKEN_LBRACKET;
        case ']':
            return TOKEN_RBRACKET;
        case ',':
            return TOKEN_COMMA;
        case '=':
            if (*lexer->current == '=') {
                lexer->current++;
                return TOKEN_EQ;
            }
            return TOKEN_EQUALS;
        case ':':
            return TOKEN_COLON;
        case '!':
            if (*lexer->current == '=') {
                lexer->current++;
                return TOKEN_NE;
            }
            lexer->error_message = "invalid syntax";
            return TOKEN_ERROR;
        case '<':
            if (*lexer->current == '=') {
                lexer->current++;
                return TOKEN_LE;
            }
            return TOKEN_LT;
        case '>':
            if (*lexer->current == '=') {
                lexer->current++;
                return TOKEN_GE;
            }
            return TOKEN_GT;
        case '"':
            // todo escape sequences
            while (*lexer->current != '"') {
                assert(*lexer->current != '\0');    // source code must end with a newline
                if (*lexer->current == '\n') {
                    lexer->error_message = "unterminated string";
                    return TOKEN_ERROR;
                }
                lexer->current++;
            }
            lexer->current++;
            return TOKEN_STRING_LITERAL;
        default:
            lexer->error_message = "unexpected character";
            return TOKEN_ERROR;
    }
}

void lexer_init(Lexer *lexer, const char *source) {
    lexer->start = source;
    lexer->current = source;
    lexer->empty_line = true;
    lexer->error_message = NULL;
    lexer->indent_stack[0] = 0;
    lexer->indent_stack_size = 1;
    lexer->pending_dedents = 0;
}

const char *lexer_error_message(Lexer *lexer) {
    return lexer->error_message;
}

Token lexer_next_token(Lexer *lexer) {
    assert(lexer->error_message == NULL);

    while (1) {
        // handle pending dedents
        if (lexer->pending_dedents > 0) {
            lexer->pending_dedents--;
            return make_token(lexer, TOKEN_DEDENT);
        }

        // determine the next token type
        TokenType tt = parse_token(lexer);
        if (tt == TOKEN_NEWLINE && lexer->empty_line) {
            // skip empty lines
            continue;
        }
        lexer->empty_line = tt == TOKEN_NEWLINE;
        return make_token(lexer, tt);
    }
}
