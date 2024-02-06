/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

/**
 * \file log.h
 * \brief Macros for logging and debugging.
 */

#ifndef LOG_H
#define LOG_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>

/**
 * \brief Prints an informational message to stderr.
 * \param fmt format string, as in printf
 * \param ... arguments
 */
#define LOG_INFO(fmt, ...) log_message(__LINE__, __FILE__, __func__, "info", fmt, ##__VA_ARGS__)

/**
 * \brief Prints a message to stderr.
 * \param line line number
 * \param file file name
 * \param func function name
 * \param kind kind of message (e.g. "error", "warning", "info")
 * \param fmt format string, as in printf
 * \param ... arguments
 */
void log_message(int line, const char *file, const char *func, const char *kind, const char *fmt, ...) __attribute__((format(printf, 5, 6)));

/**
 * \brief Prints a message to stderr.
 * \param line line number
 * \param file file name
 * \param func function name
 * \param kind kind of message (e.g. "error", "warning", "info")
 * \param fmt format string, as in printf
 * \param args arguments, as in vprintf
 */
void log_message_v(int line, const char *file, const char *func, const char *kind, const char *fmt, va_list args) __attribute__((format(printf, 5, 0)));

#ifdef __cplusplus
}
#endif
#endif //LOG_H
