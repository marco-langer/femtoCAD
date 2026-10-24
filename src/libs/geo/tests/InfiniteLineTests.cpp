#include "geo/InfiniteLine.hpp"

#include "geo/Coordinate.hpp"

#include "test_utils/matchers/CoordinateMatcher.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <optional>
#include <string>

struct InfiniteLineTestParams
{
    fcad::geo::InfiniteLine first;
    fcad::geo::InfiniteLine second;
    std::optional<fcad::geo::Coordinate> expectedIntersection;
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
    const std::optional<fcad::geo::Coordinate> maybeIntersection{ intersection(first, second) };

    // Assert
    EXPECT_THAT(maybeIntersection, fcad::test_utils::IsCoordinate(expectedIntersection));
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    InfiniteLineTests,
    InfiniteLineTest,
    testing::Values(
        InfiniteLineTestParams{
            .first = fcad::geo::InfiniteLine{
                .point = { 2.0, 2.0 },
                .direction = { 1.0, 0.0 }
            },
            .second= fcad::geo::InfiniteLine{
                .point = { 2.0, 3.0 },
                .direction = { 1.0, 0.0 }
            },
            .expectedIntersection = std::nullopt,
            .testCase = "parallel"
        },
        InfiniteLineTestParams{
            .first = fcad::geo::InfiniteLine{
                .point = { 2.0, 2.0 },
                .direction = { 1.0, 0.0 }
            },
            .second= fcad::geo::InfiniteLine{
                .point = { 2.0, 2.0 },
                .direction = { 1.0, 0.0 }
            },
            .expectedIntersection = std::nullopt,
            .testCase = "coincident"
        },
        InfiniteLineTestParams{
            .first = fcad::geo::InfiniteLine{
                .point = { 0.0, 2.0 },
                .direction = { 1.0, 0.0 }
            },
            .second= fcad::geo::InfiniteLine{
                .point = { 1.0, 0.0 },
                .direction = { 0.0, 1.0 }
            },
            .expectedIntersection = fcad::geo::Coordinate{ 1.0, 2.0 },
            .testCase = "normalized"
        }
    ),
    [](const testing::TestParamInfo<InfiniteLineTestParams>& paramInfo) {
        return paramInfo.param.testCase;
    }
);
// clang-format on
