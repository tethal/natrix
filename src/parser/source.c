/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

/**
 * \file source.c
 * \brief Implementation of source code utilities.
 */

#include "natrix/parser/source.h"
#include <stdio.h>
#include <string.h>
#include "natrix/util/mem.h"

/**
 * \brief Initializes the source code structure by copying the filename and the source code.
 *
 * All occurrences of `\r\n` and standalone `\r` are replaced with `\n`. Ensures that the last character is `\n`
 * and that the string is null-terminated.
 * \param source the source code structure to initialize
 * \param filename the name of the file, will be copied to a new buffer
 * \param src pointer to the start of the source code
 * \param src_end pointer to the character after the last character of the source code
 * \param dst pre-allocated destination buffer, must be at least `src_end - src + 2` bytes long, may be the same as `src`
 */
static void init_source(Source *source, const char *filename, const char *src, const char *src_end, char *dst) {
    char *filename_copy = nx_alloc(strlen(filename) + 1);
    strcpy(filename_copy, filename);

    source->filename = filename_copy;
    source->start = dst;
    while (src != src_end) {
        if (*src == '\r') {
            *dst++ = '\n';
            src++;
            if (*src == '\n') {
                src++;
            }
        } else {
            *dst++ = *src++;
        }
    }
    if (dst == source->start || dst[-1] != '\n') {
        *dst++ = '\n';
    }
    *dst = '\0';

    source->end = dst;
}

bool source_from_file(Source *source, const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        goto error;
    }
    if (fseek(file, 0, SEEK_END) != 0) {
        goto error;
    }
    const long file_size = ftell(file);
    if (file_size < 0) {
        goto error;
    }
    if (fseek(file, 0, SEEK_SET) != 0) {
        goto error;
    }
    char *buffer = nx_alloc(file_size + 2);
    if (fread(buffer, 1, file_size, file) != (size_t) file_size) {
        nx_free(buffer);
        goto error;
    }
    fclose(file);
    init_source(source, filename, buffer, buffer + file_size, buffer);
    return true;

error:
    if (file) {
        fclose(file);
    }
    source->filename = NULL;
    source->start = NULL;
    source->end = NULL;
    return false;
}

void source_from_string(Source *source, const char *filename, const char *string) {
    size_t src_len = strlen(string);
    char *buffer = nx_alloc(src_len + 2);
    init_source(source, filename, string, string + src_len, buffer);
}

void source_free(Source *source) {
    nx_free((char *) source->filename);
    nx_free((char *) source->start);
}
