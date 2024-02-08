/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

/**
 * \file sb.c
 * \brief String builder implementation.
 */

#include "natrix/util/sb.h"
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "natrix/util/mem.h"
#include "natrix/util/panic.h"

//! Default initial capacity of the string builder.
#define DEFAULT_CAPACITY 16

StringBuilder sb_init() {
    return sb_init_with_capacity(DEFAULT_CAPACITY);
}

StringBuilder sb_init_with_capacity(size_t initial_capacity) {
    assert(initial_capacity > 0);
    char *str = nx_alloc(initial_capacity);
    str[0] = '\0';
    return (StringBuilder) {
        .length = 0,
        .capacity = initial_capacity,
        .str = str
    };
}

void sb_free(StringBuilder *sb) {
    assert(sb->str != NULL);
    nx_free(sb->str);
    sb->str = NULL;     // only to prevent use-after-free, all functions assert that sb->str != NULL
}

void sb_ensure_capacity(StringBuilder *sb, size_t min_capacity) {
    assert(sb->str != NULL);
    if (sb->capacity >= min_capacity) {
        return;
    }
    assert(sb->capacity < (1L << 63));  // prevent overflow
    size_t new_capacity = sb->capacity + (sb->capacity >> 1);   // grow by 50%
    if (new_capacity < min_capacity) {
        new_capacity = min_capacity;
    }
    sb->str = nx_realloc(sb->str, new_capacity);
    sb->capacity = new_capacity;
}

void sb_ensure_can_append(StringBuilder *sb, size_t length) {
    assert(sb->str != NULL);
    size_t min_capacity = sb->length + length + 1;
    assert(min_capacity > sb->length);  // check overflow
    sb_ensure_capacity(sb, min_capacity);
}

void sb_append_char(StringBuilder *sb, char c) {
    assert(sb->str != NULL);
    sb_ensure_can_append(sb, 1);
    sb->str[sb->length++] = c;
    sb->str[sb->length] = '\0';
}

void sb_append_str(StringBuilder *sb, const char *str) {
    assert(sb->str != NULL);
    sb_append_str_len(sb, str, strlen(str));
}

void sb_append_str_len(StringBuilder *sb, const char *str, size_t length) {
    assert(sb->str != NULL);
    sb_ensure_can_append(sb, length);
    memcpy(sb->str + sb->length, str, length);
    sb->length += length;
    sb->str[sb->length] = '\0';
}

void sb_append_escaped_str(StringBuilder *sb, const char *str) {
    assert(sb->str != NULL);
    sb_append_escaped_str_len(sb, str, strlen(str));
}

void sb_append_escaped_str_len(StringBuilder *sb, const char *str, size_t length) {
    assert(sb->str != NULL);
    sb_ensure_can_append(sb, length);   // only an estimate, but should be enough in most cases
    for (size_t i = 0; i < length; i++) {
        char c = str[i];
        switch (c) {
            case '\n':
                sb_append_str(sb, "\\n");
                break;
            case '\r':
                sb_append_str(sb, "\\r");
                break;
            case '\t':
                sb_append_str(sb, "\\t");
                break;
            case '\0':
                sb_append_str(sb, "\\0");
                break;
            case '\\':
                sb_append_str(sb, "\\\\");
                break;
            case '"':
                sb_append_str(sb, "\\\"");
                break;
            default:
                if (c < 32 || c > 126) {
                    sb_append_formatted(sb, "\\x%02x", c);
                } else {
                    sb_append_char(sb, c);
                }
                break;
        }
    }
}

void sb_append_formatted(StringBuilder *sb, const char *format, ...) {
    assert(sb->str != NULL);
    va_list args;
    va_start(args, format);
    int char_count = vsnprintf(NULL, 0, format, args);
    if (char_count < 0) {
        PANIC("vsnprintf failed");
    }
    va_end(args);
    sb_ensure_can_append(sb, char_count);
    va_start(args, format);
    int result = vsnprintf(sb->str + sb->length, char_count + 1, format, args);
    if (result != char_count) {
        PANIC("vsnprintf failed");
    }
    va_end(args);
    sb->length += char_count;
}
