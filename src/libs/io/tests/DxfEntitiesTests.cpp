#include "DxfEntities.hpp"

#include "core/Angle.hpp"
#include "domain/Color.hpp"
#include "domain/Layer.hpp"
#include "geo/Arc.hpp"
#include "geo/Circle.hpp"
#include "geo/Line.hpp"

#include "test_utils/matchers/ArcMatcher.hpp"
#include "test_utils/matchers/CircleMatcher.hpp"
#include "test_utils/matchers/LineMatcher.hpp"

#include <opendxf/entities.hpp>
#include <opendxf/layer.hpp>

#include <gtest/gtest.h>

#include <limits>
#include <optional>

TEST(DxfEntitiesTest, ToLayer)
{
    // Arrange
    const odxf::Layer dxfLayer{ .name = "Walls" };

    // Act
    const fcad::domain::Layer layer{ fcad::io::toLayer(dxfLayer) };

    // Assert
    EXPECT_EQ(layer.name(), "Walls");
    EXPECT_TRUE(layer.isVisible());
    EXPECT_EQ(layer.color(), fcad::domain::Color{ fcad::domain::Adaptive{} });
}

TEST(DxfEntitiesTest, ToLayerColor)
{
    // Arrange
    const odxf::Layer dxfLayer{ .name = "Walls", .color = 1 };

    // Act
    const fcad::domain::Layer layer{ fcad::io::toLayer(dxfLayer) };

    // Assert
    const fcad::domain::Color expectedColor{ fcad::domain::Rgb{ .r = 255, .g = 0, .b = 0 } };
    EXPECT_TRUE(layer.isVisible());
    EXPECT_EQ(layer.color(), expectedColor);
}

TEST(DxfEntitiesTest, ToLayerTrueColor)
{
    // Arrange
    const odxf::Layer dxfLayer{ .name = "Walls", .color = 1, .trueColor = 0x3050A0 };

    // Act
    const fcad::domain::Layer layer{ fcad::io::toLayer(dxfLayer) };

    // Assert
    const fcad::domain::Color expectedColor{ fcad::domain::Rgb{ .r = 0x30, .g = 0x50, .b = 0xA0 } };
    EXPECT_EQ(layer.color(), expectedColor);
}

TEST(DxfEntitiesTest, ToLayerOff)
{
    // Arrange
    const odxf::Layer dxfLayer{ .name = "Walls", .color = -1 };

    // Act
    const fcad::domain::Layer layer{ fcad::io::toLayer(dxfLayer) };

    // Assert
    const fcad::domain::Color expectedColor{ fcad::domain::Rgb{ .r = 255, .g = 0, .b = 0 } };
    EXPECT_FALSE(layer.isVisible());
    EXPECT_EQ(layer.color(), expectedColor);
}

TEST(DxfEntitiesTest, ToLayerOffWithInvalidColor)
{
    // Arrange
    const odxf::Layer dxfLayer{ .name = "Walls", .color = std::numeric_limits<int>::min() };

    // Act
    const fcad::domain::Layer layer{ fcad::io::toLayer(dxfLayer) };

    // Assert
    EXPECT_FALSE(layer.isVisible());
    EXPECT_EQ(layer.color(), fcad::domain::Color{ fcad::domain::Adaptive{} });
}

TEST(DxfEntitiesTest, ToLayerFrozen)
{
    // Arrange
    const odxf::Layer dxfLayer{ .name = "Walls", .flags = odxf::Layer::Flags::Frozen };

    // Act
    const fcad::domain::Layer layer{ fcad::io::toLayer(dxfLayer) };

    // Assert
    EXPECT_FALSE(layer.isVisible());
}

TEST(DxfEntitiesTest, ToLineScaled)
{
    // Arrange
    odxf::Line dxfLine;
    dxfLine.start = odxf::Coordinate3d{ .x = 1.0, .y = 2.0, .z = 3.0 };
    dxfLine.end = odxf::Coordinate3d{ .x = 4.0, .y = 5.0, .z = 6.0 };

    // Act
    const fcad::geo::Line line{ fcad::io::toLine(dxfLine, 10.0) };

    // Assert
    const fcad::geo::Line expectedLine{
        .start = { 10.0, 20.0 },
        .end = { 40.0, 50.0 },
    };
    EXPECT_THAT(line, fcad::test_utils::IsLine(expectedLine));
}

TEST(DxfEntitiesTest, ToCircleScaled)
{
    // Arrange
    odxf::Circle dxfCircle;
    dxfCircle.center = odxf::Coordinate3d{ .x = 1.0, .y = 2.0 };
    dxfCircle.radius = 0.5;

    // Act
    const std::optional<fcad::geo::Circle> circle{ fcad::io::toCircle(dxfCircle, 10.0) };

    // Assert
    ASSERT_TRUE(circle.has_value());

    const fcad::geo::Circle expectedCircle{
        .center = { 10.0, 20.0 },
        .radius = fcad::geo::Radius{ 5.0 },
    };
    EXPECT_THAT(circle.value(), fcad::test_utils::IsCircle(expectedCircle));
}

TEST(DxfEntitiesTest, ToCircleWithNegativeRadius)
{
    // Arrange
    odxf::Circle dxfCircle;
    dxfCircle.radius = -0.5;

    // Act
    const std::optional<fcad::geo::Circle> circle{ fcad::io::toCircle(dxfCircle, 1.0) };

    // Assert
    EXPECT_FALSE(circle.has_value());
}

TEST(DxfEntitiesTest, ToArcScaled)
{
    // Arrange
    odxf::Arc dxfArc;
    dxfArc.center = odxf::Coordinate3d{ .x = 1.0, .y = 2.0 };
    dxfArc.radius = 0.5;
    dxfArc.startAngle = 90.0;
    dxfArc.endAngle = 180.0;

    // Act
    const std::optional<fcad::geo::Arc> arc{ fcad::io::toArc(dxfArc, 10.0) };

    // Assert
    ASSERT_TRUE(arc.has_value());

    // The angles don't scale.
    const fcad::geo::Arc expectedArc{
        .center = { 10.0, 20.0 },
        .startAngle = fcad::core::Radians::fromDegrees(90.0),
        .endAngle = fcad::core::Radians::fromDegrees(180.0),
        .radius = fcad::geo::Radius{ 5.0 },
    };
    EXPECT_THAT(arc.value(), fcad::test_utils::IsArc(expectedArc));
}

TEST(DxfEntitiesTest, ToArcWithNegativeRadius)
{
    // Arrange
    odxf::Arc dxfArc;
    dxfArc.radius = -0.5;

    // Act
    const std::optional<fcad::geo::Arc> arc{ fcad::io::toArc(dxfArc, 1.0) };

    // Assert
    EXPECT_FALSE(arc.has_value());
}
