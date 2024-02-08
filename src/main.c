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

#include <stdio.h>
#include "natrix/parser/diag.h"
#include "natrix/parser/lexer.h"

/**
 * \brief Prints a token to the standard output.
 * \param source the source code
 * \param token the token to print
 * \param last_line pointer the line number of the last printed token, will be updated
 */
static void print_token(Source *source, Token *token, size_t *last_line) {
    StringBuilder sb = sb_init();
    size_t line = source_get_line_number(source, token->start);
    size_t column = token->start - source_get_line_start(source, line) + 1;
    if (line != *last_line) {
        *last_line = line;
        sb_append_formatted(&sb, "%4zu ", line);
    } else {
        sb_append_str(&sb, "   . ");
    }
    sb_append_formatted(&sb, "%4zu %4zu %-15s ", column, token->end - token->start, token_get_type_name((*token).type));
    sb_append_escaped_str_len(&sb, token->start, token->end - token->start);
    puts(sb.str);
    sb_free(&sb);
}

/**
 * \brief Prints a table of tokens in the source code to the standard output.
 * \param source the source code
 */
void print_tokens(Source *source) {
    Lexer lexer;
    lexer_init(&lexer, source->start);
    size_t last_line = 0;
    printf("Line  Col  Len Token           Lexeme\n");
    while (1) {
        Token token = lexer_next_token(&lexer);
        print_token(source, &token, &last_line);
        if (token.type == TOKEN_ERROR) {
            diag_default_handler(NULL, DIAG_ERROR, source, token.start, token.end, "%s", lexer_error_message(&lexer));
            break;
        }
        if (token.type == TOKEN_EOF) {
            break;
        }
    }
}

/**
 * \brief Entry point of the interpreter.
 * \return 0 if successful, 1 otherwise
 */
int main(const int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }
    Source source = source_from_file(argv[1]);
    if (!source.start) {
        fprintf(stderr, "Unable to read file %s\n", argv[1]);
        return 1;
    }
    print_tokens(&source);
    source_free(&source);
}
