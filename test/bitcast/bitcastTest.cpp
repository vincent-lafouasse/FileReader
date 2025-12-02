#include "gtest/gtest.h"

extern "C" {
#include "bitcast.h"
}

using u16 = uint16_t;
using i16 = int16_t;
using u32 = uint32_t;

TEST(bitcast, U16_LE)
{
    {
        constexpr Byte b[2] = {0x00, 0x00};
        constexpr u16 expected = 0x0000;
        EXPECT_EQ(bitcastU16_LE(b), expected);
    }
    {
        constexpr Byte b[2] = {0x01, 0x00};
        constexpr u16 expected = 0x0001;
        EXPECT_EQ(bitcastU16_LE(b), expected);
    }
    {
        constexpr Byte b[2] = {0xff, 0x00};
        constexpr u16 expected = 0x00ff;
        EXPECT_EQ(bitcastU16_LE(b), expected);
    }
    {
        constexpr Byte b[2] = {0x34, 0x12};
        constexpr u16 expected = 0x1234;
        EXPECT_EQ(bitcastU16_LE(b), expected);
    }
    {
        constexpr Byte b[2] = {0xff, 0xff};
        constexpr u16 expected = 0xffff;
        EXPECT_EQ(bitcastU16_LE(b), expected);
    }
}

TEST(bitcast, U16_BE)
{
    {
        constexpr Byte b[2] = {0x00, 0x00};
        constexpr u16 expected = 0x0000;
        EXPECT_EQ(bitcastU16_BE(b), expected);
    }
    {
        constexpr Byte b[2] = {0x00, 0x01};
        constexpr u16 expected = 0x0001;
        EXPECT_EQ(bitcastU16_BE(b), expected);
    }
    {
        constexpr Byte b[2] = {0x00, 0xff};
        constexpr u16 expected = 0x00ff;
        EXPECT_EQ(bitcastU16_BE(b), expected);
    }
    {
        constexpr Byte b[2] = {0x12, 0x34};
        constexpr u16 expected = 0x1234;
        EXPECT_EQ(bitcastU16_BE(b), expected);
    }
    {
        constexpr Byte b[2] = {0xff, 0xff};
        constexpr u16 expected = 0xffff;
        EXPECT_EQ(bitcastU16_BE(b), expected);
    }
}

TEST(bitcast, U32_LE)
{
    {
        constexpr Byte b[4] = {0x00, 0x00, 0x00, 0x00};
        constexpr u32 expected = 0x00000000;
        EXPECT_EQ(bitcastU32_LE(b), expected);
    }
    {
        constexpr Byte b[4] = {0x01, 0x00, 0x00, 0x00};
        constexpr u32 expected = 0x00000001;
        EXPECT_EQ(bitcastU32_LE(b), expected);
    }
    {
        constexpr Byte b[4] = {0xfe, 0xca, 0xbe, 0xba};
        constexpr u32 expected = 0xbabecafe;
        EXPECT_EQ(bitcastU32_LE(b), expected);
    }
    {
        constexpr Byte b[4] = {0xff, 0xff, 0xff, 0xff};
        constexpr u32 expected = 0xffffffff;
        EXPECT_EQ(bitcastU32_LE(b), expected);
    }
    {
        constexpr Byte b[4] = {0x78, 0x56, 0x34, 0x12};
        constexpr u32 expected = 0x12345678;
        EXPECT_EQ(bitcastU32_LE(b), expected);
    }
}

TEST(bitcast, U32_BE)
{
    {
        constexpr Byte b[4] = {0x00, 0x00, 0x00, 0x00};
        constexpr u32 expected = 0x00000000;
        EXPECT_EQ(bitcastU32_BE(b), expected);
    }
    {
        constexpr Byte b[4] = {0x00, 0x00, 0x00, 0x01};
        constexpr u32 expected = 0x00000001;
        EXPECT_EQ(bitcastU32_BE(b), expected);
    }
    {
        constexpr Byte b[4] = {0xba, 0xbe, 0xca, 0xfe};
        constexpr u32 expected = 0xbabecafe;
        EXPECT_EQ(bitcastU32_BE(b), expected);
    }
    {
        constexpr Byte b[4] = {0xff, 0xff, 0xff, 0xff};
        constexpr u32 expected = 0xffffffff;
        EXPECT_EQ(bitcastU32_BE(b), expected);
    }
    {
        constexpr Byte b[4] = {0x12, 0x34, 0x56, 0x78};
        constexpr u32 expected = 0x12345678;
        EXPECT_EQ(bitcastU32_BE(b), expected);
    }
}

