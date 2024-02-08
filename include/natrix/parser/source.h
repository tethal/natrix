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
#include <stddef.h>

/**
 * \brief Structure representing source code.
 *
 * Upon creation, the source code is read from a file or a string, all line endings are normalized to `\n`,
 * a newline is appended to the end of the source code unless already present, the string is null-terminated,
 * and the end pointer is set to the null terminator.
 * The structure contains copies of the filename and the source code and needs to be freed with `source_free`.
 * The members should not be modified directly, use the provided functions instead.
 */
typedef struct {
    const char *filename;     //!< Name of the file containing the source code
    const char *start;        //!< Null-terminated string containing the source code
    const char *end;          //!< Pointer to the null terminator of the source code
    const char **line_starts; //!< Array of pointers to the start of each line
    size_t line_count;        //!< Number of lines in the source code
} Source;

/**
 * \brief Initializes the source code from a file.
 *
 * The file is read into memory and the source code is normalized as described in the structure documentation.
 * If the file cannot be read, the function sets all members of the source structure to NULL or zero.
 * \param filename the name of the file containing the source code
 * \return the initialized source code (filled with NULLs if the file cannot be read)
 */
Source source_from_file(const char *filename);

/**
 * \brief Initializes the source code from a string.
 *
 * Copies the source code while normalizing line endings as described in the structure documentation.
 * \param filename the name of the source code, used for error messages
 * \param string the source code, must be null-terminated
 * \return the initialized source code
 */
Source source_from_string(const char *filename, const char *string);

/**
 * \brief Frees the memory allocated for the source code.
 * \param source the source code to free
 */
void source_free(Source *source);

/**
 * \brief Returns the line number of the given position in the source code.
 * \param source the source code
 * \param position the position in the source code, must be between `source->start` and `source->end`, inclusive
 * \return the 1-based line number
 */
size_t source_get_line_number(Source *source, const char *position);

/**
 * \brief Returns the start of the given line in the source code.
 * \param source the source code
 * \param line the 1-based line number, must be between 1 and `source->line_count`, inclusive
 * \return pointer to the start of the line
 */
const char *source_get_line_start(Source *source, size_t line);

/**
 * \brief Returns the end of the given line in the source code.
 * \param source the source code
 * \param line the 1-based line number, must be between 1 and `source->line_count`, inclusive
 * \return pointer to the newline character or the null terminator at the end of the line
 */
const char *source_get_line_end(Source *source, size_t line);

#ifdef __cplusplus
}
#endif
#endif //SOURCE_H
