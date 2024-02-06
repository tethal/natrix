/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

/**
 * \file mem.h
 * \brief Memory allocation functions.
 *
 * Provides an abstraction layer for memory allocation and deallocation with consistent interface.
 */

#ifndef MEM_H
#define MEM_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

//! \brief The alignment of all memory allocations.
#define NX_ALIGNMENT            16

//! \brief Rounds x up to the nearest multiple of y, which must be a power of 2.
#define NX_ROUND_UP(x, y)       (((x) + (y) - 1) & ~((y) - 1))

//! \brief Aligns x up to the nearest multiple of NX_ALIGNMENT.
#define NX_ALIGN_UP(x)          NX_ROUND_UP(x, NX_ALIGNMENT)

//! \brief Checks if a pointer is aligned.
#define NX_IS_ALIGNED(ptr)      (((uintptr_t) (ptr) & (NX_ALIGNMENT - 1)) == 0)

/**
 * \brief Allocates a block of memory using the system allocator.
 *
 * The function always returns a valid pointer. If the allocation fails, the function panics.
 * The memory is aligned to NX_ALIGNMENT. The allocated block must be freed using `nx_free`.
 * \param size_in_bytes the size of the block to allocate, must be greater than 0
 * \return a pointer to the allocated block
 */
void *nx_alloc(size_t size_in_bytes);

/**
 * \brief Reallocates a block of memory using the system allocator.
 *
 * The function always returns a valid pointer. If the allocation fails, the function panics.
 * The memory is aligned to NX_ALIGNMENT. The allocated block must be freed using `nx_free`.
 * \param ptr a pointer to the block to reallocate
 * \param new_size_in_bytes the new size of the block, must be greater than 0
 * \return a pointer to the reallocated block
 */
void *nx_realloc(void *ptr, size_t new_size_in_bytes);

/**
 * \brief Frees a block of memory allocated using `nx_alloc`.
 *
 * The function does nothing if ptr is NULL.
 * \param ptr a pointer to the block to free
 */
void nx_free(void *ptr);

#ifdef __cplusplus
}
#endif
#endif //MEM_H
