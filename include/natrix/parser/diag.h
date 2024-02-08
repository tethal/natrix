/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

/**
 * \file diag.h
 * \brief Types and functions for reporting diagnostic messages.
 *
 * Provides abstraction for reporting diagnostic messages (such as errors and warnings) from the parser and
 * other parts of the compiler. Usually, the diagnostic messages are printed to the standard error output.
 * However, by abstracting the reporting mechanism, it is possible to capture the diagnostic messages and
 * check them in the unit tests or turn them into runtime exceptions of the guest language.
 */

#ifndef DIAG_H
#define DIAG_H
#ifdef __cplusplus
extern "C" {
#endif

#include "natrix/parser/source.h"

/**
 * \brief Kind of the diagnostic message.
 */
typedef enum {
    DIAG_ERROR,         //!< Error message
    DIAG_WARNING,       //!< Warning message
} DiagKind;

/**
 * \brief Function pointer type for reporting diagnostic messages.
 * \param data private data of the handler
 * \param kind kind of the diagnostic message
 * \param source the source code
 * \param start pointer to the source code where the error/warning starts
 * \param end pointer to the source code where the error/warning ends
 * \param fmt format string, as in printf
 * \param ... arguments
 */
typedef void (*DiagHandler)(void *data, DiagKind kind, Source *source, const char *start, const char *end, const char *fmt, ...) __attribute__((format(printf, 6, 7)));

/**
 * \brief Default handler which prints the diagnostic message to the standard error output.
 * \param data private data of the handler
 * \param kind kind of the diagnostic message
 * \param source the source code
 * \param start pointer to the source code where the error/warning starts
 * \param end pointer to the source code where the error/warning ends
 * \param fmt format string, as in printf
 * \param ... arguments
 */
void diag_default_handler(void *data, DiagKind kind, Source *source, const char *start, const char *end, const char *fmt, ...) __attribute__((format(printf, 6, 7)));

#ifdef __cplusplus
}
#endif
#endif //DIAG_H
