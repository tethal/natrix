/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

/**
 * \file nx_object_array.h
 * \brief Fixed-size array of references to natrix objects.
 *
 * These arrays are not natrix objects themselves, but they are used by other, higher-level natrix objects
 * such as lists.
 */

#ifndef NX_OBJECT_ARRAY_H
#define NX_OBJECT_ARRAY_H
#ifdef __cplusplus
extern "C" {
#endif

#include "natrix/obj/nx_object.h"

/**
 * \brief Layout of the natrix object array.
 */
typedef struct {
    GcHeader gc_header;                 //!< Header common to all natrix objects
    const int64_t size;                 //!< Capacity of the array
    NxObject *data[];                   //!< Array of pointers to items
} NxObjectArray;

/**
 * \brief Creates a new array of natrix objects.
 *
 * All items in the array are initialized to NULL.
 * May trigger garbage collection.
 * \param size capacity of the array
 * \return new array of natrix objects
 */
NxObjectArray *nx_object_array_create(int64_t size);

/**
 * \brief Creates a new array of natrix objects, copying the items from the source array.
 *
 * If the new size is greater than the source size, the extra items are initialized to NULL.
 * If the new size is smaller than the source size, the extra items are not copied.
 * May trigger garbage collection.
 * \param source array to copy items from, must not be NULL and must be rooted
 * \param new_size capacity of the new array
 * \return new array of natrix objects
 */
NxObjectArray *nx_object_array_copy(NxObjectArray *source, int64_t new_size);

#ifdef __cplusplus
}
#endif
#endif //NX_OBJECT_ARRAY_H
