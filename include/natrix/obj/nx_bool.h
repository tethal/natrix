/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

/**
 * \file nx_bool.h
 * \brief Representation and operations of natrix `bool` objects.
 */

#ifndef NX_BOOL_H
#define NX_BOOL_H
#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>
#include <stdbool.h>
#include "natrix/obj/defs.h"

/**
 * \brief Layout of `bool` instances.
 */
typedef struct {
    NxObject header;            //!< Header common to all natrix objects
    bool value;                 //!< The value
} NxBool;

/**
 * \brief Type of both `bool` instances.
 */
extern const NxType nx_type_bool;

/**
 * \brief The `false` object.
 */
extern NxObject *nx_false;

/**
 * \brief The `true` object.
 */
extern NxObject *nx_true;

/**
 * \brief Converts a C boolean value to a natrix `bool` object.
 * \param value the C boolean value
 * \return the natrix `bool` object
 */
static inline NxObject *nx_bool_wrap(bool value) {
    return value ? nx_true : nx_false;
}

/**
 * \brief Determines whether the object is an instance of the `bool` type.
 * \param object the object to check
 * \return true if the object is an instance of the `bool` type, false otherwise
 */
static inline bool nx_bool_is_instance(NxObject *object) {
    return object->type == &nx_type_bool;
}

/**
 * \brief Determines the C boolean value of a natrix `bool` object.
 * \param object the object to check
 * \return the C boolean value of the object
 */
static inline bool nx_bool_is_true(NxObject *object) {
    assert(nx_bool_is_instance(object));
    return ((NxBool *) object)->value;
}

#ifdef __cplusplus
}
#endif
#endif //NX_BOOL_H
