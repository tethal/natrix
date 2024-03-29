/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

/**
 * \file nx_str.c
 * \brief Implementation of the `str` type.
 */

#include "natrix/obj/nx_str.h"
#include <assert.h>
#include <string.h>
#include "natrix/obj/nx_bool.h"
#include "natrix/obj/nx_int.h"
#include "natrix/util/panic.h"

/**
 * \brief Allocates but does not initialize a new `str` object.
 * \param length the number of bytes (excluding the null terminator)
 * \return the newly allocated string object
 */
static NxStr *nx_str_alloc(int64_t length) {
    assert(length >= 0);
    NxStr *str = nxo_alloc(sizeof(NxStr) + length + 1, &nx_type_str);
    *((int64_t *) &str->length) = length;
    return str;
}

NxObject *nx_str_create(const char *data, int64_t length) {
    assert(data != NULL);
    assert(length >= 0);
    NxStr *str = nx_str_alloc(length);
    memcpy((char *) &str->data, data, length);
    ((char *) &str->data)[length] = '\0';
    return &str->header;
}

NxObject *nx_str_concat(NxObject *left, NxObject *right) {
    assert(nx_str_is_instance(left));
    assert(nx_str_is_instance(right));
    size_t len1 = nx_str_get_length(left);
    size_t len2 = nx_str_get_length(right);
    NxStr *result = nx_str_alloc(len1 + len2);
    memcpy((char *) &result->data, nx_str_get_cstr(left), len1);
    memcpy((char *) &result->data + len1, nx_str_get_cstr(right), len2);
    ((char *) &result->data)[len1 + len2] = '\0';
    return &result->header;
}

//! Implementation of the `as_bool` method for the `str` type.
static NxObject *nx_str_as_bool(NxObject *self) {
    assert(nx_str_is_instance(self));
    return nx_bool_wrap(nx_str_get_length(self) > 0);
}

//! Implementation of the `get_element` method for the `str` type.
static NxObject *nx_str_get_element(NxObject *self, NxObject *index) {
    assert(nx_str_is_instance(self));
    int64_t i = nxo_check_index(index, nx_str_get_length(self));
    return nx_str_create(nx_str_get_cstr(self) + i, 1);
}

const NxType nx_type_str = {
        NX_TYPE_HEADER_INIT("str", gc_trace_nop),
        .as_bool_fn = nx_str_as_bool,
        .get_element_fn = nx_str_get_element,
        .set_element_fn = NULL,
};
