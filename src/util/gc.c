/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

/**
 * \file gc.c
 * \brief Implementation of garbage collector functions.
 */

#include "natrix/util/gc.h"
#include <assert.h>
#include "natrix/util/log.h"
#include "natrix/util/mem.h"
#include "natrix/util/panic.h"

#include "natrix/util/gc_internals.h"

/**
 * \brief Internal state of the garbage collector.
 */
static GcState gc = {
    .head = NULL,
    .objects_count = 0,
    .threshold = 100,
    .roots_count = 0,
    .roots = {},
};

GcHeader *gc_alloc(size_t size_in_bytes, GcTraceFn trace_fn) {
    assert(size_in_bytes > sizeof(GcHeader));
    if (gc.objects_count >= gc.threshold) {
        gc_collect();
    }
    GcHeader *ptr = nx_alloc_no_panic(size_in_bytes);
    if (ptr == NULL) {
        gc_collect();
        ptr = nx_alloc(size_in_bytes);  // try again but panic this time
    }
    ptr->next = gc.head;
    ptr->trace_fn = trace_fn ? trace_fn : gc_trace_nop;
    gc.head = ptr;
    gc.objects_count++;
    return ptr;
}

void gc_root(GcHeader *root) {
    if (gc.roots_count >= MAX_ROOTS) {
        PANIC("too many GC roots");
    }
    gc.roots[gc.roots_count++] = root;
}

void gc_unroot(GcHeader *root) {
    assert(gc.roots_count > 0);
    assert(gc.roots[gc.roots_count - 1] == root);
    gc.roots_count--;
}

void gc_visit(GcHeader *ptr) {
    if (ptr == NULL) {
        return;
    }
    if (!IS_MARKED(ptr)) {
        MARK(ptr);
        ptr->trace_fn(ptr);
    }
}

void gc_collect() {
    // Mark phase
    for (size_t i = 0; i < gc.roots_count; i++) {
        gc_visit(gc.roots[i]);
    }

    // Sweep phase
    size_t count = 0;
    GcHeader **p = &gc.head;
    while (*p != NULL) {
        GcHeader *header = *p;
        if (IS_MARKED(header)) {
            UNMARK(header);
            assert((((uintptr_t) header->next) & 7) == 0);
            p = &header->next;
        } else {
            assert((((uintptr_t) header->next) & 7) == 0);
            *p = header->next;
            nx_free(header);
            count++;
        }
    }
    gc.objects_count -= count;

    // Double the threshold if the number of surviving objects is still above 87.5% of the threshold
    if (gc.objects_count >= gc.threshold - (gc.threshold / 8)) {
        if (gc.threshold & (1L << 63)) {
            PANIC("too many objects");
        }
        gc.threshold *= 2;
    }

#if ENABLE_GC_STATS
    LOG_INFO("GC done: freed %zu objects, %zu remaining, threshold %zu", count, gc.objects_count, gc.threshold);
#endif
}

GcState *gc_get_internal_state() {
    return &gc;
}
