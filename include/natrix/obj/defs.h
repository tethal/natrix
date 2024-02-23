/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

/**
 * \file defs.h
 * \brief Definitions and utilities for implementing natrix objects.
 */

#ifndef DEFS_H
#define DEFS_H
#ifdef __cplusplus
extern "C" {
#endif

#include "natrix/obj/nx_object.h"
#include "natrix/obj/nx_type.h"

/**
 * \brief Allocate memory for an object of given size and type.
 * \param size size of the object in bytes
 * \param type type of the object
 * \return pointer to the allocated memory (declared as `void *` to avoid casting)
 */
static inline void *nxo_alloc(size_t size, const NxType *type) {
    NxObject *obj = (NxObject *) gc_alloc(size, type->gc_trace_fn);
    obj->type = type;
    return obj;
}

/**
 * \brief Convenience function for rooting a natrix object.
 * \param obj the object to root
 */
static inline void nxo_root(NxObject *obj) {
    gc_root(&obj->gc_header);
}

/**
 * \brief Convenience function for unrooting a natrix object.
 * \param obj the object to unroot
 */
static inline void nxo_unroot(NxObject *obj) {
    gc_unroot(&obj->gc_header);
}

/**
 * \brief Convenience function for checking whether a natrix object is a valid index to a sequence of given length.
 * \param index the index to check, must be an instance of `int`
 * \param len the length of the sequence, must be non-negative
 * \return the index as a number between 0 and len - 1
 */
int64_t nxo_check_index(NxObject *index, int64_t len);

/**
 * \brief Converts an object to a boolean.
 * \param obj the object to convert
 * \return the object as a natrix `bool` value
 */
NxObject *nxo_as_bool(NxObject *obj);

/**
 * \brief Subscript operator for natrix objects.
 * \param obj the object to index
 * \param index the index to use
 * \return the element at the given index
 */
NxObject *nxo_get_element(NxObject *obj, NxObject *index);

/**
 * \brief Subscript assignment operator for natrix objects.
 * \param obj the object to index
 * \param index the index to use
 * \param value the value to assign
 */
void nxo_set_element(NxObject *obj, NxObject *index, NxObject *value);

#ifdef __cplusplus
}
#endif
#endif //DEFS_H
