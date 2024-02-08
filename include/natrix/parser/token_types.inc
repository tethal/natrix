/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

// special tokens
TT(EOF)           // end of file
TT(ERROR)         // error
TT(NEWLINE)       // new line
TT(INDENT)        // indentation level increased
TT(DEDENT)        // indentation level decreased

// symbols
TT(PLUS)          // +
TT(MINUS)         // -
TT(STAR)          // *
TT(SLASH)         // /
TT(LPAREN)        // (
TT(RPAREN)        // )

// literals
TT(INT_LITERAL)   // [0-9]+
