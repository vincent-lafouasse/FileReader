#include "FileReader.h"

#include <stdlib.h>
#include <string.h>

static uint8_t* newStringFromSlice(const uint8_t* data, size_t size)
{
    if (!data || !size) {
        return NULL;
    }

    uint8_t* out = malloc(size + 1);
    if (!out) {
        return NULL;
    }

    memcpy(out, data, size);
    out[size] = 0;
    return out;
}

typedef struct {
    uint8_t* data;
    size_t len;
    size_t cap;
} ByteVector;

static const float growingFactor = 1.5;
static const size_t initialCapacity = 64;

ReadError bv_grow(ByteVector* vec)
{
    if (vec->cap == 0) {
        vec->data = malloc(initialCapacity);
        vec->cap = initialCapacity;
        if (!vec->data) {
            return Read_OOM;
        } else {
            return Read_Ok;
        }
    }

    const float rawCapacity = growingFactor * (float)vec->cap;
    const size_t newCapacity = (size_t)rawCapacity;
    uint8_t* newBuffer = malloc(newCapacity);
    if (!newBuffer) {
        return Read_OOM;
    }
    memcpy(newBuffer, vec->data, vec->len);
    vec->cap = newCapacity;
    free(vec->data);
    vec->data = newBuffer;
    return Read_Ok;
}

ReadError bv_push(ByteVector* vec, uint8_t value)
{
    if (vec->len == vec->cap) {
        ReadError err = bv_grow(vec);
        if (err != Read_Ok) {
            return err;
        }
    }

    vec->data[vec->len++] = value;
    return Read_Ok;
}

AllocResult fr_takeLineAlloc(FileReader* fr)
{
    AllocResult out = {.data = NULL, .len = 0, .err = Read_Ok};

out:
    if (out.err != Read_Ok) {
        free(out.data);
    }
    return out;
}
