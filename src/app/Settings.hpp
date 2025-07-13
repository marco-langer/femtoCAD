#pragma once

#include <QString>

#include <optional>

struct Settings final
{
    std::optional<QString> lastDxfImportPath;
};

Settings loadSettings();

void saveSettings(const Settings& settings);
