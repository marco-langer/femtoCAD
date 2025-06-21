#include "MainWindow.hpp"
#include "core/Workspace.hpp"

#include <QApplication>

int main(int argc, char** argv)
{
    QApplication app{ argc, argv };

    Workspace workspace;

    MainWindow mainWindow{ workspace };
    mainWindow.show();

    return app.exec();
}
