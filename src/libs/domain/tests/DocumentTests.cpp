#include "domain/Document.hpp"
#include "domain/Layer.hpp"
#include "domain/LengthUnit.hpp"
#include "domain/Linetype.hpp"
#include "domain/Linetypes.hpp"

#include <QString>

#include <gtest/gtest.h>

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

TEST(DocumentTest, newDocumentsContainOnlyContinuous)
{
    const fcad::domain::Document document{ fcad::domain::LengthUnit::Millimeter };

    ASSERT_EQ(document.linetypes().size(), 1);
    EXPECT_TRUE(document.linetypes().contains(fcad::domain::continuousLinetypeId()));
}

TEST(DocumentTest, constructWithLinetypes)
{
    // Arrange
    fcad::domain::Linetypes linetypes;
    const fcad::domain::LinetypeId id{ linetypes.tryAdd(dashed()).value() };

    // Act
    const fcad::domain::Document document{ fcad::domain::LengthUnit::Millimeter,
                                           std::move(linetypes) };

    // Assert
    ASSERT_EQ(document.linetypes().size(), 2);
    EXPECT_EQ(document.linetypes().at(id).name(), nameOf(u"Dashed"));
}

TEST(DocumentTest, linetypeScaleIsOneByDefault)
{
    fcad::domain::Document document{ fcad::domain::LengthUnit::Millimeter };
    EXPECT_EQ(document.linetypeScale(), fcad::domain::LinetypeScale{ 1.0 });

    document.setLinetypeScale(fcad::domain::LinetypeScale{ 2.5 });
    EXPECT_EQ(document.linetypeScale(), fcad::domain::LinetypeScale{ 2.5 });
}

TEST(DocumentTest, addLayerWithALinetypeOfTheDocument)
{
    // Arrange
    fcad::domain::Linetypes linetypes;
    const fcad::domain::LinetypeId id{ linetypes.tryAdd(dashed()).value() };
    fcad::domain::Document document{ fcad::domain::LengthUnit::Millimeter, std::move(linetypes) };

    fcad::domain::Layer layer{ "Layer Name" };
    layer.setLinetype(id);

    // Act
    document.addLayer(std::move(layer));

    // Assert
    ASSERT_EQ(document.layers().size(), 1);
    EXPECT_EQ(document.layers().front().linetype(), id);
}
