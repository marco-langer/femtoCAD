#pragma once

#include "Settings.hpp"
#include "domain/Workspace.hpp"

#include "SceneView.hpp"

#include <QMainWindow>

namespace fcad::app {

class MainWindow final : public QMainWindow
{
public:
    MainWindow(domain::Workspace& workspace, Settings& settings);

private slots:
    void onNewProject();
    void onImportDxf();
    void onShowAxesToggled(bool visible);
    void onShowGridToggled(bool visible);
    void onShowLineweightsToggled(bool visible);
    void onClose();

private:
    void createActions();
    void updateWindowTitle(const QString& filename = QString{});

    SceneView* m_sceneView{ nullptr };

    domain::Workspace& m_workspace;
    Settings& m_settings;
    bool m_unsavedChanges{ false };
};

}   // namespace fcad::app
