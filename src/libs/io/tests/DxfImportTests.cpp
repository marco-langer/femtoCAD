#include "io/DxfImport.hpp"

#include "domain/Color.hpp"
#include "domain/Entity.hpp"
#include "domain/Layer.hpp"
#include "domain/LengthUnit.hpp"
#include "domain/Linetype.hpp"
#include "domain/Lineweight.hpp"
#include "geo/Arc.hpp"
#include "geo/Circle.hpp"
#include "geo/Line.hpp"
#include "io/DxfImportError.hpp"

#include "matchers/LayerMatcher.hpp"

#include <QFile>
#include <QStringList>

#include <gtest/gtest.h>

#include <algorithm>
#include <optional>
#include <utility>
#include <variant>

namespace {

// The colors and lineweights in the test files: example.dxf has one layer or entity per case, its
// copies with other units use ByLayer entity colors and no lineweights only.
enum class ExampleProperties {
    ByLayer,
    Mixed,
};

// The layers of the example files, with all coordinates and radii multiplied by scale.
fcad::domain::Layers exampleLayers(double scale, ExampleProperties properties)
{
    fcad::domain::Line indexColorLine{ fcad::geo::Line{
        .start = { 100.0 * scale, 100.0 * scale },
        .end = { 350.0 * scale, 400.0 * scale },
    } };
    fcad::domain::Line trueColorLine{ fcad::geo::Line{
        .start = { 150.0 * scale, 130.0 * scale },
        .end = { 250.0 * scale, 130.0 * scale },
    } };
    fcad::domain::Circle byBlockCircle{ fcad::geo::Circle{
        .center = { 50.4 * scale, 65.0 * scale },
        .radius = fcad::geo::Radius::fromValue(2.5 * scale).value(),
    } };
    // ByEntity, which becomes ByLayer.
    fcad::domain::Arc byEntityArc{ fcad::geo::Arc{
        .center = { 0.0 * scale, 2.0 * scale },
        .startAngle = fcad::core::Radians::fromDegrees(0.0),
        .endAngle = fcad::core::Radians::fromDegrees(180.0),
        .radius = fcad::geo::Radius::fromValue(0.5 * scale).value(),
    } };

    if (properties == ExampleProperties::Mixed) {
        indexColorLine.setColor(fcad::domain::Rgb{ .r = 255, .g = 0, .b = 0 });
        trueColorLine.setColor(fcad::domain::Rgb{ .r = 0x30, .g = 0x50, .b = 0xA0 });
        byBlockCircle.setColor(fcad::domain::Adaptive{});

        indexColorLine.setLineweight(fcad::domain::Lineweight{ 35 });
        trueColorLine.setLineweight(fcad::domain::DefaultLineweight{});
        // ByBlock and an invalid lineweight, which become the default.
        byBlockCircle.setLineweight(fcad::domain::DefaultLineweight{});
        byEntityArc.setLineweight(fcad::domain::DefaultLineweight{});

        indexColorLine.setLinetypeScale(fcad::domain::LinetypeScale{ 3.0 });
        // The arc's linetype scale is invalid and becomes 1.0.
    }

    fcad::domain::Layers layers;

    fcad::domain::Layer& testLayer{ layers.emplace_back("Test Layer") };
    testLayer.setVisible(false);

    fcad::domain::Layer& linesLayer{ layers.emplace_back("Lines") };
    linesLayer.add(std::move(indexColorLine));
    linesLayer.add(std::move(trueColorLine));
    if (properties == ExampleProperties::Mixed) {
        linesLayer.setLineweight(fcad::domain::Lineweight{ 50 });
    }

    fcad::domain::Layer& circlesLayer{ layers.emplace_back("Circles") };
    circlesLayer.add(std::move(byBlockCircle));
    if (properties == ExampleProperties::Mixed) {
        // Clamped from 300 in example.dxf.
        circlesLayer.setLineweight(fcad::domain::Lineweight{ 211 });
    }

    // In example.dxf its lineweight is invalid (-5) and becomes the default.
    layers.emplace_back("Arcs").add(std::move(byEntityArc));

    return layers;
}

// The name of the linetype an imported layer refers to.
QString linetypeName(const fcad::io::ImportedDxf& imported, fcad::domain::LinetypeId id)
{
    const auto it{ std::ranges::find(imported.linetypes, id, &fcad::domain::Linetype::id) };
    if (it == imported.linetypes.end()) {
        return QStringLiteral(u"<unknown id>");
    }

    return it->name().value();
}

// The name of the linetype an imported entity refers to, or "ByLayer".
QString
linetypeName(const fcad::io::ImportedDxf& imported, const fcad::domain::EntityLinetype& linetype)
{
    if (std::holds_alternative<fcad::domain::ByLayer>(linetype)) {
        return QStringLiteral(u"ByLayer");
    }

    return linetypeName(imported, std::get<fcad::domain::LinetypeId>(linetype));
}

}   // namespace

