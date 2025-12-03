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
    ASSERT_EQ(actual.err, Read_Ok);
    ASSERT_NE(actual.data, nullptr);
    ASSERT_EQ(actual.len, expected.size());
    ASSERT_EQ(std::memcmp(actual.data, expected.c_str(), expected.size()), 0);
}
}  // namespace

TEST(TakeLine, Simple)
{
    const std::vector<std::string> lines = {
        "aaa\n",
    };
    auto file = writeTempFile(
        std::accumulate(lines.begin(), lines.end(), std::string("")));
    FileReader fr = fr_open(file.c_str());

    for (const std::string& expected : lines) {
        AllocResult actual = fr_takeLineAlloc(&fr);
        expectSliceEq(expected, actual);
        free(actual.data);
    }

    fr_close(&fr);
}
