#include "core/primitives/NonNegative.hpp"

#include <gtest/gtest.h>

#include <stdexcept>

namespace {

using SUTNonNegative = NonNegative<double, struct SUTTag>;

}   // namespace

TEST(NonNegative, trivial)
{
    EXPECT_TRUE(std::is_trivially_copyable_v<SUTNonNegative>);
    EXPECT_TRUE(std::is_trivially_move_constructible_v<SUTNonNegative>);
    EXPECT_TRUE(std::is_trivially_copy_constructible_v<SUTNonNegative>);

    const bool isTriviallyAssignable{ std::is_trivially_assignable_v<SUTNonNegative, SUTNonNegative> };
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

TEST(NonNegative, fromValueSuccess)
{
    std::optional<SUTNonNegative> sut{ SUTNonNegative::fromValue(1.5) };

    ASSERT_TRUE(sut.has_value());
    EXPECT_EQ(sut.value(), 1.5);
}

TEST(NonNegative, fromValueFailure)
{
    std::optional<SUTNonNegative> sut{ SUTNonNegative::fromValue(-1.5) };

    EXPECT_FALSE(sut.has_value());
}
