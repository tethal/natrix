/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

/**
 * \file nx_bool.c
 * \brief Implementation of the `bool` type.
 */

#include "natrix/obj/nx_bool.h"

//! The `false` object.
static NxBool false_obj = {
        .header = {
                .gc_header = {
                        .next = NULL,
                        .trace_fn = gc_trace_nop,
                },
                .type = &nx_type_bool,
        },
        .value = false,
};

//! The `true` object.
static NxBool true_obj = {
        .header = {
                .gc_header = {
                        .next = NULL,
                        .trace_fn = gc_trace_nop,
                },
                .type = &nx_type_bool,
        },
        .value = true,
};

NxObject *nx_false = (NxObject *) &false_obj;
NxObject *nx_true= (NxObject *) &true_obj;

//! Implementation of the `as_bool` method for the `bool` type.
static NxObject *nx_bool_as_bool(NxObject *self) {
    assert(nx_bool_is_instance(self));
    return self;
}

const NxType nx_type_bool = {
        NX_TYPE_HEADER_INIT("bool", gc_trace_nop),
        .as_bool_fn = nx_bool_as_bool,
};
