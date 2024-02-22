/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

/**
 * \file nx_type.c
 * \brief Definition of the `type` type.
 */

#include "natrix/obj/nx_type.h"
#include "natrix/obj/nx_bool.h"

//! Implementation of the `as_bool` method for the `type` type.
static NxObject *nx_type_as_bool(NxObject *self) {
    (void) self;
    return nx_true;
}

const NxType nx_type_type = {
        NX_TYPE_HEADER_INIT("type", gc_trace_nop),
        .as_bool_fn = nx_type_as_bool,
};
