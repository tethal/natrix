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
#include <assert.h>
#include "natrix/util/mem.h"

/**
 * \brief Initializes the source code structure by copying the filename and the source code.
 *
 * All occurrences of `\r\n` and standalone `\r` are replaced with `\n`. Ensures that the last character is `\n`
 * and that the string is null-terminated.
 * \param filename the name of the file, will be copied to a new buffer
 * \param src pointer to the start of the source code
 * \param src_end pointer to the character after the last character of the source code
 * \param dst pre-allocated destination buffer, must be at least `src_end - src + 2` bytes long, may be the same as `src`
 */
static Source init_source(const char *filename, const char *src, const char *src_end, char *dst) {
    char *filename_copy = nx_alloc(strlen(filename) + 1);
    strcpy(filename_copy, filename);

    const char *start = dst;
    size_t line_count = 1;
    while (src != src_end) {
        char c = *src++;
        if (c == '\r') {
            c = '\n';
            if (*src == '\n') {
                src++;
            }
        }
        if (c == '\n') {
            line_count++;
        }
        *dst++ = c;
    }
    if (dst == start || dst[-1] != '\n') {
        *dst++ = '\n';
        line_count++;
    }
    *dst = '\0';

    return (Source) {
        .filename = filename_copy,
        .start = start,
        .end = dst,
        .line_count = line_count,
        .line_starts = NULL,
    };
}

/**
 * \brief Returns an array of pointers to the start of each line in the source code.
 *
 * The array is cached in the source code structure and freed when the source code is freed.
 * \param source pointer to the source code
 * \return array of pointers to the start of each line
 */
static const char **get_line_starts(Source *source) {
    if (source->line_starts == NULL) {
        source->line_starts = (const char **) nx_alloc((source->line_count + 1) * sizeof(char *));
        const char **line_start = source->line_starts;
        const char *ptr = source->start;
        while (ptr < source->end) {
            *line_start++ = ptr;
            while (*ptr != '\n') {
                ptr++;
            }
            ptr++;
        }
        *line_start++ = ptr;
        *line_start = ptr + 1;      // simplifies the implementation of source_get_line_end
    }
    return source->line_starts;
}

Source source_from_file(const char *filename) {
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
    return init_source(filename, buffer, buffer + file_size, buffer);

error:
    if (file) {
        fclose(file);
    }
    return (Source) {};
}

Source source_from_string(const char *filename, const char *string) {
    size_t src_len = strlen(string);
    char *buffer = nx_alloc(src_len + 2);
    return init_source(filename, string, string + src_len, buffer);
}

void source_free(Source *source) {
    nx_free((char *) source->filename);
    nx_free((char *) source->start);
    nx_free(source->line_starts);
}

size_t source_get_line_number(Source *source, const char *position) {
    assert(source->start <= position && position <= source->end);
    const char **line_starts = get_line_starts(source);
    size_t low = 0;
    size_t high = source->line_count;
    while (low < high) {
        size_t mid = (low + high) / 2;
        if (position < line_starts[mid]) {
            high = mid;
        } else {
            low = mid + 1;
        }
    }
    return low;
}

const char *source_get_line_start(Source *source, size_t line) {
    assert(line > 0 && line <= source->line_count);
    return get_line_starts(source)[line - 1];
}

const char *source_get_line_end(Source *source, size_t line) {
    assert(line > 0 && line <= source->line_count);
    return get_line_starts(source)[line] - 1;
}
