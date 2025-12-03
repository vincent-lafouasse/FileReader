#include <cstdint>
#include <cstring>
#include <format>

#include "gtest/gtest.h"

extern "C" {
#include "FileReader.h"
#include "unistd.h"
}

namespace {
std::string writeTempFile(const std::vector<uint8_t>& data)
{
    char path[] = "/tmp/fr_testXXXXXX";
    int fd = mkstemp(path);
    if (fd < 0) {
        perror("mkstemp");
        abort();
    }
    write(fd, data.data(), data.size());
    close(fd);
    return path;
}

[[maybe_unused]] void logReader(const FileReader& r)
{
    std::cerr << std::format("FileReader {{\n");
    std::cerr << std::format("\tfd:\t{}\n", r.fd);
    std::cerr << std::format("\thead:\t{}\n", r.head);
    std::cerr << std::format("\tlen:\t{}\n", r.len);

    std::cerr << "\t\"";
    for (size_t i = 0; i < r.len; ++i) {
        std::cerr << std::format("{:02x}", r.buffer[i]);
    }
    std::cerr << std::format("\"\n");
    std::cerr << std::format("}}\n");
}
}  // namespace

TEST(FileReader, OpenClose)
{
    std::vector<uint8_t> data = {1, 2, 3};
    auto file = writeTempFile(data);
    FileReader fr = fr_open(file.c_str());
    ASSERT_TRUE(fr_isOpened(&fr));
    fr_close(&fr);
    ASSERT_FALSE(fr_isOpened(&fr));
}

// peek doesn't advance, take does
TEST(FileReader, PeekAndTakeByte)
{
    std::vector<uint8_t> data = {10, 20, 30};
    const auto file = writeTempFile(data);
    FileReader fr = fr_open(file.c_str());

    // Peek first byte
    ByteResult maybeByte = fr_peekByte(&fr);
    uint8_t expected = 10;
    EXPECT_EQ(maybeByte.status, ReadStatus_Ok);
    EXPECT_EQ(maybeByte.byte, expected);

    // Peek again: should still be 10
    maybeByte = fr_peekByte(&fr);
    expected = 10;
    EXPECT_EQ(maybeByte.status, ReadStatus_Ok);
    EXPECT_EQ(maybeByte.byte, expected);

    // Take now
    maybeByte = fr_takeByte(&fr);
    expected = 10;
    EXPECT_EQ(maybeByte.status, ReadStatus_Ok);
    EXPECT_EQ(maybeByte.byte, expected);

    // Next byte is 20
    maybeByte = fr_takeByte(&fr);
    expected = 20;
    EXPECT_EQ(maybeByte.status, ReadStatus_Ok);
    EXPECT_EQ(maybeByte.byte, expected);

    // Next is 30
    maybeByte = fr_takeByte(&fr);
    expected = 30;
    EXPECT_EQ(maybeByte.status, ReadStatus_Ok);
    EXPECT_EQ(maybeByte.byte, expected);

    // Now EOF
    EXPECT_EQ(fr_peekByte(&fr).status, ReadStatus_EOF);
    EXPECT_EQ(fr_takeByte(&fr).status, ReadStatus_EOF);

    fr_close(&fr);
}

TEST(FileReader, PeekAndTakeSlice)
{
    std::vector<uint8_t> data = {1, 2, 3, 4};
    auto file = writeTempFile(data);
    FileReader fr = fr_open(file.c_str());

    // Peek first 3 bytes
    SliceResult maybeSlice = fr_peekSlice(&fr, 3);
    const uint8_t* slice = maybeSlice.slice;
    assert(maybeSlice.status == ReadStatus_Ok);
    EXPECT_EQ(slice[0], 1);
    EXPECT_EQ(slice[1], 2);
    EXPECT_EQ(slice[2], 3);

    // Peek again — still same
    maybeSlice = fr_peekSlice(&fr, 3);
    assert(maybeSlice.status == ReadStatus_Ok);
    slice = maybeSlice.slice;
    EXPECT_EQ(slice[0], 1);
    EXPECT_EQ(slice[1], 2);
    EXPECT_EQ(slice[2], 3);

    // Take 3 bytes
    maybeSlice = fr_takeSlice(&fr, 3);
    assert(maybeSlice.status == ReadStatus_Ok);
    slice = maybeSlice.slice;
    EXPECT_EQ(slice[0], 1);
    EXPECT_EQ(slice[1], 2);
    EXPECT_EQ(slice[2], 3);

    // 1 byte left
    ByteResult maybeByte = fr_takeByte(&fr);
    EXPECT_EQ(maybeByte.status, ReadStatus_Ok);
    EXPECT_EQ(maybeByte.byte, 4);

    // EOF
    EXPECT_EQ(fr_peekSlice(&fr, 1).status, ReadStatus_EOF);
    EXPECT_EQ(fr_takeSlice(&fr, 1).status, ReadStatus_EOF);

    fr_close(&fr);
}

