#include "DxfColors.hpp"

#include "domain/Color.hpp"

#include <gtest/gtest.h>

#include <limits>
#include <optional>

TEST(DxfColorsTest, ToColorIndex)
{
    const fcad::domain::Color red{ fcad::domain::Rgb{ .r = 255, .g = 0, .b = 0 } };
    const fcad::domain::Color blue{ fcad::domain::Rgb{ .r = 0, .g = 0, .b = 255 } };
    const fcad::domain::Color darkRed{ fcad::domain::Rgb{ .r = 0xA5, .g = 0x00, .b = 0x00 } };
    const fcad::domain::Color white{ fcad::domain::Rgb{ .r = 255, .g = 255, .b = 255 } };

    EXPECT_EQ(fcad::io::toColor(1, std::nullopt), red);
    EXPECT_EQ(fcad::io::toColor(5, std::nullopt), blue);
    EXPECT_EQ(fcad::io::toColor(12, std::nullopt), darkRed);
    EXPECT_EQ(fcad::io::toColor(255, std::nullopt), white);
}

TEST(DxfColorsTest, ToColorIndexSevenIsAdaptive)
{
    const fcad::domain::Color adaptive{ fcad::domain::Adaptive{} };

    EXPECT_EQ(fcad::io::toColor(7, std::nullopt), adaptive);
}

TEST(DxfColorsTest, ToColorInvalidIndexIsAdaptive)
{
    const fcad::domain::Color adaptive{ fcad::domain::Adaptive{} };

    EXPECT_EQ(fcad::io::toColor(0, std::nullopt), adaptive);
    EXPECT_EQ(fcad::io::toColor(256, std::nullopt), adaptive);
    EXPECT_EQ(fcad::io::toColor(-1, std::nullopt), adaptive);
    EXPECT_EQ(fcad::io::toColor(std::numeric_limits<int>::min(), std::nullopt), adaptive);
    EXPECT_EQ(fcad::io::toColor(std::numeric_limits<int>::max(), std::nullopt), adaptive);
}

TEST(DxfColorsTest, ToColorTrueColorTakesPrecedence)
{
    const fcad::domain::Color expected{ fcad::domain::Rgb{ .r = 0x30, .g = 0x50, .b = 0xA0 } };

    EXPECT_EQ(fcad::io::toColor(5, 0x3050A0), expected);
    EXPECT_EQ(fcad::io::toColor(7, 0x3050A0), expected);
    EXPECT_EQ(fcad::io::toColor(0, 0x3050A0), expected);
}

TEST(DxfColorsTest, ToColorTrueColorUsesLow24Bits)
{
    const fcad::domain::Color expected{ fcad::domain::Rgb{ .r = 0x30, .g = 0x50, .b = 0xA0 } };
    const fcad::domain::Color white{ fcad::domain::Rgb{ .r = 255, .g = 255, .b = 255 } };

    EXPECT_EQ(fcad::io::toColor(5, 0x7F3050A0), expected);
    EXPECT_EQ(fcad::io::toColor(5, -1), white);
}
