#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// cmake should provide the buffer size
// or you do it
#ifndef FILE_READER_BUFFER_SIZE
#error "no buffer size"
#endif

typedef enum {
    FileReader_ReadStatus_Ok,
    FileReader_ReadStatus_ReadErr,
    FileReader_ReadStatus_EOF,
    FileReader_ReadStatus_OOM,
} FileReader_ReadStatus;

typedef struct {
    uint8_t byte;
    FileReader_ReadStatus status;
} ByteResult;

typedef struct {
    const uint8_t* slice;
    size_t len;
    FileReader_ReadStatus status;
} SliceResult;

typedef struct {
    uint8_t* data;
    size_t len;
    FileReader_ReadStatus status;
} AllocResult;

typedef struct {
    int fd;
    uint8_t buffer[FILE_READER_BUFFER_SIZE];
    size_t head;
    size_t len;
    bool ownsFd;
} FileReader;

FileReader fr_open(const char* path);
FileReader fr_fromFd(int fd);  // fr_close is a no-op on borrowed fds
void fr_close(FileReader* fr);
bool fr_isOpened(const FileReader* fr);

ByteResult fr_peekByte(FileReader* fr);
ByteResult fr_takeByte(FileReader* fr);

// returns a contiguous slice to the internal buffer
// the slice might be invalidated by another call to read
SliceResult fr_peekSlice(FileReader* fr, size_t sz);
SliceResult fr_takeSlice(FileReader* fr, size_t sz);

// you own this, don't lose it
AllocResult fr_takeLineAlloc(FileReader* fr);
AllocResult fr_takeUntilAlloc(FileReader* fr, bool (*predicate)(uint8_t));

FileReader_ReadStatus fr_skip(FileReader* fr, size_t sz);

#ifdef __cplusplus
}
#endif
