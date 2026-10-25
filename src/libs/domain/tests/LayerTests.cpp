#include "domain/Layer.hpp"
#include "geo/BoundingBox.hpp"
#include "geo/Coordinate.hpp"

#include "test_utils/matchers/BoundingBoxMatcher.hpp"

#include <gtest/gtest.h>

TEST(LayerTest, boundingBox_Empty)
{
    // Arrange
    const fcad::domain::Layer layer{ "Layer Name" };

    // Act
    const std::optional<fcad::geo::BoundingBox> bb{ fcad::domain::boundingBox(layer) };

    // Assert
    EXPECT_FALSE(bb.has_value());
}

TEST(LayerTest, boundingBox)
{
    // Arrange
    fcad::domain::Layer layer{ "Layer Name" };

    layer.lines.emplace_back(
        fcad::geo::Line{
            .start = fcad::geo::Coordinate{ .x = 1.0, .y = 4.0 },
            .end = fcad::geo::Coordinate{ .x = 3.0, .y = 2.0 },
        }
    );
    layer.circles.emplace_back(
        fcad::geo::Circle{
            .center = { 2.0, 3.7 },
            .radius = fcad::geo::Radius{ 0.5 },
        }
    );
    layer.arcs.emplace_back(
        fcad::geo::Arc{
            .center = { 3.0, 1.0 },
            .startAngle = fcad::core::Radians::zero,
            .endAngle = fcad::core::Radians::pi,
            .radius = fcad::geo::Radius{ 0.5 },
        }
    );

    // Act
    const std::optional<fcad::geo::BoundingBox> bb{ fcad::domain::boundingBox(layer) };

    // Assert
    ASSERT_TRUE(bb.has_value());

    const fcad::geo::BoundingBox expectedBb{
        .min = fcad::geo::Coordinate{ .x = 1.0, .y = 1.0 },
        .max = fcad::geo::Coordinate{ .x = 3.5, .y = 4.2 },
    };

    EXPECT_THAT(bb.value(), fcad::test_utils::IsBoundingBox(expectedBb));
}
