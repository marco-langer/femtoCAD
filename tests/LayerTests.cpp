#include "core/BoundingBox.hpp"
#include "core/Coordinate.hpp"
#include "core/Layer.hpp"

#include "Matchers/BoundingBoxMatcher.hpp"

#include <gtest/gtest.h>

TEST(LayerTest, boundingBox_Empty)
{
    // Arrange
    const Layer layer{ "Layer Name" };

    // Act
    const std::optional<BoundingBox> bb{ boundingBox(layer) };

    // Assert
    EXPECT_FALSE(bb.has_value());
}

TEST(LayerTest, boundingBox)
{
    // Arrange
    Layer layer{ "Layer Name" };

    layer.lines.push_back(Line{
        .start = Coordinate{ .x = 1.0, .y = 4.0 },
        .end = Coordinate{ .x = 3.0, .y = 2.0 },
    });
    layer.circles.push_back(Circle{
        .center = { 2.0, 3.7 },
        .radius = 0.5,
    });

    // Act
    const std::optional<BoundingBox> bb{ boundingBox(layer) };

    // Assert
    ASSERT_TRUE(bb.has_value());

    const BoundingBox expectedBb{
        .min = Coordinate{ .x = 1.0, .y = 2.0 },
        .max = Coordinate{ .x = 3.0, .y = 4.2 },
    };

    EXPECT_THAT(bb.value(), IsBoundingBox(expectedBb));
}
