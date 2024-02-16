/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

/**
 * \file gc.h
 * \brief Memory management with garbage collection.
 *
 * The garbage collector is a simple mark-and-sweep algorithm. All objects are allocated using malloc().
 * The garbage collector keeps track of all allocated objects using a singly linked list.
 * It also keeps a stack of roots, which are pointers to objects that are known to be reachable.
 * When malloc() fails, or the number of allocated objects exceeds a certain threshold, the garbage collector is invoked.
 * In the mark phase, the garbage collector performs a depth-first search starting from the roots, marking all reachable objects.
 * In the sweep phase, the garbage collector iterates over all allocated objects, freeing (using free()) those that are not marked.
 * After garbage collection, if the number of surviving objects is still above or near the threshold, the threshold is increased.
 * In order to be able to find all allocated objects, the garbage collector needs to know about all pointers in all objects.
 * Each object has a pointer to a function which is called by the garbage collector during the mark phase
 * to find all pointers in the object. The function must call gc_visit() for each pointer in the object.
 * Every allocated object needs to be reachable from a root, otherwise it will be collected. This means that
 * after allocating an object, the pointer to it needs to be either written to another reachable object or added to the
 * stack of roots before any garbage collection can occur, i.e. before the next allocation.
 * The stack of roots is implemented as a fixed-size array of pointers. Rooting and unrooting must be done in a LIFO manner.
 * The garbage collector is not thread-safe.
 */

#ifndef GC_H
#define GC_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

/**
 * \brief Type of the function for tracing pointers in an object.
 *
 * Every object has a pointer to a function of this type, assigned upon creation of the object.
 * The function is called by the garbage collector during the mark phase.
 * The function must call gc_visit() for each pointer in the object.
 */
typedef void (*GcTraceFn)(void *ptr);

/**
 * \brief Header of all GC-allocated objects, it must be the first field of every object.
 */
typedef struct GcHeader {
    union {
        struct GcHeader *next;      //!< Pointer to the next object in the linked list
        uintptr_t mark;             //!< The least significant bit is used as the mark bit
    };
    GcTraceFn trace_fn;             //!< Function to trace pointers in the object, never NULL
} GcHeader;

/**
 * \brief Allocates memory for an object of the given size.
 *
 * Never returns NULL, panics if the necessary memory cannot be allocated even after garbage collection.
 * \param size_in_bytes size of the object in bytes, including the header
 * \param trace_fn function to trace pointers in the object, can be NULL if the object does not contain any pointers
 * \return pointer to the allocated object
 */
GcHeader *gc_alloc(size_t size_in_bytes, GcTraceFn trace_fn);

/**
 * \brief Roots an object, preventing it from being collected by the garbage collector.
 * \param root pointer to the object to root
 */
void gc_root(GcHeader *root);

/**
 * \brief Unroots an object, allowing it to be collected by the garbage collector.
 * \param root pointer to the object to unroot
 *
 * The object must be the last object rooted, it is passed as a parameter only to check that this is the case.
 */
void gc_unroot(GcHeader *root);

/**
 * \brief Marks an object and all objects reachable from it.
 * \param ptr pointer to the object to visit, can be NULL
 */
void gc_visit(GcHeader *ptr);

/**
 * \brief Runs the garbage collector explicitly.
 *
 * There is usually no need to call this function manually, as the garbage collector is invoked automatically.
 * However, it can be useful to call this function manually in order to free memory as soon as possible after a large
 * number of objects has become unreachable. Also, it is used in the unit tests.
 */
void gc_collect();


/**
 * \brief Tracing function for objects that do not contain any pointers. Does nothing.
 * \param ptr pointer to the object
 */
static inline void gc_trace_nop(void *ptr) {
    (void) ptr;
}

#ifdef __cplusplus
}
#endif
#endif //GC_H
