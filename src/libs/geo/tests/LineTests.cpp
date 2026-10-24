#include "geo/BoundingBox.hpp"
#include "geo/Coordinate.hpp"
#include "geo/Line.hpp"

#include "test_utils/matchers/BoundingBoxMatcher.hpp"

#include <gtest/gtest.h>

TEST(LineTest, boundingBox)
{
    // Arrange
    const fcad::geo::Line line{
        .start = fcad::geo::Coordinate{ .x = 1.0, .y = 4.0 },
        .end = fcad::geo::Coordinate{ .x = 3.0, .y = 2.0 },
    };

    // Act
    const fcad::geo::BoundingBox bb{ fcad::geo::boundingBox(line) };

    // Assert
    const fcad::geo::BoundingBox expectedBb{
        .min = fcad::geo::Coordinate{ .x = 1.0, .y = 2.0 },
        .max = fcad::geo::Coordinate{ .x = 3.0, .y = 4.0 },
    };

    EXPECT_THAT(bb, fcad::test_utils::IsBoundingBox(expectedBb));
}
