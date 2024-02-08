/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

/**
 * \file lexer.h
 * \brief Lexer for the natrix programming language.
 *
 * The lexer is responsible for converting the source code into a sequence of tokens.
 * It reads the source code character by character and returns the next token each time it is called.
 * The lexer also provides the position of each token in the source code, which is useful for error messages.
 * Since natrix uses whitespace and indentation to delimit blocks of code, the lexer also keeps track of
 * the indentation level and returns TOKEN_INDENT and TOKEN_DEDENT tokens when the indentation level changes.
 * Newlines are also returned as tokens to separate statements. Comments are removed by the lexer and are not
 * returned as a separate token. Whitespace is ignored except for indentation and newlines.
 * Empty lines (lines containing only whitespace and comments) do not produce NEWLINE tokens.
 */

#ifndef LEXER_H
#define LEXER_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "token.h"

//! Maximum number of indentation levels.
#define MAX_INDENT_STACK 64

/**
 * \brief Represents the internal state of the lexer.
 *
 * Keeps track of the current position in the source code, indentation levels, and the current error message.
 * The members of this structure should not be accessed directly, but only through the lexer functions.
 */
typedef struct {
    const char *start;                      //!< Start of the current token
    const char *current;                    //!< Current character
    const char *error_message;              //!< Error message or NULL if no error occurred
    size_t indent_stack[MAX_INDENT_STACK];  //!< Stack of indentation levels
    int indent_stack_size;                  //!< Number of entries in the indentation stack
    int pending_dedents;                    //!< Number of pending dedents
    bool empty_line;                        //!< True if the current line contains only whitespace and comments
} Lexer;

/**
 * \brief Initializes the lexer with the given source code.
 *
 * The source code is not copied, so it must outlive the lexer and all tokens returned by it.
 * The source code must be null-terminated and normalized, i.e., it must use only `\n` as the
 * newline character and end with a newline.
 *
 * \param lexer the lexer to initialize
 * \param source the source code to parse
 */
void lexer_init(Lexer *lexer, const char *source);

/**
 * \brief Returns the next token from the source code.
 *
 * If the lexer encounters an error, it returns a token with the type TOKEN_ERROR. The token
 * contains the position of the error. The error message can be retrieved using `lexer_error_message`.
 * The lexer can not recover from an error and should not be used after an error is encountered.
 * When the end of the source code is reached, the lexer returns a token with the type TOKEN_EOF and
 * keeps returning it on subsequent calls.
 *
 * \param lexer the lexer previously initialized with `lexer_init`
 * \return the next token from the source code
 */
Token lexer_next_token(Lexer *lexer);

/**
 * \brief Returns the error message from the lexer.
 *
 * If the lexer encountered an error, this function returns the error message. Otherwise, it returns NULL.
 * The error message is static and should not be modified or freed.
 *
 * \param lexer the lexer previously initialized with `lexer_init`
 * \return the error message or NULL if no error occurred
 */
const char *lexer_error_message(Lexer *lexer);

#ifdef __cplusplus
}
#endif
#endif //LEXER_H
