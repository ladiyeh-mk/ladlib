#include "../include/ladlib/array.h"

#include <stdint.h>
#include <stdlib.h>

#define DEFAULT_ARRAY_CAPACITY 8

typedef struct {
    max_align_t _alignment;
    size_t m_item_size, m_len, m_capacity;
} ArrayHeader;

// === Utils funcs () ===
static inline void array_mem_copy(void *dest, const void *src, size_t n) { uint8_t *d = dest; const uint8_t *s = src; while (n > 0) { *d++ = *s++; n--; } }

static void *array_grow(void **array_ptr) {

    void *array         = *array_ptr;
    ArrayHeader *header = (ArrayHeader *) ((uint8_t *) array - sizeof(ArrayHeader));

    size_t growth = header->m_capacity >> 1;
    if (!growth) { growth = DEFAULT_ARRAY_CAPACITY; }

    if (header->m_capacity > SIZE_MAX - growth) { return NULL; }
    return array_reserve(array_ptr, header->m_capacity + growth);
}
// ===

void *array_new(const size_t item_size, size_t capacity) {

    if (!item_size) { return NULL; }
    if (!capacity)  { capacity = DEFAULT_ARRAY_CAPACITY; }
    
    if (item_size > (SIZE_MAX - sizeof(ArrayHeader)) / capacity) { return NULL; }

    void *raw = malloc(sizeof(ArrayHeader) + (item_size * capacity));
    if (!raw) { return NULL; }

    ArrayHeader *header   = (ArrayHeader *) raw;
    header->m_item_size   = item_size;
    header->m_capacity    = capacity;
    header->m_len         = 0;

    return (void *) ((uint8_t *) raw + sizeof(ArrayHeader));
}

void *array_reserve(void **array_ptr, size_t capacity) {

    if (!array_ptr || !*array_ptr) { return NULL; }
    
    void *array         = *array_ptr;
    ArrayHeader *header = (ArrayHeader *) ((uint8_t *) array - sizeof(ArrayHeader));

    if (capacity <= header->m_capacity) { return *array_ptr; }
    if (capacity > (SIZE_MAX - sizeof(ArrayHeader)) / header->m_item_size) { return NULL; }

    void *raw = realloc(header, sizeof(ArrayHeader) + header->m_item_size * capacity);
    if (!raw) { return NULL; }

    header             = (ArrayHeader *) raw;
    header->m_capacity = capacity;

    *array_ptr = (void *) ((uint8_t *) raw + sizeof(ArrayHeader));
    return *array_ptr;
}

bool array_append(void **array_ptr, const void *item) {

    if (!array_ptr || !*array_ptr || !item) { return false; }

    void *array         = *array_ptr;
    ArrayHeader *header = (ArrayHeader *) ((uint8_t *) array - sizeof(ArrayHeader));

    uintptr_t item_addr  = (uintptr_t) item;
    uintptr_t array_addr = (uintptr_t) array;
    bool      item_inside_array = item_addr >= array_addr &&
                                    item_addr <  array_addr + header->m_len * header->m_item_size;
    size_t    item_offset       = item_inside_array ? (size_t) (item_addr - array_addr) : 0;

    if (header->m_len >= header->m_capacity) {
        if (!array_grow(array_ptr)) { return false; }

        array  = *array_ptr;
        header = (ArrayHeader *) ((uint8_t *) array - sizeof(ArrayHeader));
    }

    if (item_inside_array) { item = (uint8_t *) array + item_offset; }

    void *dest = (uint8_t *) array + (header->m_len * header->m_item_size);
    array_mem_copy(dest, item, header->m_item_size);
    header->m_len++;

    return true;
}

bool array_insert(void **array_ptr, const void *item, size_t index) {

    if (!array_ptr || !*array_ptr) { return NULL; }

    void *array         = *array_ptr;
    ArrayHeader *header = (ArrayHeader *) ((uint8_t *) array - sizeof(ArrayHeader));

    if (index > header->m_len) { return false; }

    void *item_copy = malloc(header->m_item_size);
    if (!item_copy) { return false; }

    array_mem_copy(item_copy, item, header->m_item_size);

    if (header->m_len >= header->m_capacity) {
        if (!array_grow(array_ptr)) { free(item_copy); return false; }

        array  = *array_ptr;
        header = (ArrayHeader *) ((uint8_t *) array - sizeof(ArrayHeader));
    }

    for (size_t i = header->m_len; i > index; i--) {
        array_mem_copy((uint8_t *) array + i * header->m_item_size, (uint8_t *) array + (i - 1) * header->m_item_size, header->m_item_size);
    }

    array_mem_copy((uint8_t *) array + index * header->m_item_size, item_copy, header->m_item_size);
    header->m_len++;

    free(item_copy);
    return true;
}

bool array_remove(void **array_ptr, size_t index) {

    if (!array_ptr || !*array_ptr) { return NULL; }

    void *array         = *array_ptr;
    ArrayHeader *header = (ArrayHeader *) ((uint8_t *) array - sizeof(ArrayHeader));

    if (index >= header->m_len) { return false; }

    for (size_t i = index; i + 1 < header->m_len; i++) {
        array_mem_copy(
            (uint8_t *) array + (i * header->m_item_size),
            (uint8_t *) array + ((i + 1) * header->m_item_size),
            header->m_item_size
        );
    }

    header->m_len--;
    return true;
}

bool array_pop(void **array_ptr, void *out_dest) {
    
    if (!array_ptr || !*array_ptr || !out_dest) { return NULL; }

    void *array         = *array_ptr;
    ArrayHeader *header = (ArrayHeader *) ((uint8_t *) array - sizeof(ArrayHeader));

    if (!header->m_len) { return false; }

    header->m_len--;
    void *src = (uint8_t *) array + (header->m_len * header->m_item_size);
    array_mem_copy(out_dest, src, header->m_item_size);

    return true;
}

void array_clear(void *array) {

    if (!array) { return; }

    ArrayHeader *header = (ArrayHeader *) ((uint8_t *) array - sizeof(ArrayHeader));
    header->m_len       = 0;
}

void array_destroy(void **array_ptr) {

    if (!array_ptr || !*array_ptr) { return; }

    void *array         = *array_ptr;
    ArrayHeader *header = (ArrayHeader *) ((uint8_t *) array - sizeof(ArrayHeader));

    free(header);
    *array_ptr = NULL;
}

size_t array_len(const void *array) {

    if (!array) { return 0; }

    const ArrayHeader *header = (const ArrayHeader *) ((const uint8_t *) array - sizeof(ArrayHeader));
    return header->m_len;
}

size_t array_capacity(const void *array) {

    if (!array) { return 0; }

    const ArrayHeader *header = (const ArrayHeader *) ((const uint8_t *) array - sizeof(ArrayHeader));
    return header->m_capacity;
}