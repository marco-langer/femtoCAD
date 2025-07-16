#include "core/BoundingBox.hpp"
#include "core/Circle.hpp"
#include "core/Coordinate.hpp"

#include "Matchers/BoundingBoxMatcher.hpp"
#include "Matchers/CoordinateMatcher.hpp"

#include <gtest/gtest.h>

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

    EXPECT_THAT(bb, IsBoundingBox(expectedBoundingBox));
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
    EXPECT_THAT(result, IsCoordinate(expectedPoint));
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
