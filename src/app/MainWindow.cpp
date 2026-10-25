#include "MainWindow.hpp"

#include "domain/LengthUnit.hpp"
#include "io/DxfImport.hpp"

#include <QAction>
#include <QFileDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QToolBar>

#include <optional>

#include "SceneView.hpp"
#include "features/io/dxf/LengthUnitDialog.hpp"

namespace fcad::app {

MainWindow::MainWindow(domain::Workspace& workspace, Settings& settings)
    : m_workspace{ workspace }
    , m_settings{ settings }
{
    createActions();

    m_sceneView = new SceneView{ m_workspace.document() };

    setStatusBar(new QStatusBar{});

    connect(m_sceneView, &SceneView::mousePositionChanged, [this](const QPointF& position) {
        statusBar()->showMessage(
            QString{ "(%1, %2)" }.arg(position.x(), 0, 'd', 0).arg(position.y(), 0, 'd', 0), 5000
        );
    });

    updateWindowTitle();
    setCentralWidget(m_sceneView);
}

void MainWindow::createActions()
{
    auto* newAction{ new QAction{ "New" } };
    newAction->setShortcut(QKeySequence{ Qt::CTRL | Qt::Key_N });
    newAction->setEnabled(false);
    connect(newAction, &QAction::triggered, this, &MainWindow::onNewProject);

    auto* saveAction{ new QAction{ "Save" } };
    saveAction->setShortcut(QKeySequence{ Qt::CTRL | Qt::Key_S });
    saveAction->setEnabled(false);

    auto* saveAsAction{ new QAction{ "Save As..." } };
    saveAsAction->setEnabled(false);
    saveAsAction->setShortcut(QKeySequence{ Qt::CTRL | Qt::SHIFT | Qt::Key_S });

    auto* importDxfAction{ new QAction{ "Dxf (.dxf)" } };
    importDxfAction->setShortcut(QKeySequence{ Qt::CTRL | Qt::Key_I });
    connect(importDxfAction, &QAction::triggered, this, &MainWindow::onImportDxf);

    auto* exportAction{ new QAction{ "Export..." } };
    exportAction->setEnabled(false);

    auto* quitAction{ new QAction{ "Quit" } };
    quitAction->setShortcut(QKeySequence{ Qt::CTRL | Qt::Key_Q });
    connect(quitAction, &QAction::triggered, this, &MainWindow::onClose);

    QMenu* fileMenu{ menuBar()->addMenu("&File") };
    fileMenu->addAction(newAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);

    fileMenu->addSeparator();
    QMenu* importMenu{ fileMenu->addMenu("Import") };
    importMenu->addAction(importDxfAction);
    fileMenu->addAction(exportAction);

    fileMenu->addSeparator();
    fileMenu->addAction(quitAction);
}

void MainWindow::updateWindowTitle(const QString& filePath)
{
    if (filePath.isEmpty()) {
        setWindowTitle(QStringLiteral(u"femtoCAD"));
    } else {
        const QFileInfo fileInfo{ filePath };
        setWindowTitle(QStringView{ u"femtoCAD - %1" }.arg(fileInfo.completeBaseName()));
    }
}

void MainWindow::onNewProject()
{
    domain::Document document{ m_settings.defaultUnit };
    document.addLayer(domain::Layer{ "Layer 1" });
    m_workspace.setDocument(std::move(document));
    m_sceneView->updateDocument();
}

void MainWindow::onImportDxf()
{
    QString dirName{ QDir::homePath() };
    if (m_settings.dxfImport.lastImportPath.has_value()) {
        QFileInfo fileInfo{ m_settings.dxfImport.lastImportPath.value() };
        if (fileInfo.exists()) {
            dirName = fileInfo.absolutePath();
        }
    }

    const QString filePath{
        QFileDialog::getOpenFileName(this, "Import dxf file", dirName, "Dxf (*.dxf)")
    };
    if (filePath.isEmpty()) {
        return;
    }

    core::Result<io::ImportedDxf> importResult{ io::importDxf(filePath) };
    if (!importResult.has_value()) {
        QMessageBox::critical(this, "Dxf import error", importResult.error().message);
        return;
    }

    io::ImportedDxf& importedDxf{ importResult.value() };

    const std::optional<domain::LengthUnit> unit{ importedDxf.unit.has_value()
                                                      ? importedDxf.unit
                                                      : askForDxfUnit(m_settings.dxfImport, this) };
    if (!unit.has_value()) {
        return;
    }

    domain::Document document{ unit.value() };
    document.addLayers(std::move(importedDxf.layers));
    m_workspace.setDocument(std::move(document));
    m_settings.dxfImport.lastImportPath = filePath;
    m_sceneView->updateDocument();
    updateWindowTitle(filePath);
}

void MainWindow::onClose() { close(); }

}   // namespace fcad::app
