#include "io/DxfImport.hpp"

#include "domain/Color.hpp"
#include "domain/Entity.hpp"
#include "domain/Layer.hpp"
#include "domain/LengthUnit.hpp"
#include "geo/Arc.hpp"
#include "geo/Circle.hpp"
#include "geo/Line.hpp"
#include "io/DxfImportError.hpp"

#include "matchers/LayerMatcher.hpp"

#include <QFile>
#include <QStringList>

#include <gtest/gtest.h>

#include <optional>
#include <utility>

namespace {

// The entity colors in the test files: example.dxf has one entity per color case, its copies with
// other units use ByLayer only.
enum class ExampleColors {
    ByLayer,
    Mixed,
};

// The layers of the example files, with all coordinates and radii multiplied by scale.
fcad::domain::Layers exampleLayers(double scale, ExampleColors colors)
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

    if (colors == ExampleColors::Mixed) {
        indexColorLine.setColor(fcad::domain::Rgb{ .r = 255, .g = 0, .b = 0 });
        trueColorLine.setColor(fcad::domain::Rgb{ .r = 0x30, .g = 0x50, .b = 0xA0 });
        byBlockCircle.setColor(fcad::domain::Adaptive{});
    }

    fcad::domain::Layers layers;

    fcad::domain::Layer& testLayer{ layers.emplace_back("Test Layer") };
    testLayer.setVisible(false);

    fcad::domain::Layer& linesLayer{ layers.emplace_back("Lines") };
    linesLayer.add(std::move(indexColorLine));
    linesLayer.add(std::move(trueColorLine));

    layers.emplace_back("Circles").add(std::move(byBlockCircle));
    layers.emplace_back("Arcs").add(std::move(byEntityArc));

    return layers;
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

    EXPECT_THAT(importResult.value().layers, AreLayers(exampleLayers(1.0, ExampleColors::Mixed)));

    const QStringList& warnings{ importResult.value().warnings };
    ASSERT_EQ(warnings.size(), 2);
    EXPECT_TRUE(warnings.at(0).startsWith(u"1 entity uses the color ByBlock"));
    EXPECT_TRUE(warnings.at(1).startsWith(u"1 entity uses the color ByEntity"));
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
    EXPECT_THAT(importResult.value().layers, AreLayers(exampleLayers(1.0, ExampleColors::ByLayer)));
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
        importResult.value().layers, AreLayers(exampleLayers(10.0, ExampleColors::ByLayer))
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
