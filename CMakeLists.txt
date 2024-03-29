# Copyright (c) 2024, Ondrej Tethal
# All rights reserved.
# This source code is licensed under the BSD-style license found in the
# LICENSE file in the root directory of this source tree.

cmake_minimum_required(VERSION 3.22)
project(natrix C CXX)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(natrix_lib STATIC
        src/obj/defs.c
        src/obj/nx_bool.c
        src/obj/nx_int.c
        src/obj/nx_list.c
        src/obj/nx_object_array.c
        src/obj/nx_str.c
        src/obj/nx_type.c
        src/parser/ast.c
        src/parser/diag.c
        src/parser/lexer.c
        src/parser/parser.c
        src/parser/source.c
        src/parser/token.c
        src/util/arena.c
        src/util/gc.c
        src/util/log.c
        src/util/mem.c
        src/util/panic.c
        src/util/sb.c
)
target_include_directories(natrix_lib PUBLIC include)

option(ENABLE_ARENA_STATS "Enable arena stats" OFF)
if(ENABLE_ARENA_STATS)
    target_compile_definitions(natrix_lib PUBLIC ENABLE_ARENA_STATS=1)
endif(ENABLE_ARENA_STATS)

option(ENABLE_GC_STATS "Enable GC stats" OFF)
if(ENABLE_GC_STATS)
    target_compile_definitions(natrix_lib PUBLIC ENABLE_GC_STATS=1)
endif(ENABLE_GC_STATS)

option(ENABLE_TOKEN_LOGGING "Enable logging of tokens produced by the lexer" OFF)
if(ENABLE_TOKEN_LOGGING)
    target_compile_definitions(natrix_lib PUBLIC ENABLE_TOKEN_LOGGING=1)
endif(ENABLE_TOKEN_LOGGING)

option(ENABLE_AST_LOGGING "Enable logging of the AST after parsing" OFF)
if(ENABLE_AST_LOGGING)
    target_compile_definitions(natrix_lib PUBLIC ENABLE_AST_LOGGING=1)
endif(ENABLE_AST_LOGGING)

add_executable(natrix src/main.c)
target_link_libraries(natrix PRIVATE natrix_lib)

add_subdirectory(test)
