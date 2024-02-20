/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

/**
 * \file nx_list.h
 * \brief Representation and operations of natrix `list` objects.
 */

#ifndef NX_LIST_H
#define NX_LIST_H
#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>
#include <stdbool.h>
#include "natrix/obj/defs.h"
#include "natrix/obj/nx_object_array.h"

/**
 * \brief Layout of `list` instances.
 */
typedef struct {
    NxObject header;            //!< Header common to all natrix objects
    int64_t length;             //!< Number of items in the list, always less than or equal to items->size
    NxObjectArray *items;       //!< Array of items, never NULL but may be of zero size
} NxList;

/**
 * \brief Type of all `list` instances.
 */
extern const NxType nx_type_list;

/**
 * \brief Creates a new empty `list` object with the given initial capacity.
 *
 * May trigger garbage collection.
 * \param initial_capacity initial capacity of the list, must be greater than zero
 * \return the new list object
 */
NxObject *nx_list_create(int64_t initial_capacity);

/**
 * \brief Determines whether the object is an instance of the `list` type.
 * \param object the object to check
 * \return true if the object is an instance of the `list` type, false otherwise
 */
static inline bool nx_list_is_instance(NxObject *object) {
    return object->type == &nx_type_list;
}

/**
 * \brief Returns the number of items in the list.
 * \param list the list to get the size of
 * \return the number of items in the list
 */
static inline int64_t nx_list_get_length(NxObject *list) {
    assert(nx_list_is_instance(list));
    return ((NxList *) list)->length;
}

/**
 * \brief Appends the given item to the list.
 *
 * May trigger garbage collection.
 * \param list the list to append to, must be a non-NULL, rooted instance of the `list` type
 * \param item the item to append, must be rooted or NULL
 */
void nx_list_append(NxObject *list, NxObject *item);

#ifdef __cplusplus
}
#endif
#endif //NX_LIST_H
