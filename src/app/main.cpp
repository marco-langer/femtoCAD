#include "MainWindow.hpp"
#include "Settings.hpp"
#include "core/Workspace.hpp"

#include <QApplication>

int main(int argc, char** argv)
{
    QApplication app{ argc, argv };

    Workspace workspace;
    Settings settings{ loadSettings() };

    MainWindow mainWindow{ workspace, settings };
    mainWindow.show();

    const int appResult{ app.exec() };

    if (appResult == 0) {
        saveSettings(settings);
    }

    return appResult;
}
