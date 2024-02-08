/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

/**
 * \file diag.c
 * \brief Implementation of the default handler of diagnostic messages.
 */

#include "natrix/parser/diag.h"
#include <stdarg.h>
#include <stdio.h>

void diag_default_handler(void *data, DiagKind kind, Source *source, const char *start, const char *end, const char *fmt, ...) {
    (void) data;
    size_t line = source_get_line_number(source, start);
    const char *line_start = source_get_line_start(source, line);
    size_t column = start - line_start + 1;
    fprintf(stderr, "%s:%zu:%zu: %s: ", source->filename, line, column, kind == DIAG_WARNING ? "warning" : "error");
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");

    const char *line_end = source_get_line_end(source, line);
    if (line_start == line_end) {
        return;
    }
    fprintf(stderr, "%.*s\n", (int) (line_end - line_start), line_start);

    size_t length = end - start;
    if (length == 0 || end > line_end) {
        length = 1;
    }
    for (int i = 1; i < column; i++) {
        fprintf(stderr, " ");
    }
    for (int i = 0; i < length; i++) {
        fprintf(stderr, "^");
    }
    printf("\n");
}
