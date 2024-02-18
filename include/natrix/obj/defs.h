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

#ifdef __cplusplus
}
#endif
#endif //DEFS_H