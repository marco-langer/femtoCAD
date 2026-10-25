#include "io/DxfImport.hpp"

#include "domain/Color.hpp"
#include "domain/Entity.hpp"
#include "domain/Layer.hpp"
#include "domain/LengthUnit.hpp"
#include "domain/Lineweight.hpp"
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

    EXPECT_THAT(
        importResult.value().layers, AreLayers(exampleLayers(1.0, ExampleProperties::Mixed))
    );

    const QStringList& warnings{ importResult.value().warnings };
    ASSERT_EQ(warnings.size(), 3);
    EXPECT_TRUE(warnings.at(0).startsWith(u"1 entity uses the color ByBlock"));
    EXPECT_TRUE(warnings.at(1).startsWith(u"1 entity uses the color ByEntity"));
    EXPECT_TRUE(warnings.at(2).startsWith(u"1 layer has an invalid lineweight"));
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
