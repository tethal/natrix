/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>
#include "natrix/util/arena.h"

TEST(ArenaTest, SingleChunk) {
    Arena arena;
    arena_init(&arena);
    EXPECT_EQ(arena.current_chunk, arena.first_chunk);
    EXPECT_EQ(arena.current_chunk->next_chunk, nullptr);
    EXPECT_EQ(arena.current_chunk->ptr, arena.current_chunk->start);
    EXPECT_EQ(arena.current_chunk->end, arena.current_chunk->start + 8192);
    void* ptr1 = arena_alloc(&arena, 100);  // rounded up to 112
    void* ptr2 = arena_alloc(&arena, 200);  // rounded up to 208
    EXPECT_EQ(ptr1, arena.first_chunk->start);
    EXPECT_EQ(ptr2, arena.first_chunk->start + 112);
    EXPECT_EQ(arena.current_chunk->ptr, arena.current_chunk->start + 112 + 208);
    EXPECT_EQ(arena.current_chunk->end, arena.current_chunk->start + 8192);
    ArenaStats stats;
    arena_get_stats(&arena, &stats);
    EXPECT_EQ(stats.chunk_count, 1);
    EXPECT_EQ(stats.chunk_size, 8192);
    EXPECT_EQ(stats.alloc_count, 2);
    EXPECT_EQ(stats.alloc_size, 112 + 208);
    arena_free(&arena);
}

TEST(ArenaTest, TwoChunks) {
    Arena arena;
    arena_init(&arena);
    EXPECT_EQ(arena.current_chunk, arena.first_chunk);
    EXPECT_EQ(arena.current_chunk->ptr, arena.current_chunk->start);
    EXPECT_EQ(arena.current_chunk->end, arena.current_chunk->start + 8192);
    void* ptr1 = arena_alloc(&arena, 100);   // rounded up to 112
    void* ptr2 = arena_alloc(&arena, 8100);  // rounded up to 8112
    EXPECT_EQ(ptr1, arena.first_chunk->start);
    EXPECT_EQ(ptr2, arena.current_chunk->start);
    EXPECT_EQ(arena.first_chunk->next_chunk, arena.current_chunk);
    EXPECT_EQ(arena.first_chunk->ptr, arena.first_chunk->start + 112);
    EXPECT_EQ(arena.first_chunk->end, arena.first_chunk->start + 8192);
    EXPECT_EQ(arena.current_chunk->next_chunk, nullptr);
    EXPECT_EQ(arena.current_chunk->ptr, arena.current_chunk->start + 8112);
    EXPECT_EQ(arena.current_chunk->end, arena.current_chunk->start + 8192);
    ArenaStats stats;
    arena_get_stats(&arena, &stats);
    EXPECT_EQ(stats.chunk_count, 2);
    EXPECT_EQ(stats.chunk_size, 2 * 8192);
    EXPECT_EQ(stats.alloc_count, 2);
    EXPECT_EQ(stats.alloc_size, 112 + 8112);
    arena_free(&arena);
}

TEST(ArenaTest, SpecialChunk) {
    Arena arena;
    arena_init(&arena);
    EXPECT_EQ(arena.current_chunk, arena.first_chunk);
    EXPECT_EQ(arena.current_chunk->ptr, arena.current_chunk->start);
    EXPECT_EQ(arena.current_chunk->end, arena.current_chunk->start + 8192);
    void* ptr1 = arena_alloc(&arena, 100);   // rounded up to 112
    void* ptr2 = arena_alloc(&arena, 8193);  // rounded up to 8208
    EXPECT_EQ(ptr1, arena.current_chunk->start);
    EXPECT_EQ(ptr2, arena.first_chunk->start);
    EXPECT_EQ(arena.first_chunk->next_chunk, arena.current_chunk);
    EXPECT_EQ(arena.first_chunk->ptr, arena.first_chunk->start + 8208);
    EXPECT_EQ(arena.first_chunk->end, arena.first_chunk->start + 8208);
    EXPECT_EQ(arena.current_chunk->next_chunk, nullptr);
    EXPECT_EQ(arena.current_chunk->ptr, arena.current_chunk->start + 112);
    EXPECT_EQ(arena.current_chunk->end, arena.current_chunk->start + 8192);
    ArenaStats stats;
    arena_get_stats(&arena, &stats);
    EXPECT_EQ(stats.chunk_count, 2);
    EXPECT_EQ(stats.chunk_size, 8208 + 8192);
    EXPECT_EQ(stats.alloc_count, 2);
    EXPECT_EQ(stats.alloc_size, 112 + 8208);
    arena_free(&arena);
}
