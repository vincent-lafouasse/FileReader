#include "gtest/gtest.h"

extern "C" {
#include "int24.h"
}

// for convenience
int32_t i24(uint8_t high, uint8_t mid, uint8_t low)
{
    return i24_asI32(Int24{high, mid, low});
}

TEST(i24, Zero)
{
    EXPECT_EQ(i24(0x00, 0x00, 0x00), 0);
}

TEST(i24, PositiveSimpleValues)
{
    EXPECT_EQ(i24(0x00, 0x00, 0x01), 1);
    EXPECT_EQ(i24(0x00, 0x00, 0x7f), 127);
    EXPECT_EQ(i24(0x00, 0x01, 0x00), 256);
    EXPECT_EQ(i24(0x00, 0x10, 0x00), 4096);
}

TEST(i24, PositiveMax)
{
    EXPECT_EQ(i24(0x7f, 0xff, 0xff), 0x7fffff);
}

TEST(i24, NegativeSimpleValues)
{
    // -1 in i24 = 0xffffff
    EXPECT_EQ(i24(0xff, 0xff, 0xff), -1);

    // -128 in i24 = 0xffff80
    EXPECT_EQ(i24(0xff, 0xff, 0x80), -128);

    // -256 in i24 = 0xffff00
    EXPECT_EQ(i24(0xff, 0xff, 0x00), -256);
}

TEST(i24, NegativeMidRange)
{
    // -32768 = 0xff8000
    EXPECT_EQ(i24(0xff, 0x80, 0x00), -32768);

    // -1000000 = 0xf0bdc0
    EXPECT_EQ(i24(0xf0, 0xbd, 0xc0), -1000000);
}

TEST(i24, NegativeMin)
{
    // Minimum signed 24-bit value: 0x800000 = -8,388,608
    EXPECT_EQ(i24(0x80, 0x00, 0x00), -8388608);
}

// Boundary-crossing tests around sign bit
TEST(i24, SignBoundary)
{
    // Value just below sign bit (positive)
    EXPECT_EQ(i24(0x7f, 0xff, 0xfe), 8388606);

    // Value where sign bit just flips (negative)
    EXPECT_EQ(i24(0x80, 0x00, 0x01), -8388607);
}
