/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

/**
 * \file nx_str.h
 * \brief Representation and operations of natrix `str` objects.
 */

#ifndef NX_STR_H
#define NX_STR_H
#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>
#include <stdbool.h>
#include "natrix/obj/defs.h"

/**
 * \brief Layout of `str` instances.
 *
 * The length of the string is given explicitly, the string may contain null bytes. However, the string is
 * also always null-terminated, so it can be used safely with standard C library functions.
 * Strings are sequences of bytes, not characters. The encoding is currently not specified, but it is assumed
 * that only ASCII strings are used. Support for UTF-8 may be added in the future.
 */
typedef struct {
    NxObject header;            //!< Header common to all natrix objects
    const int64_t length;       //!< Number of bytes in the string, excluding the null terminator
    const char data[];          //!< Array of bytes, with an additional null terminator at the end
} NxStr;

/**
 * \brief Type of all `str` instances.
 */
extern const NxType nx_type_str;

/**
 * \brief Creates a new `str` object with the given contents.
 *
 * May trigger garbage collection.
 * TODO: define the behavior if the string is not valid ASCII/UTF-8
 * \param data the contents of the string, must not be NULL, if it points to a GC-managed object, it must be rooted
 * \param length length of the string, must not be negative
 * \return the new str object
 */
NxObject *nx_str_create(const char *data, int64_t length);

/**
 * \brief Determines whether the object is an instance of the `str` type.
 * \param object the object to check
 * \return true if the object is an instance of the `str` type, false otherwise
 */
static inline bool nx_str_is_instance(NxObject *object) {
    return object->type == &nx_type_str;
}

/**
 * \brief Returns pointer to the value of the `str` object.
 *
 * The returned pointer is valid as long as the `str` object is not deallocated.
 * \param object the `str` object
 * \return pointer to the value of the `str` object
 */
static inline const char *nx_str_get_cstr(NxObject *object) {
    assert(nx_str_is_instance(object));
    return ((NxStr *) object)->data;
}

/**
 * \brief Returns the number of codepoints in the `str` object.
 * The length does not include the null terminator. Currently we assume that the string is ASCII, so the number of
 * bytes is equal to the number of codepoints.
 * \param object the `str` object
 * \return the length of the `str` object
 */
static inline int64_t nx_str_get_length(NxObject *object) {
    assert(nx_str_is_instance(object));
    return ((NxStr *) object)->length;
}

/**
 * \brief Concatenates two `str` objects.
 *
 * May trigger garbage collection.
 * \param left the left operand, must be a rooted instance of `str`
 * \param right the right operand, must be a rooted instance of `str`
 * \return the new `str` object
 */
NxObject *nx_str_concat(NxObject *left, NxObject *right);

#ifdef __cplusplus
}
#endif
#endif //NX_STR_H
