#include "domain/Layer.hpp"
#include "geo/BoundingBox.hpp"
#include "geo/Coordinate.hpp"

#include "test_utils/matchers/BoundingBoxMatcher.hpp"
#include "test_utils/matchers/LineMatcher.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <utility>

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

    layer.add(
        fcad::domain::Line{ fcad::geo::Line{
            .start = fcad::geo::Coordinate{ .x = 1.0, .y = 4.0 },
            .end = fcad::geo::Coordinate{ .x = 3.0, .y = 2.0 },
        } }
    );
    layer.add(
        fcad::domain::Circle{ fcad::geo::Circle{
            .center = { 2.0, 3.7 },
            .radius = fcad::geo::Radius{ 0.5 },
        } }
    );
    layer.add(
        fcad::domain::Arc{ fcad::geo::Arc{
            .center = { 3.0, 1.0 },
            .startAngle = fcad::core::Radians::zero,
            .endAngle = fcad::core::Radians::pi,
            .radius = fcad::geo::Radius{ 0.5 },
        } }
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

TEST(LayerTest, newLayersHaveDifferentIds)
{
    // Arrange
    const fcad::domain::Layer first{ "Layer Name" };
    const fcad::domain::Layer second{ "Layer Name" };

    // Assert
    EXPECT_NE(first.id(), second.id());
}

TEST(LayerTest, moveKeepsTheId)
{
    // Arrange
    fcad::domain::Layer layer{ "Layer Name" };
    const fcad::domain::LayerId id{ layer.id() };

    // Act
    const fcad::domain::Layer moved{ std::move(layer) };

    // Assert
    EXPECT_EQ(moved.id(), id);
}

TEST(LayerTest, cloneHasNewIdsAndTheSameContent)
{
    // Arrange
    const fcad::geo::Line geometry{
        .start = fcad::geo::Coordinate{ .x = 1.0, .y = 4.0 },
        .end = fcad::geo::Coordinate{ .x = 3.0, .y = 2.0 },
    };
    const fcad::domain::Color color{ fcad::domain::Rgb{ .r = 255, .g = 0, .b = 0 } };

    fcad::domain::Layer layer{ "Layer Name" };
    layer.setVisible(false);
    layer.setColor(color);
    layer.add(fcad::domain::Line{ geometry });
    layer.add(
        fcad::domain::Circle{ fcad::geo::Circle{
            .center = { 2.0, 3.7 },
            .radius = fcad::geo::Radius{ 0.5 },
        } }
    );
    layer.add(
        fcad::domain::Arc{ fcad::geo::Arc{
            .center = { 3.0, 1.0 },
            .startAngle = fcad::core::Radians::zero,
            .endAngle = fcad::core::Radians::pi,
            .radius = fcad::geo::Radius{ 0.5 },
        } }
    );

    // Act
    const fcad::domain::Layer clone{ layer.clone() };

    // Assert
    EXPECT_NE(clone.id(), layer.id());
    EXPECT_EQ(clone.name(), layer.name());
    EXPECT_FALSE(clone.isVisible());
    EXPECT_EQ(clone.color(), color);

    ASSERT_EQ(clone.lines().size(), 1);
    EXPECT_NE(clone.lines().front().id(), layer.lines().front().id());
    EXPECT_THAT(clone.lines().front().geometry(), fcad::test_utils::IsLine(geometry));

    ASSERT_EQ(clone.circles().size(), 1);
    EXPECT_NE(clone.circles().front().id(), layer.circles().front().id());

    ASSERT_EQ(clone.arcs().size(), 1);
    EXPECT_NE(clone.arcs().front().id(), layer.arcs().front().id());
}
