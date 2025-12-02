#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define FILE_READER_BUFFER_SIZE 128

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

typedef struct {
    uint8_t* slice;
    size_t len;
    ReadError err;
} SliceResult;

typedef struct {
    int fd;
    uint8_t buffer[FILE_READER_BUFFER_SIZE];
    size_t head;
    size_t len;
} FileReader;

FileReader fr_open(const char* path);
void fr_close(FileReader* fr);
bool fr_isOpened(const FileReader* fr);

ByteResult fr_peekByte(FileReader* fr);
ByteResult fr_takeByte(FileReader* fr);

SliceResult fr_peekSlice(FileReader* fr, size_t sz);
SliceResult fr_takeSlice(FileReader* fr, size_t sz);

ReadError fr_skip(FileReader* fr, size_t sz);
