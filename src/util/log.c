/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

/**
 * \file log.c
 * \brief Logging utilities.
 */

#include "natrix/util/log.h"
#include <stdio.h>

void log_message(int line, const char *file, const char *func, const char *kind, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log_message_v(line, file, func, kind, fmt, args);
    va_end(args);
}

void log_message_v(int line, const char *file, const char *func, const char *kind, const char *fmt, va_list args) {
    fprintf(stderr, "%s:%d: %s in %s: ", file, line, kind, func);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
}
