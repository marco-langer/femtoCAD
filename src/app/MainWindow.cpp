#include "MainWindow.hpp"

#include <QAction>
#include <QFileDialog>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>

#include "SceneView.hpp"

MainWindow::MainWindow(Workspace& workspace)
    : m_workspace{ workspace }
{
    createActions();

    Layer layer{ "Layer 1" };
    layer.lines.push_back(Line{ .start = Coordinate{ 100.0, 100.0 },
                                .end = Coordinate{ 400.0, 500.0 } });
    m_workspace.addLayer(std::move(layer));

    m_sceneView = new SceneView{ m_workspace };

    setStatusBar(new QStatusBar{});

    connect(m_sceneView, &SceneView::mousePositionChanged, [this](const QPointF& position) {
        statusBar()->showMessage(
            QString{ "(%1, %2)" }.arg(position.x(), 0, 'd', 0).arg(position.y(), 0, 'd', 0), 5000
        );
    });

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

    QToolBar* toolBar{ addToolBar("Tools") };
    toolBar->setFloatable(false);
    toolBar->setMovable(false);

    toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    auto* addLineAction = new QAction{ "Line" };
    addLineAction->setEnabled(false);
    toolBar->addAction(addLineAction);
}

void MainWindow::onNewProject()
{
    m_workspace.clear();
    m_workspace.addLayer(Layer{ "Layer 1" });
    m_sceneView->updateWorkspace();
}

void MainWindow::onImportDxf()
{
    QString dirName;
    const QString filePath{
        QFileDialog::getOpenFileName(this, "Import dxf file", dirName, "Dxf (*.dxf)")
    };
    if (filePath.isEmpty()) {
        return;
    }
}

void MainWindow::onClose() { close(); }
