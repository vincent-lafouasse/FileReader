#include "FileReader.h"

#include <stdlib.h>
#include <string.h>

uint8_t* newStringFromSlice(const uint8_t* data, size_t size)
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

AllocResult fr_takeLineAlloc(FileReader* fr)
{
    uint8_t* maybeNewline =
        memchr(fr->buffer + fr->head, '\n', fr->len - fr->head);
    if (maybeNewline) {
        size_t len = maybeNewline + 1 - (fr->buffer + fr->head);
    }
    return (AllocResult){0};
}
