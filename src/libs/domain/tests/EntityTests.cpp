#include "domain/Entity.hpp"
#include "geo/BoundingBox.hpp"
#include "geo/Coordinate.hpp"
#include "geo/Line.hpp"

#include "test_utils/matchers/BoundingBoxMatcher.hpp"
#include "test_utils/matchers/LineMatcher.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <type_traits>
#include <utility>

namespace {

const fcad::geo::Line someLine{
    .start = fcad::geo::Coordinate{ .x = 1.0, .y = 4.0 },
    .end = fcad::geo::Coordinate{ .x = 3.0, .y = 2.0 },
};

const fcad::geo::Line otherLine{
    .start = fcad::geo::Coordinate{ .x = -1.0, .y = 0.0 },
    .end = fcad::geo::Coordinate{ .x = 5.0, .y = 6.0 },
};

}   // namespace

TEST(EntityTest, isMoveOnly)
{
    EXPECT_FALSE(std::is_copy_constructible_v<fcad::domain::Line>);
    EXPECT_FALSE(std::is_copy_assignable_v<fcad::domain::Line>);
    EXPECT_TRUE(std::is_nothrow_move_constructible_v<fcad::domain::Line>);
    EXPECT_TRUE(std::is_nothrow_move_assignable_v<fcad::domain::Line>);
}

TEST(EntityTest, newEntitiesHaveDifferentIds)
{
    // Arrange
    const fcad::domain::Line first{ someLine };
    const fcad::domain::Line second{ someLine };

    // Assert
    EXPECT_NE(first.id(), second.id());
}

TEST(EntityTest, moveKeepsTheId)
{
    // Arrange
    fcad::domain::Line line{ someLine };
    const fcad::domain::EntityId id{ line.id() };

    // Act
    const fcad::domain::Line moved{ std::move(line) };

    // Assert
    EXPECT_EQ(moved.id(), id);
    EXPECT_THAT(moved.geometry(), fcad::test_utils::IsLine(someLine));
}

TEST(EntityTest, colorIsByLayerByDefault)
{
    const fcad::domain::Line line{ someLine };

    EXPECT_EQ(line.color(), fcad::domain::EntityColor{ fcad::domain::ByLayer{} });
}

TEST(EntityTest, cloneHasANewIdAndTheSameContent)
{
    // Arrange
    const fcad::domain::EntityColor color{ fcad::domain::Rgb{ .r = 255, .g = 0, .b = 0 } };

    fcad::domain::Line line{ someLine };
    line.setColor(color);

    // Act
    const fcad::domain::Line clone{ line.clone() };

    // Assert
    EXPECT_NE(clone.id(), line.id());
    EXPECT_THAT(clone.geometry(), fcad::test_utils::IsLine(someLine));
    EXPECT_EQ(clone.color(), color);
}

TEST(EntityTest, setGeometryKeepsTheId)
{
    // Arrange
    fcad::domain::Line line{ someLine };
    const fcad::domain::EntityId id{ line.id() };

    // Act
    line.setGeometry(otherLine);

    // Assert
    EXPECT_EQ(line.id(), id);
    EXPECT_THAT(line.geometry(), fcad::test_utils::IsLine(otherLine));
}

TEST(EntityTest, boundingBox)
{
    // Arrange
    const fcad::domain::Line line{ someLine };

    // Act
    const fcad::geo::BoundingBox bb{ fcad::domain::boundingBox(line) };

    // Assert
    const fcad::geo::BoundingBox expectedBb{
        .min = fcad::geo::Coordinate{ .x = 1.0, .y = 2.0 },
        .max = fcad::geo::Coordinate{ .x = 3.0, .y = 4.0 },
    };
    EXPECT_THAT(bb, fcad::test_utils::IsBoundingBox(expectedBb));
}
