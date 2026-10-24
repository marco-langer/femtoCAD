#pragma once

#include "Settings.hpp"
#include "domain/Workspace.hpp"

#include "SceneView.hpp"

#include <QMainWindow>

class MainWindow final : public QMainWindow
{
public:
    MainWindow(fcad::domain::Workspace& workspace, Settings& settings);

private slots:
    void onNewProject();
    void onImportDxf();
    void onClose();

private:
    void createActions();
    void updateWindowTitle(const QString& filename = QString{});

    SceneView* m_sceneView{ nullptr };

    fcad::domain::Workspace& m_workspace;
    Settings& m_settings;
    bool m_unsavedChanges{ false };
};
