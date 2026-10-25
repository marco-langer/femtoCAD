#include "core/StrongUuid.hpp"

#include <QUuid>

#include <gtest/gtest.h>

#include <concepts>
#include <type_traits>

namespace {

using TestId = fcad::core::StrongUuid<struct TestIdTag>;
using OtherTestId = fcad::core::StrongUuid<struct OtherTestIdTag>;

}   // namespace

TEST(StrongUuid, generateCreatesRandomUuids)
{
    const TestId first{ TestId::generate() };
    const TestId second{ TestId::generate() };

    EXPECT_NE(first, second);
    EXPECT_EQ(first.value().version(), QUuid::Random);
    EXPECT_EQ(second.value().version(), QUuid::Random);
}

TEST(StrongUuid, comparesByValue)
{
    const QUuid uuid{ QUuid::createUuid() };

    EXPECT_EQ(TestId{ uuid }, TestId{ uuid });
    EXPECT_EQ(TestId{ uuid }.value(), uuid);
}

TEST(StrongUuid, differentTagsAreDifferentTypes)
{
    EXPECT_FALSE((std::is_convertible_v<TestId, OtherTestId>));
    EXPECT_FALSE((std::equality_comparable_with<TestId, OtherTestId>));
}
