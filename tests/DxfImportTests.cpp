#include "core/io/DxfImport.hpp"

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

    Layer& testLayer{ expectedLayers.emplace_back(Layer{ "Test Layer" }) };
    testLayer.visible = false;

    Layer& linesLayer{ expectedLayers.emplace_back(Layer{ "Lines" }) };
    linesLayer.lines.push_back(Line{
        .start = { 100.0, 100.0 },
        .end = { 350.0, 400.0 },
    });
    linesLayer.lines.push_back(Line{
        .start = { 150.0, 130.0 },
        .end = { 250.0, 130.0 },
    });

    EXPECT_THAT(layers, AreLayers(expectedLayers));
}