TEST(DxfImportTest, ImportExample)
{
    // Arrange
    const QString filePath{ TEST_DATA_DIR "example.dxf" };
    ASSERT_TRUE(QFile::exists(filePath));

    // Act
    fcad::core::Result<fcad::io::ImportedDxf> importResult{ fcad::io::importDxf(filePath) };

    // Assert
    if (!importResult.has_value()) {
        FAIL() << importResult.error().message.toStdString();
    }

    const fcad::io::ImportedDxf& imported{ importResult.value() };

    EXPECT_THAT(imported.layers, AreLayers(exampleLayers(1.0, ExampleProperties::Mixed)));

    // CONTINUOUS is built in, GAS contains a text, and BAD<NAME> and the second "dashed" are
    // invalid, so only two linetypes are added to Continuous.
    ASSERT_EQ(imported.linetypes.size(), 3);
    auto linetype{ imported.linetypes.begin() };
    EXPECT_EQ(linetype->id(), fcad::domain::continuousLinetypeId());
    ++linetype;
    EXPECT_EQ(linetype->name().value(), u"DASHED");
    EXPECT_EQ(
        linetype->pattern().elements(),
        (fcad::domain::PatternElements{
            fcad::domain::Dash{ fcad::domain::PatternLength{ 0.5 } },
            fcad::domain::Gap{ fcad::domain::PatternLength{ 0.25 } },
        })
    );
    ++linetype;
    EXPECT_EQ(linetype->name().value(), u"DOTTED");
    EXPECT_EQ(
        linetype->pattern().elements(),
        (fcad::domain::PatternElements{
            fcad::domain::Dot{},
            fcad::domain::Gap{ fcad::domain::PatternLength{ 0.25 } },
        })
    );
    EXPECT_EQ(imported.linetypeScale, fcad::domain::LinetypeScale{ 2.0 });

    // The layers refer to GAS, DASHED, "dotted" and an unknown linetype.
    ASSERT_EQ(imported.layers.size(), 4);
    EXPECT_EQ(linetypeName(imported, imported.layers.at(0).linetype()), u"Continuous");
    EXPECT_EQ(linetypeName(imported, imported.layers.at(1).linetype()), u"DASHED");
    EXPECT_EQ(linetypeName(imported, imported.layers.at(2).linetype()), u"DOTTED");
    EXPECT_EQ(linetypeName(imported, imported.layers.at(3).linetype()), u"Continuous");

    // The entities refer to "dotted", ByLayer, ByBlock and an unknown linetype.
    const fcad::domain::Lines& lines{ imported.layers.at(1).lines() };
    ASSERT_EQ(lines.size(), 2);
    EXPECT_EQ(linetypeName(imported, lines.at(0).linetype()), u"DOTTED");
    EXPECT_EQ(linetypeName(imported, lines.at(1).linetype()), u"ByLayer");
    ASSERT_EQ(imported.layers.at(2).circles().size(), 1);
    EXPECT_EQ(
        linetypeName(imported, imported.layers.at(2).circles().front().linetype()), u"Continuous"
    );
    ASSERT_EQ(imported.layers.at(3).arcs().size(), 1);
    EXPECT_EQ(
        linetypeName(imported, imported.layers.at(3).arcs().front().linetype()), u"Continuous"
    );

    const QStringList& warnings{ imported.warnings };
    ASSERT_EQ(warnings.size(), 11);
    EXPECT_TRUE(warnings.at(0).startsWith(u"1 entity uses the color ByBlock"));
    EXPECT_TRUE(warnings.at(1).startsWith(u"1 entity uses the color ByEntity"));
    EXPECT_TRUE(warnings.at(2).startsWith(u"1 layer has an invalid lineweight"));
    EXPECT_TRUE(warnings.at(3).startsWith(u"1 entity uses the lineweight ByBlock"));
    EXPECT_TRUE(warnings.at(4).startsWith(u"1 entity has an invalid lineweight"));
    EXPECT_TRUE(warnings.at(5).startsWith(u"1 linetype contains texts or shapes"));
    EXPECT_TRUE(warnings.at(6).startsWith(u"2 linetypes have an invalid or duplicate name"));
    EXPECT_TRUE(warnings.at(7).startsWith(u"1 layer uses an unknown linetype"));
    EXPECT_TRUE(warnings.at(8).startsWith(u"1 entity uses the linetype ByBlock"));
    EXPECT_TRUE(warnings.at(9).startsWith(u"1 entity uses an unknown linetype"));
    EXPECT_TRUE(warnings.at(10).startsWith(u"1 entity has an invalid linetype scale"));
}

