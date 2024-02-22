/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
#include "natrix/obj/nx_bool.h"

TEST(NxTypeTest, AsBool) {
    EXPECT_EQ(nxo_as_bool((NxObject *) &nx_type_bool), nx_true);
}
