# Copyright (c) 2024, Ondrej Tethal
# All rights reserved.
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree.

cmake_minimum_required(VERSION 3.22)
project(natrix C CXX)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(natrix_lib STATIC
        src/util/arena.c
        src/util/log.c
        src/util/mem.c
        src/util/panic.c
        src/util/sb.c
)
target_include_directories(natrix_lib PUBLIC include)

option(ENABLE_ARENA_STATS "Enable arena stats" ON)
if(ENABLE_ARENA_STATS)
    target_compile_definitions(natrix_lib PUBLIC ENABLE_ARENA_STATS=1)
endif(ENABLE_ARENA_STATS)

add_executable(natrix src/main.c)
target_link_libraries(natrix PRIVATE natrix_lib)

add_subdirectory(test)