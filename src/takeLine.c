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

AllocResult fr_takeLineAlloc(FileReader* fr)
{
    AllocResult out = {.data = NULL, .len = 0, .err = Read_Ok};

    uint8_t* head = fr->buffer + fr->head;
    size_t remainingLen = fr->len - fr->head;
    uint8_t* maybeNewline = memchr(head, '\n', remainingLen);
    if (maybeNewline) {
        size_t len = maybeNewline + 1 - head;
        SliceResult res = fr_takeSlice(fr, len);
        if (res.err != Read_Ok) {
            out.err = res.err;
            goto out;
        }

        out.data = newStringFromSlice(res.slice, len);
        out.len = len;
        if (!out.data) {
            out.err = Read_OOM;
            goto out;
        }

        goto out;
    }

    SliceResult remainingBuffer = fr_takeSlice(fr, remainingLen);
    if (remainingBuffer.err != Read_Ok) {
        out.err = remainingBuffer.err;
        goto out;
    }

    uint8_t* line = newStringFromSlice(remainingBuffer.slice, remainingLen);
    if (!line) {
        out.err = Read_OOM;
        goto out;
    }

    SliceResult buffer = fr_peekSlice(fr, FILE_READER_BUFFER_SIZE);
    if (buffer.err != Read_Ok) {
        out.err = buffer.err;
        goto out;
    }

out:
    if (out.err != Read_Ok) {
        free(out.data);
    }
    return out;
}
