#include "core/Positive.hpp"

#include <gtest/gtest.h>

#include <limits>
#include <optional>
#include <type_traits>
#include <utility>

namespace {

using SUTPositive = fcad::core::Positive<double, struct SUTTag>;

}   // namespace

TEST(Positive, trivial)
{
    EXPECT_TRUE(std::is_trivially_copyable_v<SUTPositive>);
    EXPECT_TRUE(std::is_trivially_destructible_v<SUTPositive>);
    EXPECT_FALSE(std::is_default_constructible_v<SUTPositive>);
}

TEST(Positive, constructor)
{
    const SUTPositive sut{ 2.5 };

    EXPECT_EQ(sut.value(), 2.5);
}

TEST(Positive, comparesByValue)
{
    EXPECT_EQ(SUTPositive{ 1.5 }, SUTPositive{ 1.5 });
    EXPECT_LT(SUTPositive{ 0.5 }, SUTPositive{ 1.5 });
}

class PositiveFromValueTest : public testing::TestWithParam<std::pair<double, bool>>
{
};

TEST_P(PositiveFromValueTest, fromValue)
{
    // Arrange
    const auto& [value, expectedSuccess]{ GetParam() };

    // Act
    const std::optional<SUTPositive> sut{ SUTPositive::fromValue(value) };

    // Assert
    ASSERT_EQ(sut.has_value(), expectedSuccess);
    if (sut.has_value()) {
        EXPECT_EQ(sut->value(), value);
    }
}

INSTANTIATE_TEST_SUITE_P(
    PositiveFromValueTests,
    PositiveFromValueTest,
    testing::Values(
        std::pair{ 1.5, true },
        std::pair{ std::numeric_limits<double>::denorm_min(), true },
        std::pair{ 0.0, false },
        std::pair{ -0.0, false },
        std::pair{ -1.5, false },
        std::pair{ std::numeric_limits<double>::infinity(), false },
        std::pair{ std::numeric_limits<double>::quiet_NaN(), false }
    )
);
