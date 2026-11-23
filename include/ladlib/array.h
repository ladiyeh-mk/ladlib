#ifndef LADLIB_ARRAY_H
#define LADLIB_ARRAY_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Usage: int *nums = array_new(sizeof(int), 0);
 *        array_append((void **) &nums, &x);
 *        array_destroy((void **) &nums);
 *
 * append, insert and reserve can move the array, so pass &handle
 * and re-read any item pointers after calling them.
 */

void  *array_new(size_t item_size, size_t capacity);      /* capacity 0 = default (8), NULL on failure */
void  *array_reserve(void **array_ptr, size_t capacity);  /* never shrinks, NULL on failure */

bool   array_append(void **array_ptr, const void *item);
bool   array_insert(void **array_ptr, const void *item, size_t index);
bool   array_remove(void **array_ptr, size_t index);
bool   array_pop(void **array_ptr, void *out_dest);

void   array_clear(void *array);                          /* len = 0, keeps memory */
void   array_destroy(void **array_ptr);                   /* frees and sets handle to NULL */

size_t array_len(const void *array);
size_t array_capacity(const void *array);

#ifdef __cplusplus
}
#endif

#endif /* LADLIB_ARRAY_H */