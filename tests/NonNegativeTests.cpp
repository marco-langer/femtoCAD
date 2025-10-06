#include "core/primitives/NonNegative.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include <stdexcept>

namespace {

using SUTNonNegative = NonNegative<double, struct SUTTag>;

}   // namespace

TEST(NonNegative, trivial)
{
    EXPECT_TRUE(std::is_trivially_copyable_v<SUTNonNegative>);
    EXPECT_TRUE(std::is_trivially_move_constructible_v<SUTNonNegative>);
    EXPECT_TRUE(std::is_trivially_copy_constructible_v<SUTNonNegative>);

    const bool isTriviallyAssignable{
        std::is_trivially_assignable_v<SUTNonNegative, SUTNonNegative>
    };
    EXPECT_TRUE(isTriviallyAssignable);

    EXPECT_TRUE(std::is_trivially_copy_assignable_v<SUTNonNegative>);
    EXPECT_TRUE(std::is_trivially_move_assignable_v<SUTNonNegative>);

    EXPECT_TRUE(std::is_trivially_destructible_v<SUTNonNegative>);
}

TEST(NonNegative, ConstructorSuccess)
{
    const SUTNonNegative sut{ 1.5 };

    EXPECT_EQ(sut.value(), 1.5);
}

TEST(NonNegative, ConstructorFailure)
{
    EXPECT_THROW({ SUTNonNegative sut{ -1.5 }; }, std::invalid_argument);
}

class NonNegativeFromValueTest : public testing::TestWithParam<std::pair<double, bool>>
{
};

TEST_P(NonNegativeFromValueTest, fromValue)
{
    // Arrange
    const auto& [value, expectedSuccess]{ GetParam() };

    // Act
    std::optional<SUTNonNegative> sut{ SUTNonNegative::fromValue(value) };

    // Assert
    EXPECT_EQ(sut.has_value(), expectedSuccess);
    if (sut.has_value() && !std::isnan(sut.value())) {
        EXPECT_EQ(sut.value(), value);
    }
}

INSTANTIATE_TEST_SUITE_P(
    NonNegativeFromValueTests,
    NonNegativeFromValueTest,
    testing::Values(
        std::pair{ 1.5, true },
        std::pair{ -1.5, false },
        std::pair{ 0.0, true },
        std::pair{ -std::numeric_limits<double>::infinity(), false },
        std::pair{ -std::numeric_limits<double>::quiet_NaN(), false }
    )
);
