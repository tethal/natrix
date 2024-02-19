/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

/**
 * \file nx_int.c
 * \brief Implementation of the `int` type.
 */

#include "natrix/obj/nx_int.h"

//! The minimum cached integer value.
#define CACHE_MIN (-1)
//! The maximum cached integer value.
#define CACHE_MAX 255
//! The number of cached integers.
#define CACHE_SIZE (CACHE_MAX - CACHE_MIN + 1)
//! Check if the integer is cached.
#define IS_CACHED(x) ((x) >= CACHE_MIN && (x) <= CACHE_MAX)
//! Get the index of the integer in the cache, provided it is cached (use IS_CACHED to check).
#define CACHE_INDEX(x) ((x) - CACHE_MIN)
//! The cached integers, initialized in `init_int_cache`.
static NxInt cached_ints[CACHE_SIZE];

//! Initializes the cached integers. Executed before `main`.
__attribute__((constructor)) static void init_int_cache() {
    for (int64_t i = CACHE_MIN; i <= CACHE_MAX; i++) {
        cached_ints[CACHE_INDEX(i)].header.gc_header.next = NULL;
        cached_ints[CACHE_INDEX(i)].header.gc_header.trace_fn = gc_trace_nop;
        cached_ints[CACHE_INDEX(i)].header.type = &nx_type_int;
        *((int64_t *) &cached_ints[CACHE_INDEX(i)].value) = i;
    }
}

NxObject *nx_int_create(int64_t value) {
    if (IS_CACHED(value)) {
        return (NxObject *) &cached_ints[CACHE_INDEX(value)];
    }
    NxInt *obj = nxo_alloc(sizeof(NxInt), &nx_type_int);
    *((int64_t *) &obj->value) = value;
    return &obj->header;
}

const NxType nx_type_int = {
        NX_TYPE_HEADER_INIT("int", gc_trace_nop),
};
