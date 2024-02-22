/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

/**
 * \file nx_list.c
 * \brief Implementation of the `list` type.
 */

#include "natrix/obj/nx_list.h"
#include <assert.h>
#include "natrix/obj/nx_bool.h"

/**
 * \brief Reports all references contained in the list to the garbage collector.
 * \param obj the list to trace
 */
static void nx_list_gc_trace(void *obj) {
    assert(nx_list_is_instance(obj));
    gc_visit(&((NxList *) obj)->items->gc_header);
}

NxObject *nx_list_create(int64_t initial_capacity) {
    assert(initial_capacity > 0);
    NxObjectArray *items = nx_object_array_create(initial_capacity);
    gc_root(&items->gc_header);
    NxList *list = nxo_alloc(sizeof(NxList), &nx_type_list);
    list->length = 0;
    list->items = items;
    gc_unroot(&items->gc_header);
    return &list->header;
}

void nx_list_append(NxObject *list, NxObject *item) {
    assert(nx_list_is_instance(list));
    NxList *l = (NxList *) list;
    if (l->length == l->items->size) {
        // todo generic ensure_capacity, check overflow
        int64_t new_capacity = l->items->size * 2 + 1;
        l->items = nx_object_array_copy(l->items, new_capacity);
    }
    l->items->data[l->length++] = item;
}

//! Implementation of the `as_bool` method for the `list` type.
static NxObject *nx_list_as_bool(NxObject *self) {
    assert(nx_list_is_instance(self));
    return nx_bool_wrap(((NxList *) self)->length > 0);
}

const NxType nx_type_list = {
        NX_TYPE_HEADER_INIT("list", nx_list_gc_trace),
        .as_bool_fn = nx_list_as_bool,
};
