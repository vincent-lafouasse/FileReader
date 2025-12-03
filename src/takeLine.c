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

static const float growingFactor = 1.5f;
static const size_t initialCapacity = 64;

static ReadStatus bv_grow(ByteVector* vec)
{
    if (vec->cap == 0) {
        vec->data = malloc(initialCapacity);
        vec->cap = initialCapacity;
        if (!vec->data) {
            return ReadStatus_OOM;
        } else {
            return ReadStatus_Ok;
        }
    }

    const float rawCapacity = growingFactor * (float)vec->cap;
    const size_t newCapacity = (size_t)rawCapacity;
    uint8_t* newBuffer = malloc(newCapacity);
    if (!newBuffer) {
        return ReadStatus_OOM;
    }
    memcpy(newBuffer, vec->data, vec->len);
    vec->cap = newCapacity;
    free(vec->data);
    vec->data = newBuffer;
    return ReadStatus_Ok;
}

static ReadStatus bv_push(ByteVector* vec, uint8_t value)
{
    if (vec->len == vec->cap) {
        ReadStatus status = bv_grow(vec);
        if (status != ReadStatus_Ok) {
            return status;
        }
    }

    vec->data[vec->len++] = value;
    return ReadStatus_Ok;
}

AllocResult fr_takeUntilAlloc(FileReader* fr, bool (*predicate)(uint8_t))
{
    ReadStatus status = ReadStatus_Ok;
    ByteVector line = bv_new();

    if (fr_peekByte(fr).status == ReadStatus_EOF) {
        status = ReadStatus_EOF;
        goto out;
    }

    while (1) {
        ByteResult byte = fr_takeByte(fr);
        if (byte.status == ReadStatus_EOF) {
            break;
        } else if (byte.status != ReadStatus_Ok) {
            status = byte.status;
            goto out;
        }

        status = bv_push(&line, byte.byte);
        if (status != ReadStatus_Ok) {
            goto out;
        }

        if (predicate(byte.byte)) {
            break;
        }
    }

out:
    if (status != ReadStatus_Ok) {
        free(line.data);
        line.data = NULL;
        line.len = 0;
    }
    return (AllocResult){.data = line.data, .len = line.len, .status = status};
}

static bool isNewline(uint8_t byte)
{
    return byte == '\n';
}

AllocResult fr_takeLineAlloc(FileReader* fr)
{
    return fr_takeUntilAlloc(fr, isNewline);
}
