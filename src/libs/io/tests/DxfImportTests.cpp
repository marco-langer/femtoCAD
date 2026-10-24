#include "io/DxfImport.hpp"

#include "domain/Document.hpp"
#include "domain/Layer.hpp"
#include "geo/Arc.hpp"
#include "geo/Circle.hpp"
#include "geo/Line.hpp"

#include "matchers/LayerMatcher.hpp"

#include <QFile>

#include <gtest/gtest.h>

TEST(DxfImportTest, ImportExample)
{
    // Arrange
    const QString filePath{ TEST_DATA_DIR "example.dxf" };
    ASSERT_TRUE(QFile::exists(filePath));

    // Act
    std::expected<fcad::domain::Document, QString> importResult{ fcad::io::importDxf(filePath) };

    // Assert
    if (!importResult.has_value()) {
        FAIL() << importResult.error().toStdString();
    }

    const fcad::domain::Layers& layers{ importResult.value().layers() };

    fcad::domain::Layers expectedLayers;

    fcad::domain::Layer& testLayer{ expectedLayers.emplace_back("Test Layer") };
    testLayer.visible = false;

    fcad::domain::Layer& linesLayer{ expectedLayers.emplace_back("Lines") };
    linesLayer.lines.push_back(
        fcad::geo::Line{
            .start = { 100.0, 100.0 },
            .end = { 350.0, 400.0 },
        }
    );
    linesLayer.lines.push_back(
        fcad::geo::Line{
            .start = { 150.0, 130.0 },
            .end = { 250.0, 130.0 },
        }
    );

    fcad::domain::Layer& circlesLayer{ expectedLayers.emplace_back("Circles") };
    circlesLayer.circles.push_back(
        fcad::geo::Circle{
            .center = { 50.4, 65.0 },
            .radius = fcad::geo::Radius{ 2.5 },
        }
    );

    fcad::domain::Layer& arcsLayer{ expectedLayers.emplace_back("Arcs") };
    arcsLayer.arcs.push_back(
        fcad::geo::Arc{
            .center = { 0.0, 2.0 },
            .startAngle = fcad::core::Radians::fromDegrees(0.0),
            .endAngle = fcad::core::Radians::fromDegrees(180.0),
            .radius = fcad::geo::Radius{ 0.5 },
        }
    );

    EXPECT_THAT(layers, AreLayers(expectedLayers));
}
