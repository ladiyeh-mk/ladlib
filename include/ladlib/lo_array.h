#ifndef LO_ARRAY_H
#define LO_ARRAY_H

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DEFAULT_ARRAY_CAPACITY
#define DEFAULT_ARRAY_CAPACITY 8
#endif

typedef struct Array Array;

Array  *lo_array_new_default(size_t item_size);
Array  *lo_array_new(size_t item_size, size_t capacity);
void   *lo_array_get_items(Array *array);
bool   lo_array_reserve(Array *array, size_t capacity);

bool   lo_array_append(Array *array, const void *item);
bool   lo_array_insert(Array *array, const void *item, size_t index);
bool   lo_array_remove(Array *array, size_t index);
bool   lo_array_pop(Array *array, void *out_dest);

void   lo_array_clear(Array *array);
void   lo_array_destroy(Array **array_ptr);

size_t lo_array_len(const Array *array);
size_t lo_array_capacity(const Array *array);

#ifdef __cplusplus
}
#endif

#endif /* LO_ARRAY_H */