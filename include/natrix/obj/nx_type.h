/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

/**
 * \file nx_type.h
 * \brief Representation and operations of natrix `type` objects.
 */

#ifndef NX_TYPE_H
#define NX_TYPE_H
#ifdef __cplusplus
extern "C" {
#endif

#include "natrix/obj/nx_object.h"

/**
 * \brief Initializes the header of a type.
 *
 * This macro should be used to initialize the header of a statically allocated type, like this:
 * \code{.c}
 * const NxType nx_type_some_type = {
 *         NX_TYPE_HEADER_INIT("SomeType", some_type_trace_fn),
 *         // ... initialization of other fields
 * };
 * \endcode
 */
#define NX_TYPE_HEADER_INIT(type_name, trace_obj_fn)    \
        .header = {                                     \
                .gc_header = {                          \
                        .next = NULL,                   \
                        .trace_fn = gc_trace_nop,       \
                },                                      \
                .type = &nx_type_type,                  \
        },                                              \
        .name = type_name,                              \
        .gc_trace_fn = trace_obj_fn

/**
 * \brief Represents a natrix type.
 *
 * A type is also an object, so it has a header with type pointing to itself. Types are immutable.
 */
typedef struct NxType {
    NxObject header;                                //!< Header common to all natrix objects
    const char *name;                               //!< Name of the type, statically allocated
    GcTraceFn gc_trace_fn;                          //!< Function to trace pointers in objects of this type
    NxObject *(*as_bool_fn)(NxObject *self);        //!< Converts an object of this type to a boolean
    NxObject *(*get_element_fn)(NxObject *self, NxObject *index);        //!< Gets an element at the given index
    void (*set_element_fn)(NxObject *self, NxObject *index, NxObject *value);        //!< Sets an element at the given index
} NxType;

/**
 * \brief Type of all `type` objects.
 */
extern const NxType nx_type_type;

#ifdef __cplusplus
}
#endif
#endif //NX_TYPE_H
