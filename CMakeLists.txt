# Copyright (c) 2024, Ondrej Tethal
# All rights reserved.
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree.

cmake_minimum_required(VERSION 3.22)
project(natrix C CXX)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(natrix_lib STATIC
        src/util/panic.c
)
target_include_directories(natrix_lib PUBLIC include)

add_executable(natrix src/main.c)
target_link_libraries(natrix PRIVATE natrix_lib)

add_subdirectory(test)
