#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifndef FILE_READER_BUFFER_SIZE
#error "no buffer size"
#endif

typedef enum {
    Read_Ok,
    Read_Err,
    Read_EOF,
} ReadError;

const char* readErrorRepr(ReadError rr);

typedef struct {
    uint8_t byte;
    ReadError err;
} ByteResult;

// yes SliceResult and AllocResult are functionally identical but the semantics
// of .slice vs .data reinforces that the slice does not own the data
typedef struct {
    uint8_t* slice;
    size_t len;
    ReadError err;
} SliceResult;

typedef struct {
    uint8_t* data;
    size_t len;
    ReadError err;
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

SliceResult fr_peekSlice(FileReader* fr, size_t sz);
SliceResult fr_takeSlice(FileReader* fr, size_t sz);

AllocResult fr_takeLineAlloc(FileReader* fr);

ReadError fr_skip(FileReader* fr, size_t sz);
