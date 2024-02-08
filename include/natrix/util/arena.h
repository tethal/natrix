/*
 * Copyright (c) 2024, Ondrej Tethal
 * All rights reserved.
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

/**
 * \file arena.h
 * \brief Arena allocator.
 *
 * The arena allocator is a simple memory allocator that allocates objects from larger chunks of memory.
 * The allocator is designed to be used for allocating many small objects that are deallocated all at once.
 * It is used for allocating the AST nodes since they are never deallocated individually.
 *
 * The allocator is implemented as a singly linked list of chunks. When the current chunk is full, a new chunk
 * is allocated and added to the end of the list. When the allocator is destroyed, all chunks are deallocated
 * at once. Chunks are 8192 bytes long. If an object is larger than 8192 bytes, a special chunk (exactly sized
 * for the object) is allocated and put to the front of the list.
 */

#ifndef ARENA_H
#define ARENA_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

/**
 * \brief A chunk of memory from which objects are allocated.
 *
 * Internal structure of the arena allocator, not to be used directly.
 */
typedef struct ArenaChunk ArenaChunk;
struct ArenaChunk {
    uint8_t *start;             //!< Start of the chunk
    uint8_t *end;               //!< End of the chunk
    uint8_t *ptr;               //!< Current position in the chunk
    ArenaChunk *next_chunk;     //!< Next chunk, or NULL if this is the last chunk
};

/**
 * \brief The arena allocator.
 *
 * Internal structure of the arena allocator, not to be used directly.
 */
typedef struct {
    ArenaChunk *first_chunk;    //!< First chunk in the list
    ArenaChunk *current_chunk;  //!< Current chunk where the next allocation will be made
    size_t alloc_count;         //!< Total number of allocations made from this arena
} Arena;

/**
 * \brief Statistics about the arena allocator, filled by `arena_get_stats`.
 */
typedef struct {
    size_t alloc_count;         //!< Total number of allocations made from the arena
    size_t chunk_count;         //!< Total number of chunks allocated
    size_t alloc_size;          //!< Total size of all allocations in bytes, including padding
    size_t chunk_size;          //!< Total size of all chunks allocated in bytes, excluding headers
} ArenaStats;

/**
 * \brief Initializes the arena allocator.
 * \return the initialized arena
 */
Arena arena_init();

/**
 * \brief Destroys the arena allocator and deallocates all memory.
 *
 * All pointers returned by `arena_alloc` become invalid after this function is called.
 * \param arena the arena to destroy, must be initialized by `arena_create`
 */
void arena_free(Arena *arena);

/**
 * \brief Allocates a block of memory from the arena.
 *
 * Never returns NULL. If the allocation fails, the program panics. The memory is not initialized.
 * The returned pointer is aligned to 16 bytes. The pointer is valid until the arena is destroyed.
 * \param arena the arena from which to allocate, must be initialized by `arena_create`
 * \param size size of the block to allocate
 * \return pointer to the allocated block
 */
void *arena_alloc(Arena *arena, size_t size);

/**
 * \brief Fills the `stats` structure with statistics about the arena.
 * \param arena the arena to get statistics for
 * \param stats the structure to fill with statistics
 */
void arena_get_stats(Arena *arena, ArenaStats *stats);

#ifdef __cplusplus
}
#endif
#endif //ARENA_H
