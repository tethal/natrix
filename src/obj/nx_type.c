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

const NxType nx_type_type = {
        NX_TYPE_HEADER_INIT("type", gc_trace_nop),
};
