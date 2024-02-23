/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

/**
 * \file defs.c
 * \brief Implementation of generic object manipulation functions.
 */

#include "natrix/obj/defs.h"
#include <assert.h>
#include "natrix/obj/nx_bool.h"
#include "natrix/obj/nx_int.h"
#include "natrix/util/panic.h"

int64_t nxo_check_index(NxObject *index, int64_t len) {
    assert(index != NULL);
    assert(len >= 0);
    if (!nx_int_is_instance(index)) {
        PANIC("Index must be an integer");
    }
    int64_t i = nx_int_get_value(index);
    if (i < 0) {
        i += len;
    }
    if (i < 0 || i >= len) {
        PANIC("Index out of range");
    }
    return i;
}

NxObject *nxo_as_bool(NxObject *obj) {
    assert(obj != NULL);
    if (obj->type->as_bool_fn == NULL) {
        PANIC("cannot convert '%s' object of to bool", obj->type->name);
    }
    NxObject *result = obj->type->as_bool_fn(obj);
    if (result == NULL || !nx_bool_is_instance(result)) {
        PANIC("as_bool_fn returned non-bool object");
    }
    return result;
}

NxObject *nxo_get_element(NxObject *obj, NxObject *index) {
    assert(obj != NULL);
    assert(index != NULL);
    if (obj->type->get_element_fn == NULL) {
        PANIC("'%s' object is not subscriptable", obj->type->name);
    }
    NxObject *result = obj->type->get_element_fn(obj, index);
    if (result == NULL) {
        PANIC("get_element_fn returned NULL");
    }
    return result;
}

void nxo_set_element(NxObject *obj, NxObject *index, NxObject *value) {
    assert(obj != NULL);
    assert(index != NULL);
    assert(value != NULL);
    if (obj->type->set_element_fn == NULL) {
        PANIC("'%s' object does not support item assignment", obj->type->name);
    }
    obj->type->set_element_fn(obj, index, value);
}
