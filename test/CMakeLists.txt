# Copyright (c) 2024, Ondrej Tethal
# All rights reserved.
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree.

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/f8d7d77c06936315286eb55f8de22cd23c188571.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_custom_target(check COMMAND $<TARGET_FILE:natrix_test> DEPENDS natrix_test WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/test-data)

add_executable(natrix_test EXCLUDE_FROM_ALL
        obj/test_nx_bool.cpp
        obj/test_nx_int.cpp
        obj/test_nx_list.cpp
        obj/test_nx_str.cpp
        obj/test_nx_type.cpp
        parser/test_ast.cpp
        parser/test_lexer.cpp
        parser/test_parser.cpp
        parser/test_source.cpp
        parser/test_token.cpp
        util/test_arena.cpp
        util/test_gc.cpp
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
