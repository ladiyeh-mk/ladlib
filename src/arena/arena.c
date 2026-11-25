#include "../../include/ladlib/arena.h"
#include <stdlib.h>

#define DEFAULT_ARENA_CAPACITY (1 << 10)
#define ARENA_BASE_ALIGN       64   /* power of two; alignment guaranteed for buffer start */

// typedef struct {
//     max_align_t _alignment;
//     uint8_t *buffer;
//     size_t offset, capacity;
// } Lad_Arena;

Lad_Arena *arena_new(size_t capacity) {
    if (!capacity) { capacity = DEFAULT_ARENA_CAPACITY; }

    const size_t slack = ARENA_BASE_ALIGN - 1;
    if (capacity > SIZE_MAX - sizeof(Lad_Arena) - slack) { return NULL; }

    Lad_Arena *arena = malloc(sizeof(Lad_Arena) + capacity + slack);
    if (!arena) { return NULL; }

    uintptr_t raw     = (uintptr_t) arena + sizeof(Lad_Arena);
    uintptr_t aligned = (raw + slack) & ~(uintptr_t) slack;

    arena->buffer   = (uint8_t *) arena + (aligned - (uintptr_t) arena);
    arena->offset   = 0;
    arena->capacity = capacity;

    return arena;
}

void *arena_alloc_walign(Lad_Arena *arena, size_t item_size, size_t alignment) {
    if (!arena || !item_size || !alignment) { return NULL; }
    if (alignment & (alignment - 1))        { return NULL; } /* must be a power of two */

    size_t buf_align_offset = (uintptr_t) arena->buffer & (alignment - 1);
    size_t cur_align_offset = (buf_align_offset + (arena->offset & (alignment - 1))) & (alignment - 1);
    size_t padding          = (alignment - cur_align_offset) & (alignment - 1);

    size_t remaining = arena->capacity - arena->offset;
    if (padding > remaining || item_size > remaining - padding) { return NULL; }

    void *ptr      = arena->buffer + arena->offset + padding;
    arena->offset += padding + item_size;

    return ptr;
}

void *arena_alloc(Lad_Arena *arena, size_t item_size) {
    return arena_alloc_walign(arena, item_size, _Alignof(max_align_t));
}

size_t arena_save(const Lad_Arena *arena) { if (arena) { return arena->offset; } return 0; }
size_t arena_remaining(const Lad_Arena *arena) { if (arena) { return arena->capacity - arena->offset; } return 0; }

void arena_restore(Lad_Arena *arena, const size_t checkpoint) { if (arena) { arena->offset = checkpoint; } }
void arena_reset(Lad_Arena *arena) { arena_restore(arena, 0); }
void arena_destroy(Lad_Arena *arena)  { if (arena) { free(arena); arena = NULL; } }