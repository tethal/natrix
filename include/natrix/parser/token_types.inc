/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

// special tokens
TT(EOF)                 // end of file
TT(ERROR)               // error
TT(NEWLINE)             // new line
TT(INDENT)              // indentation level increased
TT(DEDENT)              // indentation level decreased
TT(IDENTIFIER)          // [a-zA-Z_][a-zA-Z0-9_]*

// symbols
TT(PLUS)                // +
TT(MINUS)               // -
TT(STAR)                // *
TT(SLASH)               // /
TT(LPAREN)              // (
TT(RPAREN)              // )
TT(EQUALS)              // =
TT(COLON)               // :
TT(EQ)                  // ==
TT(NE)                  // !=
TT(GE)                  // >=
TT(GT)                  // >
TT(LE)                  // <=
TT(LT)                  // <

// literals
TT(INT_LITERAL)         // [0-9]+
TT(STRING_LITERAL)      // ".*"

// keywords
TT(KW_ELIF)             // elif
TT(KW_ELSE)             // else
TT(KW_IF)               // if
TT(KW_PASS)             // pass
TT(KW_PRINT)            // print
TT(KW_WHILE)            // while
