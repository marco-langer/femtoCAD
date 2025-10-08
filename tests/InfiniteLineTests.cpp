#include "core/InfiniteLine.hpp"

#include "core/Coordinate.hpp"

#include "Matchers/CoordinateMatcher.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <optional>
#include <string>

struct InfiniteLineTestParams
{
    InfiniteLine first;
    InfiniteLine second;
    std::optional<Coordinate> expectedIntersection;
    std::string testCase;
};

struct InfiniteLineTest : testing::TestWithParam<InfiniteLineTestParams>
{
};

TEST_P(InfiniteLineTest, Intersection)
{
    // Arrange
    const auto& [first, second, expectedIntersection, _]{ GetParam() };

    // Act
    const std::optional<Coordinate> maybeIntersection{ intersection(first, second) };

    // Assert
    EXPECT_THAT(maybeIntersection, IsCoordinate(expectedIntersection));
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    InfiniteLineTests,
    InfiniteLineTest,
    testing::Values(
        InfiniteLineTestParams{
            .first = InfiniteLine{
                .point = { 2.0, 2.0 },
                .direction = { 1.0, 0.0 }
            },
            .second= InfiniteLine{
                .point = { 2.0, 3.0 },
                .direction = { 1.0, 0.0 }
            },
            .expectedIntersection = std::nullopt,
            .testCase = "parallel"
        },
        InfiniteLineTestParams{
            .first = InfiniteLine{
                .point = { 2.0, 2.0 },
                .direction = { 1.0, 0.0 }
            },
            .second= InfiniteLine{
                .point = { 2.0, 2.0 },
                .direction = { 1.0, 0.0 }
            },
            .expectedIntersection = std::nullopt,
            .testCase = "coincident"
        },
        InfiniteLineTestParams{
            .first = InfiniteLine{
                .point = { 0.0, 2.0 },
                .direction = { 1.0, 0.0 }
            },
            .second= InfiniteLine{
                .point = { 1.0, 0.0 },
                .direction = { 0.0, 1.0 }
            },
            .expectedIntersection = Coordinate{ 1.0, 2.0 },
            .testCase = "normalized"
        }
    ),
    [](const testing::TestParamInfo<InfiniteLineTestParams>& paramInfo) {
        return paramInfo.param.testCase;
    }
);
// clang-format on
