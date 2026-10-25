#include "io/DxfImport.hpp"

#include "domain/Layer.hpp"
#include "domain/LengthUnit.hpp"
#include "geo/Arc.hpp"
#include "geo/Circle.hpp"
#include "geo/Line.hpp"
#include "io/DxfImportError.hpp"

#include "matchers/LayerMatcher.hpp"

#include <QFile>

#include <gtest/gtest.h>

#include <optional>

namespace {

// The layers of example.dxf, with all coordinates and radii multiplied by scale.
fcad::domain::Layers exampleLayers(double scale)
{
    fcad::domain::Layers layers;

    fcad::domain::Layer& testLayer{ layers.emplace_back("Test Layer") };
    testLayer.visible = false;

    fcad::domain::Layer& linesLayer{ layers.emplace_back("Lines") };
    linesLayer.lines.emplace_back(
        fcad::geo::Line{
            .start = { 100.0 * scale, 100.0 * scale },
            .end = { 350.0 * scale, 400.0 * scale },
        }
    );
    linesLayer.lines.emplace_back(
        fcad::geo::Line{
            .start = { 150.0 * scale, 130.0 * scale },
            .end = { 250.0 * scale, 130.0 * scale },
        }
    );

    fcad::domain::Layer& circlesLayer{ layers.emplace_back("Circles") };
    circlesLayer.circles.emplace_back(
        fcad::geo::Circle{
            .center = { 50.4 * scale, 65.0 * scale },
            .radius = fcad::geo::Radius::fromValue(2.5 * scale).value(),
        }
    );

    fcad::domain::Layer& arcsLayer{ layers.emplace_back("Arcs") };
    arcsLayer.arcs.emplace_back(
        fcad::geo::Arc{
            .center = { 0.0 * scale, 2.0 * scale },
            .startAngle = fcad::core::Radians::fromDegrees(0.0),
            .endAngle = fcad::core::Radians::fromDegrees(180.0),
            .radius = fcad::geo::Radius::fromValue(0.5 * scale).value(),
        }
    );

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

    EXPECT_THAT(importResult.value().layers, AreLayers(exampleLayers(1.0)));
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
    EXPECT_THAT(importResult.value().layers, AreLayers(exampleLayers(1.0)));
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
    EXPECT_THAT(importResult.value().layers, AreLayers(exampleLayers(10.0)));
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
