#pragma once

#include "Settings.hpp"
#include "core/Workspace.hpp"

#include "SceneView.hpp"

#include <QMainWindow>

class MainWindow final : public QMainWindow
{
public:
    MainWindow(Workspace& workspace, Settings& settings);

private slots:
    void onNewProject();
    void onImportDxf();
    void onClose();

private:
    void createActions();
    void updateWindowTitle(const QString& filename = QString{});

    SceneView* m_sceneView{ nullptr };

    Workspace& m_workspace;
    Settings& m_settings;
    bool m_unsavedChanges{ false };
};
