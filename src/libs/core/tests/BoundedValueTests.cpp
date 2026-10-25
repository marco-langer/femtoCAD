#include "core/BoundedValue.hpp"

#include <gtest/gtest.h>

#include <optional>
#include <type_traits>
#include <utility>

namespace {

using SUTBoundedValue = fcad::core::BoundedValue<int, -2, 10, struct SUTTag>;

}   // namespace

TEST(BoundedValue, trivial)
{
    EXPECT_TRUE(std::is_trivially_copyable_v<SUTBoundedValue>);
    EXPECT_TRUE(std::is_trivially_destructible_v<SUTBoundedValue>);
    EXPECT_FALSE(std::is_default_constructible_v<SUTBoundedValue>);
}

TEST(BoundedValue, bounds)
{
    EXPECT_EQ(SUTBoundedValue::min, -2);
    EXPECT_EQ(SUTBoundedValue::max, 10);
}

TEST(BoundedValue, constructor)
{
    const SUTBoundedValue sut{ 7 };

    EXPECT_EQ(sut.value(), 7);
}

TEST(BoundedValue, comparesByValue)
{
    EXPECT_EQ(SUTBoundedValue{ 3 }, SUTBoundedValue{ 3 });
    EXPECT_LT(SUTBoundedValue{ -2 }, SUTBoundedValue{ 3 });
    EXPECT_GT(SUTBoundedValue{ 10 }, SUTBoundedValue{ 3 });
}

class BoundedValueFromValueTest : public testing::TestWithParam<std::pair<int, bool>>
{
};

TEST_P(BoundedValueFromValueTest, fromValue)
{
    // Arrange
    const auto& [value, expectedSuccess]{ GetParam() };

    // Act
    const std::optional<SUTBoundedValue> sut{ SUTBoundedValue::fromValue(value) };

    // Assert
    ASSERT_EQ(sut.has_value(), expectedSuccess);
    if (sut.has_value()) {
        EXPECT_EQ(sut->value(), value);
    }
}

INSTANTIATE_TEST_SUITE_P(
    BoundedValueFromValueTests,
    BoundedValueFromValueTest,
    testing::Values(
        std::pair{ -3, false },
        std::pair{ -2, true },
        std::pair{ 0, true },
        std::pair{ 10, true },
        std::pair{ 11, false }
    )
);

class BoundedValueClampedTest : public testing::TestWithParam<std::pair<int, int>>
{
};

TEST_P(BoundedValueClampedTest, clamped)
{
    // Arrange
    const auto& [value, expectedValue]{ GetParam() };

    // Act
    const SUTBoundedValue sut{ SUTBoundedValue::clamped(value) };

    // Assert
    EXPECT_EQ(sut.value(), expectedValue);
}

INSTANTIATE_TEST_SUITE_P(
    BoundedValueClampedTests,
    BoundedValueClampedTest,
    testing::Values(
        std::pair{ -100, -2 },
        std::pair{ -2, -2 },
        std::pair{ 5, 5 },
        std::pair{ 10, 10 },
        std::pair{ 100, 10 }
    )
);
