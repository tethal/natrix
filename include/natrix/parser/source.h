/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

/**
 * \file source.h
 * \brief Utilities for manipulating source code.
 */

#ifndef SOURCE_H
#define SOURCE_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

/**
 * \brief Structure representing source code.
 *
 * Upon creation, the source code is read from a file or a string, all line endings are normalized to `\n`,
 * a newline is appended to the end of the source code unless already present, the string is null-terminated,
 * and the end pointer is set to the null terminator.
 * The structure contains copies of the filename and the source code and needs to be freed with `source_free`.
 * The members should not be modified.
 */
typedef struct {
    const char *filename;     //!< Name of the file containing the source code
    const char *start;        //!< Null-terminated string containing the source code
    const char *end;          //!< Pointer to the null terminator of the source code
} Source;

/**
 * \brief Initializes the source code from a file.
 *
 * The file is read into memory and the source code is normalized as described in the structure documentation.
 * If the file cannot be read, the function sets all members of the source structure to NULL and returns false.
 * \param source the source code to initialize
 * \param filename the name of the file containing the source code
 * \return true if the file was successfully read, false otherwise
 */
bool source_from_file(Source *source, const char *filename);

/**
 * \brief Initializes the source code from a string.
 *
 * Copies the source code while normalizing line endings as described in the structure documentation.
 * \param source the source code to initialize
 * \param filename the name of the source code, used for error messages
 * \param string the source code, must be null-terminated
 */
void source_from_string(Source *source, const char *filename, const char *string);

/**
 * \brief Frees the memory allocated for the source code.
 * \param source the source code to free
 */
void source_free(Source *source);

#ifdef __cplusplus
}
#endif
#endif //SOURCE_H
