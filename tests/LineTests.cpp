#include "core/BoundingBox.hpp"
#include "core/Coordinate.hpp"
#include "core/Line.hpp"

#include "Matchers/BoundingBoxMatcher.hpp"

#include <gtest/gtest.h>

TEST(LineTest, boundingBox)
{
    // Arrange
    const Line line{
        .start = Coordinate{ .x = 1.0, .y = 4.0 },
        .end = Coordinate{ .x = 3.0, .y = 2.0 },
    };

    // Act
    const BoundingBox bb{ boundingBox(line) };

    // Assert
    const BoundingBox expectedBb{
        .min = Coordinate{ .x = 1.0, .y = 2.0 },
        .max = Coordinate{ .x = 3.0, .y = 4.0 },
    };

    EXPECT_THAT(bb, IsBoundingBox(expectedBb));
}
