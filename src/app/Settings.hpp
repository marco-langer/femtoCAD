#pragma once

#include <QString>

#include <optional>

struct Settings
{
    std::optional<QString> lastDxfImportPath;
};

Settings loadSettings();

void saveSettings(const Settings& settings);