TEST(bitcast, I16_LE)
{
    {
        constexpr Byte b[2] = {0x00, 0x00};
        constexpr i16 expected = 0x0000;
        EXPECT_EQ(bitcastI16_LE(b), expected);
    }
    {
        constexpr Byte b[2] = {0x01, 0x00};
        constexpr i16 expected = 0x0001;
        EXPECT_EQ(bitcastI16_LE(b), expected);
    }
    {
        constexpr Byte b[2] = {0xff, 0x7f};
        constexpr i16 expected = INT16_MAX;
        EXPECT_EQ(bitcastI16_LE(b), expected);
    }
    {
        constexpr Byte b[2] = {0x00, 0x80};
        constexpr i16 expected = INT16_MIN;
        EXPECT_EQ(bitcastI16_LE(b), expected);
    }
    {
        constexpr Byte b[2] = {0xfe, 0xca};
        constexpr i16 expected = static_cast<i16>(0xcafe);
        EXPECT_EQ(bitcastI16_LE(b), expected);
    }
    {
        constexpr Byte b[2] = {0xff, 0xff};
        constexpr i16 expected = -1;
        EXPECT_EQ(bitcastI16_LE(b), expected);
    }
}

TEST(bitcast, I16_BE)
{
    {
        constexpr Byte b[2] = {0x00, 0x00};
        constexpr i16 expected = 0x0000;
        EXPECT_EQ(bitcastI16_BE(b), expected);
    }
    {
        constexpr Byte b[2] = {0x00, 0x01};
        constexpr i16 expected = 0x0001;
        EXPECT_EQ(bitcastI16_BE(b), expected);
    }
    {
        constexpr Byte b[2] = {0x7f, 0xff};
        constexpr i16 expected = INT16_MAX;
        EXPECT_EQ(bitcastI16_BE(b), expected);
    }
    {
        constexpr Byte b[2] = {0x80, 0x00};
        constexpr i16 expected = INT16_MIN;
        EXPECT_EQ(bitcastI16_BE(b), expected);
    }
    {
        constexpr Byte b[2] = {0xca, 0xfe};
        constexpr i16 expected = static_cast<i16>(0xcafe);
        EXPECT_EQ(bitcastI16_BE(b), expected);
    }
    {
        constexpr Byte b[2] = {0xff, 0xff};
        constexpr i16 expected = -1;
        EXPECT_EQ(bitcastI16_BE(b), expected);
    }
}

TEST(bitcast, I32_LE)
{
    {
        constexpr Byte b[4] = {0x00, 0x00, 0x00, 0x00};
        constexpr int32_t expected = 0x00000000;
        EXPECT_EQ(bitcastI32_LE(b), expected);
    }
    {
        constexpr Byte b[4] = {0x01, 0x00, 0x00, 0x00};
        constexpr int32_t expected = 0x00000001;
        EXPECT_EQ(bitcastI32_LE(b), expected);
    }
    {
        constexpr Byte b[4] = {0xff, 0xff, 0xff, 0x7f};
        constexpr int32_t expected = INT32_MAX;
        EXPECT_EQ(bitcastI32_LE(b), expected);
    }
    {
        constexpr Byte b[4] = {0x00, 0x00, 0x00, 0x80};
        constexpr int32_t expected = INT32_MIN;
        EXPECT_EQ(bitcastI32_LE(b), expected);
    }
    {
        constexpr Byte b[4] = {0xfe, 0xca, 0xbe, 0xba};
        constexpr int32_t expected = static_cast<int32_t>(0xbabecafe);
        EXPECT_EQ(bitcastI32_LE(b), expected);
    }
    {
        constexpr Byte b[4] = {0xff, 0xff, 0xff, 0xff};
        constexpr int32_t expected = -1;
        EXPECT_EQ(bitcastI32_LE(b), expected);
    }
}

TEST(bitcast, I32_BE)
{
    {
        constexpr Byte b[4] = {0x00, 0x00, 0x00, 0x00};
        constexpr int32_t expected = 0x00000000;
        EXPECT_EQ(bitcastI32_BE(b), expected);
    }
    {
        constexpr Byte b[4] = {0x00, 0x00, 0x00, 0x01};
        constexpr int32_t expected = 0x00000001;
        EXPECT_EQ(bitcastI32_BE(b), expected);
    }
    {
        constexpr Byte b[4] = {0x7f, 0xff, 0xff, 0xff};
        constexpr int32_t expected = INT32_MAX;
        EXPECT_EQ(bitcastI32_BE(b), expected);
    }
    {
        constexpr Byte b[4] = {0x80, 0x00, 0x00, 0x00};
        constexpr int32_t expected = INT32_MIN;
        EXPECT_EQ(bitcastI32_BE(b), expected);
    }
    {
        constexpr Byte b[4] = {0xba, 0xbe, 0xca, 0xfe};
        constexpr int32_t expected = static_cast<int32_t>(0xbabecafe);
        EXPECT_EQ(bitcastI32_BE(b), expected);
    }
    {
        constexpr Byte b[4] = {0xff, 0xff, 0xff, 0xff};
        constexpr int32_t expected = -1;
        EXPECT_EQ(bitcastI32_BE(b), expected);
    }
}
