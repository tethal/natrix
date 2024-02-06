/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

/**
 * \file panic.c
 * \brief Panic function implementation
 */

#include "natrix/util/panic.h"
#include <stdlib.h>
#include "natrix/util/log.h"

void panic(int line, const char *file, const char *func, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log_message_v(line, file, func, "PANIC", fmt, args);
    va_end(args);
    exit(1);
}
