#include "FileReader.h"

#include <assert.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

static const size_t buffer_size = FILE_READER_BUFFER_SIZE;

FileReader fr_open(const char* path)
{
    const int fd = open(path, O_RDONLY);

    return (FileReader){.fd = fd, .head = 0, .len = 0};
}

void fr_close(FileReader* fr)
{
    if (fr->fd > 2) {
        close(fr->fd);
    }
    fr->fd = -1;
}

bool fr_isOpened(const FileReader* fr)
{
    return fr && fr->fd != -1;
}

static void fr_reseatHead(FileReader* r)
{
    const size_t newLen = r->len - r->head;
    const uint8_t* head = r->buffer + r->head;
    memcpy(r->buffer, head, newLen);
    r->head = 0;
    r->len = newLen;
}

static ReadError fr_fillRemaining(FileReader* r)
{
    const ssize_t bytesRead =
        read(r->fd, r->buffer + r->len, buffer_size - r->len);

    if (bytesRead < 0) {
        return Read_Err;
    } else if (bytesRead == 0) {
        return Read_EOF;
    }

    r->len += bytesRead;
    return Read_Ok;
}

SliceResult fr_peekSlice(FileReader* fr, size_t sz)
{
    assert(sz < buffer_size);

    if (fr->len - fr->head < sz) {
        fr_reseatHead(fr);
        const ReadError err = fr_fillRemaining(fr);
        if (err != Read_Ok) {
            return (SliceResult){.err = err};
        }
    }
    if (fr->len < sz) {
        return (SliceResult){.err = Read_Err};
    }

    // a correct read is assured here
    return (SliceResult){.slice = fr->buffer + fr->head, .err = Read_Ok};
}

SliceResult fr_takeSlice(FileReader* fr, size_t sz)
{
    const SliceResult maybeSlice = fr_peekSlice(fr, sz);
    if (maybeSlice.err == Read_Ok) {
        fr->head += sz;
    }
    return maybeSlice;
}

ByteResult fr_takeByte(FileReader* fr)
{
    const ByteResult maybeByte = fr_peekByte(fr);
    if (maybeByte.err == Read_Ok) {
        fr->head += 1;
    }
    return maybeByte;
}

ByteResult fr_peekByte(FileReader* fr)
{
    if (fr == NULL) {
        return (ByteResult){.err = Read_Err};
    }
    if (fr->len == 0 || fr->head == fr->len) {
        const ssize_t bytesRead = read(fr->fd, fr->buffer, buffer_size);
        if (bytesRead < 0) {
            return (ByteResult){.err = Read_Err};
        } else if (bytesRead == 0) {
            return (ByteResult){.err = Read_EOF};
        }
        fr->len = bytesRead;
        fr->head = 0;
    }
    return (ByteResult){.byte = fr->buffer[fr->head], .err = Read_Ok};
}

ReadError fr_skip(FileReader* fr, size_t sz)
{
    for (size_t i = 0; i < sz; i++) {
        const ByteResult maybeByte = fr_takeByte(fr);
        if (maybeByte.err != Read_Ok) {
            return maybeByte.err;
        }
    }
    return Read_Ok;
}

const char* rr_repr(ReadError rr)
{
    switch (rr) {
        case Read_Ok:
            return "Ok";
        case Read_Err:
            return "Err";
        case Read_EOF:
            return "EOF";
    }
    return NULL;
}
