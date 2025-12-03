#include "FileReader.h"

#include <stdlib.h>
#include <string.h>

typedef struct {
    uint8_t* data;
    size_t len;
    size_t cap;
} ByteVector;

static ByteVector bv_new(void)
{
    return (ByteVector){.data = NULL, .len = 0, .cap = 0};
}

static const float growingFactor = 1.5;
static const size_t initialCapacity = 64;

static ReadError bv_grow(ByteVector* vec)
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

static ReadError bv_push(ByteVector* vec, uint8_t value)
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
    ReadError err = Read_Ok;
    ByteVector line = bv_new();

    if (fr_peekByte(fr).err == Read_EOF) {
        err = Read_EOF;
        goto out;
    }

    while (1) {
        ByteResult byte = fr_takeByte(fr);
        if (byte.err == Read_EOF) {
            break;
        } else if (byte.err != Read_Ok) {
            err = byte.err;
            goto out;
        }

        err = bv_push(&line, byte.byte);
        if (err != Read_Ok) {
            goto out;
        }

        if (byte.byte == '\n') {
            break;
        }
    }

out:
    if (err != Read_Ok) {
        free(line.data);
        line.data = NULL;
        line.len = 0;
    }
    return (AllocResult){.data = line.data, .len = line.len, .err = err};
}
