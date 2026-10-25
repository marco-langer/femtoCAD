#include "domain/Linetype.hpp"
#include "domain/Linetypes.hpp"

#include <QString>

#include <gtest/gtest.h>

#include <iterator>
#include <optional>
#include <utility>

namespace {

fcad::domain::LinetypeName nameOf(const char16_t* name)
{
    return fcad::domain::LinetypeName::fromValue(QString::fromUtf16(name)).value();
}

fcad::domain::LinetypeDefinition dashed()
{
    return fcad::domain::LinetypeDefinition{
        nameOf(u"Dashed"),
        fcad::domain::DashPattern::fromElements(
            {
                fcad::domain::Dash{ fcad::domain::PatternLength{ 0.5 } },
                fcad::domain::Gap{ fcad::domain::PatternLength{ 0.25 } },
            }
        )
            .value(),
    };
}

}   // namespace

TEST(LinetypesTest, newCollectionsContainOnlyContinuous)
{
    const fcad::domain::Linetypes linetypes;

    ASSERT_EQ(linetypes.size(), 1);
    EXPECT_EQ(linetypes.begin()->id(), fcad::domain::continuousLinetypeId());
    EXPECT_TRUE(linetypes.contains(fcad::domain::continuousLinetypeId()));
    EXPECT_EQ(linetypes.at(fcad::domain::continuousLinetypeId()).name(), nameOf(u"Continuous"));
}

TEST(LinetypesTest, add)
{
    // Arrange
    fcad::domain::Linetypes linetypes;

    // Act
    const fcad::domain::LinetypeId id{ linetypes.tryAdd(dashed()).value() };

    // Assert
    ASSERT_EQ(linetypes.size(), 2);
    EXPECT_TRUE(linetypes.contains(id));
    EXPECT_EQ(linetypes.at(id).name(), nameOf(u"Dashed"));
    EXPECT_EQ(std::next(linetypes.begin())->id(), id);
}

TEST(LinetypesTest, tryAddRejectsATakenName)
{
    // Arrange
    fcad::domain::Linetypes linetypes;
    const std::optional<fcad::domain::LinetypeId> first{ linetypes.tryAdd(dashed()) };

    // Act
    fcad::domain::LinetypeDefinition sameName{ dashed() };
    sameName.name = nameOf(u"DASHED");
    const std::optional<fcad::domain::LinetypeId> second{ linetypes.tryAdd(std::move(sameName)) };
    const std::optional<fcad::domain::LinetypeId> continuous{ linetypes.tryAdd(
        fcad::domain::LinetypeDefinition{ .name = nameOf(u"continuous"),
                                          .pattern = fcad::domain::DashPattern::continuous() }
    ) };

    // Assert
    EXPECT_TRUE(first.has_value());
    EXPECT_EQ(second, std::nullopt);
    EXPECT_EQ(continuous, std::nullopt);
    EXPECT_EQ(linetypes.size(), 2);
}

TEST(LinetypesTest, containsOnlyItsOwnLinetypes)
{
    const fcad::domain::Linetypes linetypes;

    EXPECT_FALSE(linetypes.contains(fcad::domain::LinetypeId::generate()));
}
