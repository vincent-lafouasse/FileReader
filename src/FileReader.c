#include "FileReader.h"

#include <assert.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

FileReader fr_open(const char* path)
{
    const int fd = open(path, O_RDONLY);

    return (FileReader){.fd = fd, .head = 0, .len = 0, .ownsFd = true};
}

void fr_close(FileReader* fr)
{
    if (fr->ownsFd && fr->fd > 2) {
        close(fr->fd);
    }
    fr->fd = -1;
}

bool fr_isOpened(const FileReader* fr)
{
    return fr && fr->fd != -1;
}

FileReader fr_fromFd(int fd)
{
    return (FileReader){.fd = fd, .head = 0, .len = 0, .ownsFd = false};
}

static void fr_reseatHead(FileReader* r)
{
    const size_t newLen = r->len - r->head;
    const uint8_t* head = r->buffer + r->head;
    memcpy(r->buffer, head, newLen);
    r->head = 0;
    r->len = newLen;
}

static FileReader_ReadStatus fr_fillRemaining(FileReader* r)
{
    const ssize_t bytesRead =
        read(r->fd, r->buffer + r->len, FILE_READER_BUFFER_SIZE - r->len);

    if (bytesRead < 0) {
        return FileReader_ReadStatus_ReadErr;
    } else if (bytesRead == 0) {
        return FileReader_ReadStatus_EOF;
    }

    r->len += bytesRead;
    return FileReader_ReadStatus_Ok;
}

SliceResult fr_peekSlice(FileReader* fr, size_t sz)
{
    assert(sz < FILE_READER_BUFFER_SIZE);

    if (fr->len - fr->head < sz) {
        fr_reseatHead(fr);
        const FileReader_ReadStatus status = fr_fillRemaining(fr);
        if (status != FileReader_ReadStatus_Ok) {
            return (SliceResult){.status = status};
        }
    }
    if (fr->len < sz) {
        return (SliceResult){.status = FileReader_ReadStatus_ReadErr};
    }

    // a correct read is assured here
    return (SliceResult){.slice = fr->buffer + fr->head,
                         .len = sz,
                         .status = FileReader_ReadStatus_Ok};
}

SliceResult fr_takeSlice(FileReader* fr, size_t sz)
{
    const SliceResult maybeSlice = fr_peekSlice(fr, sz);
    if (maybeSlice.status == FileReader_ReadStatus_Ok) {
        fr->head += sz;
    }
    return maybeSlice;
}

ByteResult fr_takeByte(FileReader* fr)
{
    const ByteResult maybeByte = fr_peekByte(fr);
    if (maybeByte.status == FileReader_ReadStatus_Ok) {
        fr->head += 1;
    }
    return maybeByte;
}

ByteResult fr_peekByte(FileReader* fr)
{
    if (fr == NULL) {
        return (ByteResult){.status = FileReader_ReadStatus_ReadErr};
    }
    if (fr->len == 0 || fr->head == fr->len) {
        const ssize_t bytesRead =
            read(fr->fd, fr->buffer, FILE_READER_BUFFER_SIZE);
        if (bytesRead < 0) {
            return (ByteResult){.status = FileReader_ReadStatus_ReadErr};
        } else if (bytesRead == 0) {
            return (ByteResult){.status = FileReader_ReadStatus_EOF};
        }
        fr->len = bytesRead;
        fr->head = 0;
    }
    return (ByteResult){.byte = fr->buffer[fr->head],
                        .status = FileReader_ReadStatus_Ok};
}

FileReader_ReadStatus fr_skip(FileReader* fr, size_t sz)
{
    for (size_t i = 0; i < sz; i++) {
        const ByteResult maybeByte = fr_takeByte(fr);
        if (maybeByte.status != FileReader_ReadStatus_Ok) {
            return maybeByte.status;
        }
    }
    return FileReader_ReadStatus_Ok;
}
