#ifndef LADLIB_LO_ARRAY_H
#define LADLIB_LO_ARRAY_H

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Usage: Array *nums = lo_array_new_default(sizeof(int));
 *        lo_array_append(nums, &x);
 *        int *items = lo_array_get_items(nums);
 *        lo_array_destroy(&nums);
 *
 * The Array handle never moves, but append, insert and reserve can move
 * the item buffer, so call lo_array_get_items again after them.
 */

typedef struct Array Array;

Array  *lo_array_new_default(size_t item_size);           /* starts at DEFAULT_ARRAY_CAPACITY, NULL on failure */
Array  *lo_array_new(size_t item_size, size_t capacity);  /* NULL on failure */
void   *lo_array_get_items(Array *array);                 /* raw item buffer, can be invalidated by growth */
bool   lo_array_reserve(Array *array, size_t capacity);   /* false on failure */

bool   lo_array_append(Array *array, const void *item);
bool   lo_array_insert(Array *array, const void *item, size_t index);
bool   lo_array_remove(Array *array, size_t index);
bool   lo_array_pop(Array *array, void *out_dest);        /* copies last item into out_dest */

void   lo_array_clear(Array *array);                      /* len = 0, keeps memory */
void   lo_array_destroy(Array **array_ptr);               /* frees and sets handle to NULL */

size_t lo_array_len(const Array *array);
size_t lo_array_capacity(const Array *array);

#ifdef __cplusplus
}
#endif

#endif /* LADLIB_LO_ARRAY_H */