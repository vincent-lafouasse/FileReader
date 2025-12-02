#include "gtest/gtest.h"

extern "C" {
#include "options/Options.h"
}

TEST(ParseOptions, HeadlessFlag)
{
    const char* args[] = {"--headless", "--input", "file.wav"};
    const size_t sz = sizeof(args) / sizeof(*args);
    OptionsResult res = parseOptions(args, sz);

    ASSERT_EQ(res.err, NoError);
    EXPECT_TRUE(res.options.headless);
    EXPECT_STREQ(res.options.input, "file.wav");
}

TEST(ParseOptions, InputFlagLong)
{
    const char* args[] = {"--input", "file.wav"};
    const size_t sz = sizeof(args) / sizeof(*args);
    OptionsResult res = parseOptions(args, sz);

    ASSERT_EQ(res.err, NoError);
    EXPECT_FALSE(res.options.headless);
    EXPECT_STREQ(res.options.input, "file.wav");
}

TEST(ParseOptions, InputFlagShort)
{
    const char* args[] = {"-i", "file.wav"};
    const size_t sz = sizeof(args) / sizeof(*args);
    OptionsResult res = parseOptions(args, sz);

    ASSERT_EQ(res.err, NoError);
    EXPECT_FALSE(res.options.headless);
    EXPECT_STREQ(res.options.input, "file.wav");
}

TEST(ParseOptions, PositionalInput)
{
    const char* args[] = {"file.wav"};
    const size_t sz = sizeof(args) / sizeof(*args);
    OptionsResult res = parseOptions(args, sz);

    ASSERT_EQ(res.err, NoError);
    EXPECT_FALSE(res.options.headless);
    EXPECT_STREQ(res.options.input, "file.wav");
}

TEST(ParseOptions, FlagBeforePositional)
{
    const char* args[] = {"--headless", "file.wav"};
    const size_t sz = sizeof(args) / sizeof(*args);
    OptionsResult res = parseOptions(args, sz);

    ASSERT_EQ(res.err, NoError);
    EXPECT_TRUE(res.options.headless);
    EXPECT_STREQ(res.options.input, "file.wav");
}

TEST(ParseOptions, UnknownFlag)
{
    const char* args[] = {"--foo", "file.wav"};
    const size_t sz = sizeof(args) / sizeof(*args);
    OptionsResult res = parseOptions(args, sz);

    EXPECT_EQ(res.err, E_Unknown_Flag);
    EXPECT_STREQ(res.fault, "--foo");
}

TEST(ParseOptions, MissingInputForFlag)
{
    const char* args[] = {"--input"};
    const size_t sz = sizeof(args) / sizeof(*args);
    OptionsResult res = parseOptions(args, sz);

    EXPECT_EQ(res.err, E_Bad_Usage);
    EXPECT_STREQ(res.fault, "--input");
}

TEST(ParseOptions, ManyPositionals)
{
    const char* args[] = {"file1.wav", "file2.wav"};
    const size_t sz = sizeof(args) / sizeof(*args);
    OptionsResult res = parseOptions(args, sz);

    EXPECT_EQ(res.err, NoError);
    EXPECT_STREQ(res.options.input, "file2.wav");
}

TEST(ParseOptions, NoInputProvided)
{
    const char* args[] = {"--headless"};
    const size_t sz = sizeof(args) / sizeof(*args);
    OptionsResult res = parseOptions(args, sz);

    EXPECT_EQ(res.err, E_Bad_Usage);
}

TEST(ParseOptions, MixedFlagsAndPositionals)
{
    const char* args[] = {"file.wav", "--headless"};
    const size_t sz = sizeof(args) / sizeof(*args);
    OptionsResult res = parseOptions(args, sz);

    ASSERT_EQ(res.err, NoError);
    EXPECT_TRUE(res.options.headless);
    EXPECT_STREQ(res.options.input, "file.wav");
}
