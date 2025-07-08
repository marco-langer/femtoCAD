#include "Settings.hpp"

#include <QSettings>

Settings loadSettings()
{
    Settings settings;

    QSettings qsettings{ "femtoCAD", "femtoCAD" };

    QVariant lastDxfPath{ qsettings.value("lastDxfImport") };
    if (lastDxfPath.canConvert<QString>()) {
        settings.lastDxfImportPath = lastDxfPath.toString();
    }

    return settings;
}

void saveSettings(const Settings& settings)
{
    QSettings qsettings{ "femtoCAD", "femtoCAD" };

    if (settings.lastDxfImportPath.has_value()) {
        qsettings.setValue("lastDxfImport", settings.lastDxfImportPath.value());
    }
}
