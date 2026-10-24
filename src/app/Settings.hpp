#pragma once

#include <QString>

#include <optional>

namespace fcad::app {

struct Settings final
{
    std::optional<QString> lastDxfImportPath;
};

Settings loadSettings();

void saveSettings(const Settings& settings);

}   // namespace fcad::app
