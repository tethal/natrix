/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
#include "natrix/util/mem.h"

TEST(MemTest, AlignUp) {
    EXPECT_EQ(NX_ALIGN_UP(0), 0);
    EXPECT_EQ(NX_ALIGN_UP(1), 16);
    EXPECT_EQ(NX_ALIGN_UP(15), 16);
    EXPECT_EQ(NX_ALIGN_UP(16), 16);
    EXPECT_EQ(NX_ALIGN_UP(17), 32);
    EXPECT_EQ(NX_ALIGN_UP(0x1234567890), 0x1234567890);
    EXPECT_EQ(NX_ALIGN_UP(0x1234567891), 0x12345678A0);
}

TEST(MemTest, IsAligned) {
    EXPECT_TRUE(NX_IS_ALIGNED(0));
    EXPECT_FALSE(NX_IS_ALIGNED(1));
    EXPECT_FALSE(NX_IS_ALIGNED(15));
    EXPECT_TRUE(NX_IS_ALIGNED(16));
    EXPECT_TRUE(NX_IS_ALIGNED(0x1234567890));
    EXPECT_FALSE(NX_IS_ALIGNED(0x1234567891));
}