// partial slice does not advance
TEST(FileReader, SlicePartialReadFailsAndDoesNotAdvance)
{
    std::vector<uint8_t> data = {6, 7};
    auto file = writeTempFile(data);
    FileReader fr = fr_open(file.c_str());

    // Try reading 3 bytes but file has only 2
    EXPECT_EQ(fr_peekSlice(&fr, 3).status, ReadStatus_ReadErr);

    // Ensure buffer hasn't advanced: peek a byte -> should be 9
    ByteResult maybeByte = fr_peekByte(&fr);
    EXPECT_EQ(maybeByte.status, ReadStatus_Ok);
    EXPECT_EQ(maybeByte.byte, 6);

    // take a byte, ensure it's still 9
    maybeByte = fr_takeByte(&fr);
    EXPECT_EQ(maybeByte.status, ReadStatus_Ok);
    EXPECT_EQ(maybeByte.byte, 6);

    // now only 1 byte remains
    maybeByte = fr_takeByte(&fr);
    EXPECT_EQ(maybeByte.status, ReadStatus_Ok);
    EXPECT_EQ(maybeByte.byte, 7);

    // EOF
    EXPECT_EQ(fr_peekSlice(&fr, 1).status, ReadStatus_EOF);
    EXPECT_EQ(fr_peekSlice(&fr, 67).status, ReadStatus_EOF);

    fr_close(&fr);
}

TEST(FileReader, SliceCrossesBufferBoundary)
{
    constexpr size_t sz = FILE_READER_BUFFER_SIZE;

    std::vector<uint8_t> data(sz + 5);
    for (size_t i = 0; i < data.size(); i++) {
        data[i] = i & 0xff;
    }

    auto file = writeTempFile(data);
    FileReader fr = fr_open(file.c_str());

    // Move head to sz - 2
    for (size_t i = 0; i < sz - 2; i++) {
        const ByteResult maybeByte = fr_takeByte(&fr);
        EXPECT_EQ(maybeByte.status, ReadStatus_Ok);
        EXPECT_EQ(maybeByte.byte, data[i]);
    }

    // Now head == sz - 2 inside buffer.
    // Request a slice of 6 bytes => must cross the boundary.
    const SliceResult maybeSlice = fr_takeSlice(&fr, 6);
    const uint8_t* expectedValues = data.data() + sz - 2;
    EXPECT_EQ(maybeSlice.status, ReadStatus_Ok);
    EXPECT_EQ(memcmp(maybeSlice.slice, expectedValues, 6), 0);

    // Now 1 byte should remain (the last element in data)
    const ByteResult last = fr_takeByte(&fr);
    EXPECT_EQ(last.status, ReadStatus_Ok);
    EXPECT_EQ(last.byte, data.back());

    // EOF
    EXPECT_EQ(fr_peekSlice(&fr, 1).status, ReadStatus_EOF);
    EXPECT_EQ(fr_peekSlice(&fr, 67).status, ReadStatus_EOF);
    EXPECT_EQ(fr_peekByte(&fr).status, ReadStatus_EOF);
    EXPECT_EQ(fr_takeByte(&fr).status, ReadStatus_EOF);

    fr_close(&fr);
}

TEST(FileReader, CrossingPartialSliceFailsAndDoesNotAdvance)
{
    constexpr size_t sz = FILE_READER_BUFFER_SIZE;

    // File shorter than slice request
    std::vector<uint8_t> data(sz + 3);  // 3 bytes after boundary
    for (size_t i = 0; i < data.size(); i++) {
        data[i] = i & 0xff;
    }

    auto file = writeTempFile(data);
    FileReader fr = fr_open(file.c_str());

    // Advance near end: leave only 3 bytes remaining
    EXPECT_EQ(fr_skip(&fr, data.size() - 3), ReadStatus_Ok);

    // Try reading 6 bytes, but only 3 remain in the whole file
    EXPECT_EQ(fr_peekSlice(&fr, 6).status, ReadStatus_ReadErr);

    // Reader must NOT advance
    const ByteResult maybeByte = fr_takeByte(&fr);
    EXPECT_EQ(maybeByte.status, ReadStatus_Ok);
    EXPECT_EQ(maybeByte.byte, data[data.size() - 3]);

    fr_close(&fr);
}
