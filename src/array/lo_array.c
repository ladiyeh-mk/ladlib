#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#define DEFAULT_ARRAY_CAPACITY 8
static inline void array_mem_copy(void *dest, const void *src, size_t n) { uint8_t *d = dest; const uint8_t *s = src; while (n > 0) { *d++ = *s++; n--; } }

typedef struct Array {

    void *m_items;
    size_t m_item_size, m_len, m_capacity;

} Array;

Array *lo_array_new_default(const size_t item_size) {
    if (!item_size) { return NULL; }
    if (item_size > SIZE_MAX / DEFAULT_ARRAY_CAPACITY) { return NULL; }

    Array *array = malloc(sizeof(Array));
    if (!array) { return NULL; }

    array->m_item_size = item_size;
    array->m_capacity  = DEFAULT_ARRAY_CAPACITY;
    array->m_len       = 0;

    array->m_items     = malloc(item_size * DEFAULT_ARRAY_CAPACITY);
    if (!array->m_items) { free(array); return NULL; }

    return array;
}

Array *lo_array_new(const size_t item_size, const size_t capacity) {
    if (!item_size || !capacity) { return NULL; }
    if (item_size > SIZE_MAX / capacity) { return NULL; }

    Array *array = malloc(sizeof(Array));
    if (!array) { return NULL; }

    array->m_item_size = item_size;
    array->m_capacity  = capacity;
    array->m_len       = 0;

    array->m_items     = malloc(item_size * capacity);
    if (!array->m_items) { free(array); return NULL; }

    return array;
}

void *lo_array_get_items(Array *array) { if (array) { if (array->m_items) { return array->m_items; } } return NULL; }

bool lo_array_reserve(Array *array, const size_t capacity) {
    if (!array || !array->m_items || !capacity) { return false; }

    if (capacity <= array->m_capacity) { return true; }
    if (capacity > SIZE_MAX / array->m_item_size) { return false; }

    void *raw = realloc(array->m_items, array->m_item_size * capacity);
    if (!raw) { return false; }

    array->m_items    = raw;
    array->m_capacity = capacity;

    return true;
}

static bool lo_array_grow(Array *array) {
    size_t growth = array->m_capacity >> 1;
    if (growth == 0) { growth = 1; }

    if (array->m_capacity > SIZE_MAX - growth) { return false; }
    return lo_array_reserve(array, array->m_capacity + growth);
}

bool lo_array_append(Array *array, const void *item) {
    if (!array || !array->m_items || !item) { return false; }

    uintptr_t item_addr  = (uintptr_t) item;
    uintptr_t array_addr = (uintptr_t) array->m_items;
    bool      item_inside_array = item_addr >= array_addr &&
                                    item_addr <  array_addr + array->m_len * array->m_item_size;
    size_t    item_offset       = item_inside_array ? (size_t) (item_addr - array_addr) : 0;

    if (array->m_len >= array->m_capacity && !lo_array_grow(array)) { return false; }
    if (item_inside_array) { item = (uint8_t *) array->m_items + item_offset; }

    void *destination = (uint8_t *) array->m_items + (array->m_len * array->m_item_size);
    array_mem_copy(destination, item, array->m_item_size);
    array->m_len++;

    return true;
}

bool lo_array_insert(Array *array, const void *item, size_t index) {
    if (!array || !array->m_items || !item || index > array->m_len) { return false; }

    size_t item_size = array->m_item_size;

    void *copy = malloc(item_size);
    if (!copy) { return false; }

    array_mem_copy(copy, item, item_size);

    if (array->m_len >= array->m_capacity && !lo_array_grow(array)) {
        free(copy);
        return false;
    }

    uint8_t *base = array->m_items;

    for (size_t i = array->m_len; i > index; i--) {
        array_mem_copy(base + i * item_size, base + (i - 1) * item_size, item_size);
    }

    array_mem_copy(base + index * item_size, copy, item_size);
    array->m_len++;

    free(copy);
    return true;
}

bool lo_array_remove(Array *array, size_t index) {
    if (!array || !array->m_items || index >= array->m_len) { return false; }

    for (size_t i = index; i + 1 < array->m_len; i++) {
        array_mem_copy(
            (uint8_t *) array->m_items + (i * array->m_item_size),
            (uint8_t *) array->m_items + ((i + 1) * array->m_item_size),
            array->m_item_size
        );
    }

    array->m_len--;
    return true;
}

bool lo_array_pop(Array *array, void *out_dest) {
    if (!array || !array->m_items || !array->m_len || !out_dest) { return false; }

    array->m_len--;
    void *src = (uint8_t *) array->m_items + (array->m_len * array->m_item_size);
    array_mem_copy(out_dest, src, array->m_item_size);

    return true;
}

void lo_array_clear(Array *array) {
    if (!array || !array->m_items) { return; }
    array->m_len = 0;
}

void lo_array_destroy(Array **array_ptr) {
    if (!array_ptr || !*array_ptr) { return; }
    Array *array = *array_ptr;

    free(array->m_items);
    free(array);
    
    *array_ptr = NULL;
}

size_t lo_array_len(const Array *array) {
    if (!array || !array->m_items) { return 0; }
    return array->m_len;
}

size_t lo_array_capacity(const Array *array) {
    if (!array || !array->m_items) { return 0; }
    return array->m_capacity;
}