/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

/**
 * \file gc_internals.h
 * \brief Internal structures and functions of the garbage collector.
 *
 * This file is not part of the public API and should not be included in client code.
 * It is used by the garbage collector and the unit tests.
 */

#ifndef GC_INTERNALS_H
#define GC_INTERNALS_H
#ifdef __cplusplus
extern "C" {
#endif

//! Determines whether the object is marked as reachable during the garbage collection.
#define IS_MARKED(p)    ((p)->mark & 1)
//! Marks the object as reachable during the mark phase of garbage collection.
#define MARK(p)         ((p)->mark |= 1)
//! Unmarks the object during the sweep phase of garbage collection.
#define UNMARK(p)       ((p)->mark &= ~1)
//! The maximum number of roots.
#define MAX_ROOTS 64

/**
 * \brief Internal state of the garbage collector, exposed for testing purposes.
 */
typedef struct {
    GcHeader *head;                 //!< Head of the linked list of all allocated objects
    size_t objects_count;           //!< Number of allocated objects
    size_t threshold;               //!< Threshold of `objects_count` after which the garbage collection is triggered
    size_t roots_count;             //!< Number of roots
    GcHeader *roots[MAX_ROOTS];     //!< Stack of roots
} GcState;

/**
 * \brief Provides access to the internal state of the garbage collector for testing purposes.
 */
GcState *gc_get_internal_state();

#ifdef __cplusplus
}
#endif
#endif //GC_INTERNALS_H
