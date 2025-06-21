#pragma once

#include "core/Workspace.hpp"

#include "SceneView.hpp"

#include <QMainWindow>

class MainWindow : public QMainWindow
{
public:
    MainWindow(Workspace& workspace);

private slots:
    void onNewProject();
    void onImportDxf();
    void onClose();

private:
    void createActions();

    SceneView* m_sceneView{ nullptr };

    Workspace& m_workspace;
    bool m_unsavedChanges{ false };
};
