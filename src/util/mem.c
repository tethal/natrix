/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

/**
 * \file mem.c
 * \brief Memory utilities implementation.
 */

#include "natrix/util/mem.h"
#include <assert.h>
#include <malloc.h>
#include "natrix/util/panic.h"

void *nx_alloc(size_t size_in_bytes) {
    void *ptr = nx_alloc_no_panic(size_in_bytes);
    if (ptr == NULL) {
        PANIC("Out of memory");
    }
    return ptr;
}

void *nx_alloc_no_panic(size_t size_in_bytes) {
    assert(size_in_bytes > 0);      // avoid implementation-defined behavior
    void *ptr = malloc(size_in_bytes);
    assert(NX_IS_ALIGNED(ptr));
    return ptr;
}

void *nx_realloc(void *ptr, size_t new_size_in_bytes) {
    assert(new_size_in_bytes > 0);  // avoid implementation-defined behavior
    void *new_ptr = realloc(ptr, new_size_in_bytes);
    if (new_ptr == NULL) {
        PANIC("Out of memory");
    }
    assert(NX_IS_ALIGNED(new_ptr));
    return new_ptr;
}

void nx_free(void *ptr) {
    free(ptr);
}
