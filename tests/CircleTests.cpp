#include "core/BoundingBox.hpp"
#include "core/Circle.hpp"
#include "core/Coordinate.hpp"

#include "test_utils/matchers/BoundingBoxMatcher.hpp"
#include "test_utils/matchers/CircleMatcher.hpp"
#include "test_utils/matchers/CoordinateMatcher.hpp"

#include <gtest/gtest.h>

#include <optional>
#include <string>

TEST(CircleTest, boundingBox)
{
    // Arrange
    const Circle circle{
        .center =
            Coordinate{
                .x = 1.0,
                .y = 2.0,
            },
        .radius = Radius{ 3.0 },
    };

    // Act
    const BoundingBox bb{ boundingBox(circle) };

    // Assert
    const BoundingBox expectedBoundingBox{
        .min = Coordinate{ .x = -2.0, .y = -1.0 },
        .max = Coordinate{ .x = 4.0, .y = 5.0 },
    };

    EXPECT_THAT(bb, fcad::test_utils::IsBoundingBox(expectedBoundingBox));
}

struct CircleNearestPointParams
{
    Coordinate point;
    Coordinate expectedPoint;
};

class CircleNearestPointFixture : public testing::TestWithParam<CircleNearestPointParams>
{
};

TEST_P(CircleNearestPointFixture, NearestPoint)
{
    // Arrange
    const auto& [point, expectedPoint]{ GetParam() };

    const Circle circle{ .center = Coordinate{ .x = 1.0, .y = 2.0 }, .radius = Radius{ 3.0 } };

    // Act
    const Coordinate result{ nearestPoint(circle, point) };

    // Assert
    EXPECT_THAT(result, fcad::test_utils::IsCoordinate(expectedPoint));
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    CircleNearestPoint,
    CircleNearestPointFixture,
    testing::Values(
        CircleNearestPointParams
        {
            .point = Coordinate{ 1, 2 },
            .expectedPoint = Coordinate{ 4, 2 }
        },
        CircleNearestPointParams
        {
            .point = Coordinate{ 1, 5 },
            .expectedPoint = Coordinate{ 1, 5 }
        },
        CircleNearestPointParams
        {
            .point = Coordinate{ 1, 3},
            .expectedPoint = Coordinate{ 1, 5 }
        },CircleNearestPointParams
        {
            .point = Coordinate{ 1, 10},
            .expectedPoint = Coordinate{ 1, 5 }
        }
    )
);
// clang-format on

struct CircleFromPointsParams
{
    Coordinate first;
    Coordinate second;
    Coordinate third;
    std::optional<Circle> expectedCircle;
    std::string testCase;
};

struct CircleFromPointsTest : testing::TestWithParam<CircleFromPointsParams>
{
};

TEST_P(CircleFromPointsTest, FromPoints)
{
    // Arrange
    const auto& [first, second, third, expectedCircle, _]{ GetParam() };

    // Act
    const std::optional<Circle> maybeCircle{ circleFromPoints(first, second, third) };

    // Assert
    EXPECT_THAT(maybeCircle, fcad::test_utils::IsCircle(expectedCircle));
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    CircleFromPointsTests,
    CircleFromPointsTest,
    testing::Values(
        CircleFromPointsParams{
            .first = { 1.0, 1.0 },
            .second = { 1.0, 1.0 },
            .third = { 1.0, 1.0 },
            .expectedCircle = std::nullopt,
            .testCase = "all_identical"
        },
        CircleFromPointsParams{
            .first = { 1.0, 1.0 },
            .second = { 1.0, 1.0 },
            .third = { 2.0, 2.0 },
            .expectedCircle = std::nullopt,
            .testCase = "two_identical_1"
        },
        CircleFromPointsParams{
            .first = { 1.0, 1.0 },
            .second = { 2.0, 2.0 },
            .third = { 1.0, 1.0 },
            .expectedCircle = std::nullopt,
            .testCase = "two_identical_2"
        },
        CircleFromPointsParams{
            .first = { 1.0, 1.0 },
            .second = { 2.0, 2.0 },
            .third = { 2.0, 2.0 },
            .expectedCircle = std::nullopt,
            .testCase = "two_identical_3"
        },
        CircleFromPointsParams{
            .first = { 1.0, 1.0 },
            .second = { 2.0, 2.0 },
            .third = { 3.0, 3.0 },
            .expectedCircle = std::nullopt,
            .testCase = "collinear"
        },
        CircleFromPointsParams{
            .first = { 3.0, 2.0 },
            .second = { 1.0, 4.0 },
            .third = { 5.0, 4.0 },
            .expectedCircle = Circle{
                .center = { 3.0, 4.0 },
                .radius = Radius{ 2.0 }
            },
            .testCase = "valid_circle"
        }
    ),
    [](const testing::TestParamInfo<CircleFromPointsParams>& paramInfo) {
        return paramInfo.param.testCase;
    }
);
// clang-format on
