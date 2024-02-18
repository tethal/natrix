/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

/**
 * \file nx_object.h
 * \brief Defines the header common to all natrix objects.
 *
 * The header contains a pointer to the type of the object and GC-related fields.
 * The macro `NX_OBJECT_HEADER` should be used to include the header in a concrete object structure:
 * \code{.c}
 * typedef struct {
 *    NX_OBJECT_HEADER
 *    // ... other fields specific to the object
 * } NxSomeObject;
 * \endcode
 */

#ifndef NX_OBJECT_H
#define NX_OBJECT_H
#ifdef __cplusplus
extern "C" {
#endif

#include "natrix/util/gc.h"

/**
 * \brief Header common to all natrix objects.
 */
typedef struct {
    GcHeader gc_header;                     //!< GC header
    const struct NxType *type;              //!< Type of the object
} NxObject;

#ifdef __cplusplus
}
#endif
#endif //NX_OBJECT_H
