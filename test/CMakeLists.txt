# Copyright (c) 2024, Ondrej Tethal
# All rights reserved.
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree.

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/f8d7d77c06936315286eb55f8de22cd23c188571.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(natrix_test EXCLUDE_FROM_ALL
        util/test_arena.cpp
        util/test_mem.cpp
        util/test_sb.cpp
)

target_link_libraries(
        natrix_test
        natrix_lib
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(natrix_test)

set_target_properties(gtest PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(gmock PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(gtest_main PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(gmock_main PROPERTIES EXCLUDE_FROM_ALL TRUE)