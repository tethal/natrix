/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

/**
 * \file panic.h
 * \brief Macro for aborting the program with a message.
 */

#ifndef PANIC_H
#define PANIC_H
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Prints a message to stderr and exits the program.
 *
 * This macro is used in situations where the program reaches an unrecoverable state.
 * \param fmt format string, as in printf
 * \param ... arguments
 */
#define PANIC(fmt, ...) panic(__LINE__, __FILE__, __func__, fmt, ##__VA_ARGS__)

/**
 * \brief Prints a message to stderr and exits the program.
 * \param line line number
 * \param file file name
 * \param func function name
 * \param fmt format string, as in printf
 * \param ... arguments
 */
void panic(int line, const char *file, const char *func, const char *fmt, ...) __attribute__((noreturn, cold, format(printf, 4, 5)));

#ifdef __cplusplus
}
#endif
#endif //PANIC_H
