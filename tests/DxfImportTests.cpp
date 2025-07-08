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
}
