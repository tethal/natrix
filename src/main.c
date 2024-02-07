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
#include "natrix/parser/source.h"

/**
 * \brief Entry point of the interpreter.
 * \return 0 if successful, 1 otherwise
 */
int main(const int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }
    Source source;
    if (!source_from_file(&source, argv[1])) {
        fprintf(stderr, "Unable to read file %s\n", argv[1]);
        return 1;
    }
    printf("Length: %zu bytes\nSource code:\n%s", source.end - source.start, source.start);
    source_free(&source);
}
