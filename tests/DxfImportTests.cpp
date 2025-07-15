#include "core/io/DxfImport.hpp"

#include "core/Arc.hpp"
#include "core/Circle.hpp"
#include "core/Layer.hpp"
#include "core/Line.hpp"

#include "Matchers/LayerMatcher.hpp"

#include <QFile>

#include <gtest/gtest.h>

TEST(DxfImportTest, ImportExample)
{
    // Arrange
    const QString filePath{ TEST_DATA_DIR "example.dxf" };
    ASSERT_TRUE(QFile::exists(filePath));

    Workspace workspace;

    // Act
    tl::expected<void, QString> importResult{ importDxf(workspace, filePath) };

    // Assert
    if (!importResult.has_value()) {
        FAIL() << importResult.error().toStdString();
    }

    const Layers& layers{ workspace.layers() };

    Layers expectedLayers;

    Layer& testLayer{ expectedLayers.emplace_back("Test Layer") };
    testLayer.visible = false;

    Layer& linesLayer{ expectedLayers.emplace_back("Lines") };
    linesLayer.lines.push_back(Line{
        .start = { 100.0, 100.0 },
        .end = { 350.0, 400.0 },
    });
    linesLayer.lines.push_back(Line{
        .start = { 150.0, 130.0 },
        .end = { 250.0, 130.0 },
    });

    Layer& circlesLayer{ expectedLayers.emplace_back("Circles") };
    circlesLayer.circles.push_back(Circle{
        .center = { 50.4, 65.0 },
        .radius = 2.5,
    });

    Layer& arcsLayer{ expectedLayers.emplace_back("Arcs") };
    arcsLayer.arcs.push_back(Arc{
        .center = { 0.0, 2.0 },
        .startAngle = Radians::fromDegrees(0.0),
        .endAngle = Radians::fromDegrees(180.0),
        .radius = 0.5,
    });

    EXPECT_THAT(layers, AreLayers(expectedLayers));
}
