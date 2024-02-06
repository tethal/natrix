/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

/**
 * \file arena.c
 * \brief Arena allocator implementation.
 */

#include "natrix/util/arena.h"
#include <assert.h>
#include "natrix/util/log.h"
#include "natrix/util/mem.h"

//! \brief Default size of a chunk
#define DEFAULT_CHUNK_SIZE  8192

//! \brief Size of the header of each chunk,
#define HEADER_SIZE         NX_ALIGN_UP(sizeof(ArenaChunk))

/**
 * \brief Allocates a new chunk of memory.
 * \param size_in_bytes size of the chunk in bytes
 * \return pointer to the allocated chunk
 */
static ArenaChunk *alloc_chunk(size_t size_in_bytes) {
    uint8_t *ptr = (uint8_t *) nx_alloc(HEADER_SIZE + size_in_bytes);
    ArenaChunk *chunk = (ArenaChunk *) ptr;
    chunk->start = ptr + HEADER_SIZE;
    assert(NX_IS_ALIGNED(chunk->start));
    chunk->end = chunk->start + size_in_bytes;
    chunk->ptr = chunk->start;
    chunk->next_chunk = NULL;
    return chunk;
}

void arena_init(Arena *arena) {
    arena->first_chunk = alloc_chunk(DEFAULT_CHUNK_SIZE);
    arena->current_chunk = arena->first_chunk;
    arena->alloc_count = 0;
}

void arena_free(Arena *arena) {
#if ENABLE_ARENA_STATS
    ArenaStats stats;
    arena_get_stats(arena, &stats);
    LOG_INFO("arena stats: %zu bytes in %zu chunks, %zu bytes in %zu objects", stats.chunk_size, stats.chunk_count, stats.alloc_size, stats.alloc_count);
#endif
    ArenaChunk *chunk = arena->first_chunk;
    while (chunk != NULL) {
        ArenaChunk *next = chunk->next_chunk;
        nx_free(chunk);
        chunk = next;
    }
}

void *arena_alloc(Arena *arena, size_t size) {
    size = NX_ALIGN_UP(size);
    arena->alloc_count++;
    if (size > DEFAULT_CHUNK_SIZE) {
        ArenaChunk *chunk = alloc_chunk(size);
        chunk->next_chunk = arena->first_chunk;
        arena->first_chunk = chunk;
        chunk->ptr += size;     // not really needed, but keeps the same invariant for all chunks
        return chunk->start;
    }
    if (arena->current_chunk->ptr + size > arena->current_chunk->end) {
        ArenaChunk *chunk = alloc_chunk(DEFAULT_CHUNK_SIZE);
        arena->current_chunk->next_chunk = chunk;
        arena->current_chunk = chunk;
    }
    void *ptr = arena->current_chunk->ptr;
    arena->current_chunk->ptr += size;
    return ptr;
}

void arena_get_stats(Arena *arena, ArenaStats *stats) {
    stats->alloc_count = arena->alloc_count;
    stats->chunk_count = 0;
    stats->alloc_size = 0;
    stats->chunk_size = 0;
    ArenaChunk *chunk = arena->first_chunk;
    while (chunk != NULL) {
        stats->chunk_count++;
        stats->alloc_size += chunk->ptr - chunk->start;
        stats->chunk_size += chunk->end - chunk->start;
        chunk = chunk->next_chunk;
    }
}
