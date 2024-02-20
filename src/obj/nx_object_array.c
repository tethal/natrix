/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

/**
 * \file nx_object_array.c
 * \brief Implementation of NxObjectArray.
 */

#include "natrix/obj/nx_object_array.h"
#include <assert.h>
#include <string.h>

/**
 * \brief GC trace function for NxObjectArray.
 * \param ptr pointer to the array
 */
static void nx_object_array_gc_trace(void *ptr) {
    NxObjectArray *array = (NxObjectArray *) ptr;
    for (int64_t i = 0; i < array->size; i++) {
        gc_visit(&array->data[i]->gc_header);
    }
}

/**
 * \brief Allocates but does not initialize an array of objects.
 *
 * May trigger garbage collection.
 * \param size size of the array
 * \return pointer to the allocated array
 */
static NxObjectArray *nx_object_array_alloc(int64_t size) {
    assert(size >= 0);
    NxObjectArray *array = (NxObjectArray *) gc_alloc(sizeof(NxObjectArray) + size * sizeof(NxObject *), nx_object_array_gc_trace);
    *((int64_t *) &array->size) = size;
    return array;
}

NxObjectArray *nx_object_array_create(int64_t size) {
    assert(size >= 0);
    NxObjectArray *array = nx_object_array_alloc(size);
    memset(array->data, 0, size * sizeof(NxObject *));
    return array;
}

NxObjectArray *nx_object_array_copy(NxObjectArray *source, int64_t new_size) {
    assert(new_size >= 0);
    NxObjectArray *array = nx_object_array_alloc(new_size);
    int64_t copy_size = source->size < new_size ? source->size : new_size;
    memcpy(array->data, source->data, copy_size * sizeof(NxObject *));
    memset(array->data + copy_size, 0, (new_size - copy_size) * sizeof(NxObject *));
    return array;
}
