/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

/**
 * \file nx_int.h
 * \brief Representation and operations of natrix `int` objects.
 */

#ifndef NX_INT_H
#define NX_INT_H
#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include "natrix/obj/defs.h"

/**
 * \brief Defines the layout of a natrix `int` object.
 *
 * The `int` object is a simple wrapper around a 64-bit integer value. It is immutable.
 * For simplicity, the `int` object has fixed precision and overflow behavior is inherited from the underlying C type.
 * This may change as the language evolves - either arbitrary precision or overflow checking could be implemented.
 */
typedef struct {
    NxObject header;        //!< Header common to all natrix objects
    const int64_t value;    //!< The value of the `int` object
} NxInt;

/**
 * \brief Type of all `int` objects.
 */
extern const NxType nx_type_int;

/**
 * \brief Creates a new natrix `int` object.
 *
 * May trigger garbage collection.
 * \param value the value of the `int` object
 * \return the new `int` object
 */
NxObject *nx_int_create(int64_t value);

/**
 * \brief Determines whether the object is an instance of the `int` type.
 * \param object the object to check
 * \return true if the object is an instance of the `int` type, false otherwise
 */
static inline bool nx_int_is_instance(NxObject *object) {
    return object->type == &nx_type_int;
}

/**
 * \brief Returns the value of the `int` object.
 * \param object the `int` object
 * \return the value of the `int` object
 */
static inline int64_t nx_int_get_value(NxObject *object) {
    assert(nx_int_is_instance(object));
    return ((NxInt *) object)->value;
}

#ifdef __cplusplus
}
#endif
#endif //NX_INT_H
