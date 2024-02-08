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

const char *token_get_type_name(const TokenType type) {
    switch (type) {
        #define TT(x) case TOKEN_##x: return #x;
        #include "natrix/parser/token_types.inc"
        #undef TT
        default:
            return "UNKNOWN";
    }
}

void token_to_string(const Token *token, StringBuilder *sb) {
    sb_append_str(sb, token_get_type_name(token->type));
    sb_append_str(sb, "(\"");
    sb_append_escaped_str_len(sb, token->start, token->end - token->start);
    sb_append_str(sb, "\")");
}
