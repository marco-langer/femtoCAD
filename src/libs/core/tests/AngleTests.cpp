#include "core/Angle.hpp"

#include <gtest/gtest.h>

#include <numbers>
#include <type_traits>

TEST(Angle, trivialDegree)
{
    EXPECT_TRUE(std::is_trivially_copyable_v<fcad::core::Degrees>);
    EXPECT_TRUE(std::is_trivially_move_constructible_v<fcad::core::Degrees>);
    EXPECT_TRUE(std::is_trivially_copy_constructible_v<fcad::core::Degrees>);

    const bool isTriviallyAssignable{
        std::is_trivially_assignable_v<fcad::core::Degrees, fcad::core::Degrees>
    };
    EXPECT_TRUE(isTriviallyAssignable);

    EXPECT_TRUE(std::is_trivially_copy_assignable_v<fcad::core::Degrees>);
    EXPECT_TRUE(std::is_trivially_move_assignable_v<fcad::core::Degrees>);

    EXPECT_TRUE(std::is_trivially_destructible_v<fcad::core::Degrees>);
}

TEST(Angle, trivialRadian)
{
    EXPECT_TRUE(std::is_trivially_copyable_v<fcad::core::Radians>);
    EXPECT_TRUE(std::is_trivially_move_constructible_v<fcad::core::Radians>);
    EXPECT_TRUE(std::is_trivially_copy_constructible_v<fcad::core::Radians>);

    const bool isTriviallyAssignable{
        std::is_trivially_assignable_v<fcad::core::Radians, fcad::core::Radians>
    };
    EXPECT_TRUE(isTriviallyAssignable);

    EXPECT_TRUE(std::is_trivially_copy_assignable_v<fcad::core::Radians>);
    EXPECT_TRUE(std::is_trivially_move_assignable_v<fcad::core::Radians>);

    EXPECT_TRUE(std::is_trivially_destructible_v<fcad::core::Radians>);
}

TEST(Angle, CopyConstructor)
{
    // Arrange
    fcad::core::Degrees angle1{ 90.0 };
    const fcad::core::Degrees angle2{ 90.0 };

    // Act
    const fcad::core::Degrees angle3{ angle1 };
    const fcad::core::Degrees angle4{ angle2 };

    // Assert
    EXPECT_EQ(angle3.value(), 90.0);
    EXPECT_EQ(angle4.value(), 90.0);
}

TEST(Angle, ConversionConstructorDegreeToRadian)
{
    // Arrange
    const fcad::core::Degrees angle1{ 90.0 };

    // Act
    const fcad::core::Radians angle2{ angle1 };

    // Assert
    EXPECT_EQ(angle2.value(), std::numbers::pi / 2.0);
}

TEST(Angle, ConversionConstructorRadianToDegree)
{
    // Arrange
    const fcad::core::Radians angle1{ std::numbers::pi };

    // Act
    const fcad::core::Degrees angle2{ angle1 };

    // Assert
    EXPECT_EQ(angle2.value(), 180.0);
}

TEST(Angle, DegreesfromDegrees)
{
    // Arrange && Act
    const fcad::core::Degrees angle{ fcad::core::Degrees::fromDegrees(90.0) };

    // Assert
    EXPECT_EQ(angle.value(), 90.0);
}

TEST(Angle, RadiansfromDegrees)
{
    // Arrange && Act
    const fcad::core::Radians angle{ fcad::core::Radians::fromDegrees(90.0) };

    // Assert
    EXPECT_EQ(angle.value(), std::numbers::pi / 2.0);
}

TEST(Angle, normalized)
{
    EXPECT_EQ(fcad::core::normalized<fcad::core::AngleUnit::Degree>(180.0), 180.0);
    EXPECT_EQ(fcad::core::normalized<fcad::core::AngleUnit::Degree>(400.0), 40.0);
    EXPECT_EQ(fcad::core::normalized<fcad::core::AngleUnit::Degree>(800.0), 80.0);
    EXPECT_EQ(fcad::core::normalized<fcad::core::AngleUnit::Degree>(-30.0), 330.0);
    EXPECT_EQ(fcad::core::normalized<fcad::core::AngleUnit::Degree>(-360.0), 360.0);
    EXPECT_EQ(fcad::core::normalized<fcad::core::AngleUnit::Degree>(-400.0), 320.0);
    EXPECT_EQ(fcad::core::normalized<fcad::core::AngleUnit::Degree>(-800.0), 280.0);

    constexpr double pi{ std::numbers::pi };
    EXPECT_EQ(fcad::core::normalized<fcad::core::AngleUnit::Radian>(pi), pi);
    EXPECT_EQ(fcad::core::normalized<fcad::core::AngleUnit::Radian>(2 * pi), 2 * pi);
    EXPECT_EQ(fcad::core::normalized<fcad::core::AngleUnit::Radian>(3 * pi), pi);
    EXPECT_EQ(fcad::core::normalized<fcad::core::AngleUnit::Radian>(5 * pi), pi);
    EXPECT_EQ(fcad::core::normalized<fcad::core::AngleUnit::Radian>(-pi), pi);
    EXPECT_EQ(fcad::core::normalized<fcad::core::AngleUnit::Radian>(-2 * pi), 2 * pi);
    EXPECT_EQ(fcad::core::normalized<fcad::core::AngleUnit::Radian>(-3 * pi), pi);
    EXPECT_EQ(fcad::core::normalized<fcad::core::AngleUnit::Radian>(-5 * pi), pi);
}