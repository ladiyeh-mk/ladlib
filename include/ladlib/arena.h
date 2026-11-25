#ifndef LADLIB_ARENA_H
#define LADLIB_ARENA_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Usage: Lad_Arena *arena = arena_new(0);
 *        float *data = arena_alloc_walign(arena, n * sizeof(float), 64);
 *        size_t mark = arena_save(arena);
 *        ...temporary allocations...
 *        arena_restore(arena, mark);
 *        arena_destroy(arena);
 *
 * Memory is not zeroed. Pointers stay valid until restore, reset or destroy.
 */

typedef struct {
    max_align_t _alignment;
    uint8_t *buffer;
    size_t offset, capacity;
} Lad_Arena;

Lad_Arena *arena_new(size_t capacity);                    /* capacity 0 = default (1024), NULL on failure */

void  *arena_alloc(Lad_Arena *arena, size_t item_size);   /* max_align_t aligned, NULL if full */
void  *arena_alloc_walign(Lad_Arena *arena, size_t item_size, size_t alignment); /* power of two, NULL if full */

size_t arena_save(const Lad_Arena *arena);                /* current offset, use as checkpoint */
size_t arena_remaining(const Lad_Arena *arena);

void   arena_restore(Lad_Arena *arena, size_t checkpoint); /* rewinds only, ignores later offsets */
void   arena_reset(Lad_Arena *arena);                     /* offset = 0, keeps memory */
void   arena_destroy(Lad_Arena *arena);

#ifdef __cplusplus
}
#endif

#endif /* LADLIB_ARENA_H */