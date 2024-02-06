/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

/**
 * \file sb.h
 * \brief A simple builder for C strings.
 *
 * Provides functions for building strings by appending characters, strings or formatted strings.
 * The string builder automatically grows its capacity as needed. The string is always null-terminated.
 * The string builder needs to be freed with `sb_free` when it is no longer needed.
 * The functions never fail. If necessary memory cannot be allocated, the program will panic.
 */

#ifndef SB_H
#define SB_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

/**
 * \brief The state of the string builder.
 *
 * The members should not be modified directly, use the provided functions instead.
 */
typedef struct {
    char *str;          //!< The string data, always null-terminated
    size_t length;      //!< The length of the string excluding the null-terminator
    size_t capacity;    //!< The capacity of the allocated memory block, always greater than or equal to length + 1
} StringBuilder;

/**
 * \brief Initializes the string builder with default capacity.
 * \param sb the string builder to initialize
 */
void sb_init(StringBuilder *sb);

/**
 * \brief Initializes the string builder with given capacity.
 * \param sb the string builder to initialize
 * \param initial_capacity the initial capacity of the string builder, must be greater than 0
 */
void sb_init_with_capacity(StringBuilder *sb, size_t initial_capacity);

/**
 * \brief Destroys the string builder.
 * \param sb the string builder to destroy
 */
void sb_free(StringBuilder *sb);

/**
 * \brief Ensures that the string builder has at least the given capacity.
 *
 * If the capacity is already greater than or equal to the given capacity, does nothing.
 * May over-allocate the memory to avoid frequent reallocations.
 * \param sb the string builder
 * \param min_capacity the minimum capacity
 */
void sb_ensure_capacity(StringBuilder *sb, size_t min_capacity);

/**
 * \brief Ensures that the string builder has enough capacity to append the given length.
 *
 * If the capacity already allows appending the given length, does nothing.
 * May over-allocate the memory to avoid frequent reallocations.
 * \param sb the string builder
 * \param length the number of characters to be appended
 */
void sb_ensure_can_append(StringBuilder *sb, size_t length);

/**
 * \brief Appends a character to the string builder.
 * \param sb the string builder to append to
 * \param c the character to append
 */
void sb_append_char(StringBuilder *sb, char c);

/**
 * \brief Appends a null-terminated string to the string builder.
 * \param sb the string builder to append to
 * \param str the string to append, must be null-terminated
 */
void sb_append_str(StringBuilder *sb, const char *str);

/**
 * \brief Appends a string of given length to the string builder.
 * \param sb the string builder to append to
 * \param str the string to append
 * \param length the length of the string
 */
void sb_append_str_len(StringBuilder *sb, const char *str, size_t length);

/**
 * \brief Appends a null-terminated string to the string builder, escaping special characters.
 * \param sb the string builder to append to
 * \param str the string to append, must be null-terminated
 */
void sb_append_escaped_str(StringBuilder *sb, const char *str);

/**
 * \brief Appends a string of given length to the string builder, escaping special characters.
 *
 * Escapes all characters with ASCII code less than 32 or greater than 126, and also the characters `\\` and `\"`.
 * \param sb the string builder to append to
 * \param str the string to append
 * \param length the length of the string
 */
void sb_append_escaped_str_len(StringBuilder *sb, const char *str, size_t length);

/**
 * \brief Appends a formatted string to the string builder.
 * \param sb the string builder to append to
 * \param format the format string, as in `printf`
 * \param ... the format arguments
 */
void sb_append_formatted(StringBuilder *sb, const char *format, ...) __attribute__((format(printf, 2, 3)));

#ifdef __cplusplus
}
#endif
#endif //SB_H
