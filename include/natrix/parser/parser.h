/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

/**
 * \file parser.h
 * \brief Parser for the natrix language.
 *
 * The parser is a recursive descent parser which constructs an abstract syntax tree from the source code.
 * It relies on the lexer to provide tokens. See \ref natrix_grammar for the grammar of the language.
 */

#ifndef PARSER_H
#define PARSER_H
#ifdef __cplusplus
extern "C" {
#endif

#include "natrix/parser/ast.h"
#include "natrix/parser/diag.h"

/**
 * \brief Parses a natrix source.
 * \param arena memory arena to allocate the abstract syntax tree nodes from
 * \param source source code to parse
 * \param diag_handler diagnostics handler for reporting errors
 * \param diag_data private data of the diagnostics handler
 * \return the root of the abstract syntax tree representing the source or NULL if the source contains errors
 */
Stmt *parse_file(Arena *arena, Source *source, DiagHandler diag_handler, void *diag_data);

#ifdef __cplusplus
}
#endif

#endif //PARSER_H
