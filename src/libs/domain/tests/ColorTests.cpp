#include "domain/Color.hpp"

#include <gtest/gtest.h>

namespace {

const fcad::domain::Color red{ fcad::domain::Rgb{ .r = 255, .g = 0, .b = 0 } };
const fcad::domain::Color blue{ fcad::domain::Rgb{ .r = 0, .g = 0, .b = 255 } };
const fcad::domain::Color adaptive{ fcad::domain::Adaptive{} };

}   // namespace

TEST(ColorTest, resolvedByLayerIsTheLayerColor)
{
    EXPECT_EQ(fcad::domain::resolved(fcad::domain::ByLayer{}, red), red);
    EXPECT_EQ(fcad::domain::resolved(fcad::domain::ByLayer{}, adaptive), adaptive);
}

TEST(ColorTest, resolvedOwnColorIgnoresTheLayerColor)
{
    EXPECT_EQ(fcad::domain::resolved(fcad::domain::Rgb{ .r = 0, .g = 0, .b = 255 }, red), blue);
    EXPECT_EQ(fcad::domain::resolved(fcad::domain::Adaptive{}, red), adaptive);
}
