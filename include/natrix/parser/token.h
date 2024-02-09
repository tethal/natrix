/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

/**
 * \file token.h
 * \brief Defines the representation of a token and associated functions.
 *
 * A token is a piece of the source code categorized by its type, such as a literal, a symbol, or a keyword.
 * Apart from the type, it also contains pointers to the start and end of the token in the source code,
 * providing both the position and the character sequence of the token.
 * Some tokens are empty (e.g., `TOKEN_EOF`), in which case the start and end pointers are equal.
 *
 * For example, the excerpt `12 + 30` contains three tokens:
 * \code
 *     TOKEN_INT_LITERAL "12"
 *     TOKEN_PLUS        "+"
 *     TOKEN_INT_LITERAL "30"
 * \endcode
 */

#ifndef TOKEN_H
#define TOKEN_H
#ifdef __cplusplus
extern "C" {
#endif

#include "natrix/util/sb.h"

/**
 * \brief Determines the type of the token.
 *
 * See `token_types.inc` for the list of token types. For each token type in the file, a corresponding enum value
 * is defined with the `TOKEN_` prefix. For example, `INT_LITERAL` is defined as `TOKEN_INT_LITERAL` and represents
 * any integer literal in the source code.
 */
typedef enum {
    #define TT(x) TOKEN_##x,
    #include "token_types.inc"
    #undef TT
    TOKEN_TYPE_COUNT
} TokenType;

/**
 * \brief Represents a token in the source code.
 *
 * The `start` and `end` fields point directly to the source code. They should never be modified or freed,
 * and should not be accessed after the source code is deallocated.
 */
typedef struct {
    TokenType type;             //!< Type of the token
    const char *start;          //!< Pointer to the start of the token in the source code
    const char *end;            //!< Pointer to the character after the end of the token in the source code
} Token;

/**
 * \brief Returns the name of the token type as a string.
 *
 * The returned string is a pointer to a static string and should not be modified or freed.
 * \param type type of the token
 * \return name of the token type or "UNKNOWN" if the type is not recognized
 */
const char *token_get_type_name(TokenType type);

/**
 * \brief Appends a string representation of the token for debugging purposes to the string builder.
 * \param token pointer to the token
 * \param sb pointer to the string builder
 */
void token_to_string(const Token *token, StringBuilder *sb);

#ifdef __cplusplus
}
#endif
#endif //TOKEN_H
