#include <cstdint>
#include <cstring>
#include <numeric>

#include "gtest/gtest.h"

extern "C" {
#include "FileReader.h"
#include "unistd.h"
}

namespace {
std::string writeTempFile(const std::string& data)
{
    char path[] = "/tmp/fr_testXXXXXX";
    int fd = mkstemp(path);
    if (fd < 0) {
        perror("mkstemp");
        abort();
    }
    write(fd, data.c_str(), data.size());
    close(fd);
    return path;
}

void expectSliceEq(const std::string& expected, const AllocResult& actual)
{
    ASSERT_EQ(actual.status, FileReader_ReadStatus_Ok);
    ASSERT_NE(actual.data, nullptr);
    ASSERT_EQ(actual.len, expected.size());
    ASSERT_EQ(std::memcmp(actual.data, expected.c_str(), expected.size()), 0);
}
}  // namespace

TEST(TakeLineAlloc, SingleLineWithNewline)
{
    const std::vector<std::string> lines = {"hello\n"};
    auto file = writeTempFile(
        std::accumulate(lines.begin(), lines.end(), std::string("")));
    FileReader fr = fr_open(file.c_str());

    AllocResult actual = fr_takeLineAlloc(&fr);
    expectSliceEq(lines[0], actual);
    free(actual.data);

    AllocResult eof = fr_takeLineAlloc(&fr);
    ASSERT_EQ(eof.status, FileReader_ReadStatus_EOF);
    fr_close(&fr);
}

TEST(TakeLineAlloc, SingleLineNoNewline)
{
    const std::vector<std::string> lines = {"hello"};
    auto file = writeTempFile(
        std::accumulate(lines.begin(), lines.end(), std::string("")));
    FileReader fr = fr_open(file.c_str());

    AllocResult actual = fr_takeLineAlloc(&fr);
    expectSliceEq(lines[0], actual);
    free(actual.data);

    AllocResult eof = fr_takeLineAlloc(&fr);
    ASSERT_EQ(eof.status, FileReader_ReadStatus_EOF);
    fr_close(&fr);
}

TEST(TakeLineAlloc, MultipleLines)
{
    const std::vector<std::string> lines = {"a\n", "bb\n", "ccc\n"};
    auto file = writeTempFile(
        std::accumulate(lines.begin(), lines.end(), std::string("")));
    FileReader fr = fr_open(file.c_str());

    for (const std::string& expected : lines) {
        AllocResult actual = fr_takeLineAlloc(&fr);
        expectSliceEq(expected, actual);
        free(actual.data);
    }

    AllocResult eof = fr_takeLineAlloc(&fr);
    ASSERT_EQ(eof.status, FileReader_ReadStatus_EOF);
    fr_close(&fr);
}

TEST(TakeLineAlloc, EmptyFile)
{
    auto file = writeTempFile("");
    FileReader fr = fr_open(file.c_str());

    AllocResult actual = fr_takeLineAlloc(&fr);
    ASSERT_EQ(actual.status, FileReader_ReadStatus_EOF);
    ASSERT_EQ(actual.data, nullptr);
    ASSERT_EQ(actual.len, 0u);

    fr_close(&fr);
}

TEST(TakeLineAlloc, EmptyLineAtStart)
{
    const std::vector<std::string> lines = {"\n", "hello\n"};
    auto file = writeTempFile(
        std::accumulate(lines.begin(), lines.end(), std::string("")));
    FileReader fr = fr_open(file.c_str());

    for (const std::string& expected : lines) {
        AllocResult actual = fr_takeLineAlloc(&fr);
        expectSliceEq(expected, actual);
        free(actual.data);
    }

    AllocResult eof = fr_takeLineAlloc(&fr);
    ASSERT_EQ(eof.status, FileReader_ReadStatus_EOF);
    fr_close(&fr);
}

// -------------------
// Very long line (multiple heap grows and reads)
// -------------------
TEST(TakeLineAlloc, LongLine)
{
    const size_t bigLen = std::max(5000, 3 * FILE_READER_BUFFER_SIZE / 2);
    std::string longLine(bigLen, 'x');
    longLine.push_back('\n');
    auto file = writeTempFile(longLine);
    FileReader fr = fr_open(file.c_str());

    AllocResult actual = fr_takeLineAlloc(&fr);
    expectSliceEq(longLine, actual);
    free(actual.data);

    AllocResult eof = fr_takeLineAlloc(&fr);
    ASSERT_EQ(eof.status, FileReader_ReadStatus_EOF);
    fr_close(&fr);
}

TEST(TakeLineAlloc, ConsecutiveEmptyLines)
{
    const std::vector<std::string> lines = {"\n", "\n", "\n"};
    auto file = writeTempFile(
        std::accumulate(lines.begin(), lines.end(), std::string("")));
    FileReader fr = fr_open(file.c_str());

    for (const std::string& expected : lines) {
        AllocResult actual = fr_takeLineAlloc(&fr);
        expectSliceEq(expected, actual);
        free(actual.data);
    }

    AllocResult eof = fr_takeLineAlloc(&fr);
    ASSERT_EQ(eof.status, FileReader_ReadStatus_EOF);
    fr_close(&fr);
}
