/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

/**
 * \file token.c
 * \brief Implementation of token functions.
 */

#include "natrix/parser/token.h"

/**
 * \brief Names of token types.
 */
static const char *TOKEN_TYPE_NAMES[] = {
    #define TT(x) [TOKEN_##x] = #x,
    #include "natrix/parser/token_types.inc"
    #undef TT
};

const char *token_get_type_name(const TokenType type) {
    return type >= 0 && type < TOKEN_TYPE_COUNT ? TOKEN_TYPE_NAMES[type] : "UNKNOWN";
}

void token_to_string(const Token *token, StringBuilder *sb) {
    sb_append_str(sb, token_get_type_name(token->type));
    sb_append_str(sb, "(\"");
    sb_append_escaped_str_len(sb, token->start, token->end - token->start);
    sb_append_str(sb, "\")");
}
