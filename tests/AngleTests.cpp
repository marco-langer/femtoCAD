#include "core/units/Angle.hpp"

#include <gtest/gtest.h>

#include <numbers>
#include <type_traits>

TEST(Angle, trivialDegree)
{
    EXPECT_TRUE(std::is_trivially_copyable_v<Degrees>);
    EXPECT_TRUE(std::is_trivially_move_constructible_v<Degrees>);
    EXPECT_TRUE(std::is_trivially_copy_constructible_v<Degrees>);

    const bool isTriviallyAssignable{ std::is_trivially_assignable_v<Degrees, Degrees> };
    EXPECT_TRUE(isTriviallyAssignable);

    EXPECT_TRUE(std::is_trivially_copy_assignable_v<Degrees>);
    EXPECT_TRUE(std::is_trivially_move_assignable_v<Degrees>);

    EXPECT_TRUE(std::is_trivially_destructible_v<Degrees>);
}

TEST(Angle, trivialRadian)
{
    EXPECT_TRUE(std::is_trivially_copyable_v<Radians>);
    EXPECT_TRUE(std::is_trivially_move_constructible_v<Radians>);
    EXPECT_TRUE(std::is_trivially_copy_constructible_v<Radians>);

    const bool isTriviallyAssignable{ std::is_trivially_assignable_v<Radians, Radians> };
    EXPECT_TRUE(isTriviallyAssignable);

    EXPECT_TRUE(std::is_trivially_copy_assignable_v<Radians>);
    EXPECT_TRUE(std::is_trivially_move_assignable_v<Radians>);

    EXPECT_TRUE(std::is_trivially_destructible_v<Radians>);
}

TEST(Angle, CopyConstructor)
{
    // Arrange
    Degrees angle1{ 90.0 };
    const Degrees angle2{ 90.0 };

    // Act
    const Degrees angle3{ angle1 };
    const Degrees angle4{ angle2 };

    // Assert
    EXPECT_EQ(angle3.value(), 90.0);
    EXPECT_EQ(angle4.value(), 90.0);
}

TEST(Angle, ConversionConstructorDegreeToRadian)
{
    // Arrange
    const Degrees angle1{ 90.0 };

    // Act
    const Radians angle2{ angle1 };

    // Assert
    EXPECT_EQ(angle2.value(), std::numbers::pi / 2.0);
}

TEST(Angle, ConversionConstructorRadianToDegree)
{
    // Arrange
    const Radians angle1{ std::numbers::pi };

    // Act
    const Degrees angle2{ angle1 };

    // Assert
    EXPECT_EQ(angle2.value(), 180.0);
}

TEST(Angle, DegreesfromDegrees)
{
    // Arrange && Act
    const Degrees angle{ Degrees::fromDegrees(90.0) };

    // Assert
    EXPECT_EQ(angle.value(), 90.0);
}

TEST(Angle, RadiansfromDegrees)
{
    // Arrange && Act
    const Radians angle{ Radians::fromDegrees(90.0) };

    // Assert
    EXPECT_EQ(angle.value(), std::numbers::pi / 2.0);
}

TEST(Angle, normalized)
{
    EXPECT_EQ(normalized<AngleUnit::Degree>(180.0), 180.0);
    EXPECT_EQ(normalized<AngleUnit::Degree>(400.0), 40.0);
    EXPECT_EQ(normalized<AngleUnit::Degree>(800.0), 80.0);
    EXPECT_EQ(normalized<AngleUnit::Degree>(-30.0), 330.0);
    EXPECT_EQ(normalized<AngleUnit::Degree>(-360.0), 360.0);
    EXPECT_EQ(normalized<AngleUnit::Degree>(-400.0), 320.0);
    EXPECT_EQ(normalized<AngleUnit::Degree>(-800.0), 280.0);

    constexpr double pi{ std::numbers::pi };
    EXPECT_EQ(normalized<AngleUnit::Radian>(pi), pi);
    EXPECT_EQ(normalized<AngleUnit::Radian>(2 * pi), 2 * pi);
    EXPECT_EQ(normalized<AngleUnit::Radian>(3 * pi), pi);
    EXPECT_EQ(normalized<AngleUnit::Radian>(5 * pi), pi);
    EXPECT_EQ(normalized<AngleUnit::Radian>(-pi), pi);
    EXPECT_EQ(normalized<AngleUnit::Radian>(-2 * pi), 2 * pi);
    EXPECT_EQ(normalized<AngleUnit::Radian>(-3 * pi), pi);
    EXPECT_EQ(normalized<AngleUnit::Radian>(-5 * pi), pi);
}