TEST(DxfImportTest, ImportWithoutUnit)
{
    // Arrange
    const QString filePath{ TEST_DATA_DIR "example.dxf" };
    ASSERT_TRUE(QFile::exists(filePath));

    // Act
    fcad::core::Result<fcad::io::ImportedDxf> importResult{ fcad::io::importDxf(filePath) };

    // Assert
    if (!importResult.has_value()) {
        FAIL() << importResult.error().message.toStdString();
    }

    EXPECT_EQ(importResult.value().unit, std::nullopt);
}

TEST(DxfImportTest, ImportUnit)
{
    // Arrange
    const QString filePath{ TEST_DATA_DIR "example-millimeter.dxf" };
    ASSERT_TRUE(QFile::exists(filePath));

    // Act
    fcad::core::Result<fcad::io::ImportedDxf> importResult{ fcad::io::importDxf(filePath) };

    // Assert
    if (!importResult.has_value()) {
        FAIL() << importResult.error().message.toStdString();
    }

    EXPECT_EQ(importResult.value().unit, fcad::domain::LengthUnit::Millimeter);
    EXPECT_THAT(
        importResult.value().layers, AreLayers(exampleLayers(1.0, ExampleProperties::ByLayer))
    );
    EXPECT_TRUE(importResult.value().warnings.isEmpty());
}

TEST(DxfImportTest, ImportUnsupportedUnit)
{
    // Arrange
    const QString filePath{ TEST_DATA_DIR "example-decameter.dxf" };
    ASSERT_TRUE(QFile::exists(filePath));

    // Act
    fcad::core::Result<fcad::io::ImportedDxf> importResult{ fcad::io::importDxf(filePath) };

    // Assert
    if (!importResult.has_value()) {
        FAIL() << importResult.error().message.toStdString();
    }

    EXPECT_EQ(importResult.value().unit, fcad::domain::LengthUnit::Meter);
    EXPECT_THAT(
        importResult.value().layers, AreLayers(exampleLayers(10.0, ExampleProperties::ByLayer))
    );
    EXPECT_EQ(importResult.value().warnings.size(), 1);
}

TEST(DxfImportTest, ImportInvalidUnit)
{
    // Arrange
    const QString filePath{ TEST_DATA_DIR "invalid-unit.dxf" };
    ASSERT_TRUE(QFile::exists(filePath));

    // Act
    fcad::core::Result<fcad::io::ImportedDxf> importResult{ fcad::io::importDxf(filePath) };

    // Assert
    if (!importResult.has_value()) {
        FAIL() << importResult.error().message.toStdString();
    }

    EXPECT_EQ(importResult.value().unit, std::nullopt);
    EXPECT_EQ(importResult.value().warnings.size(), 1);
}

TEST(DxfImportTest, MissingFile)
{
    // Arrange
    const QString filePath{ TEST_DATA_DIR "missing.dxf" };
    ASSERT_FALSE(QFile::exists(filePath));

    // Act
    fcad::core::Result<fcad::io::ImportedDxf> importResult{ fcad::io::importDxf(filePath) };

    // Assert
    ASSERT_FALSE(importResult.has_value());
    EXPECT_EQ(importResult.error().code, fcad::io::DxfImportError::CannotOpenFile);
    EXPECT_FALSE(importResult.error().message.isEmpty());
}
