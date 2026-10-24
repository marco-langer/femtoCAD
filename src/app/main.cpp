#include "MainWindow.hpp"
#include "Settings.hpp"
#include "domain/Workspace.hpp"

#include <QApplication>

int main(int argc, char** argv)
{
    QApplication app{ argc, argv };

    fcad::domain::Workspace workspace;
    fcad::app::Settings settings{ fcad::app::loadSettings() };

    fcad::app::MainWindow mainWindow{ workspace, settings };
    mainWindow.show();

    const int appResult{ app.exec() };

    if (appResult == 0) {
        fcad::app::saveSettings(settings);
    }

    return appResult;
}
