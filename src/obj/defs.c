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
#include "natrix/util/panic.h"

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
