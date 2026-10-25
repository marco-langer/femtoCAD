#pragma once

#include "domain/LengthUnit.hpp"

#include <QString>

#include <optional>

namespace fcad::app {

struct DxfImportSettings final
{
    std::optional<QString> lastImportPath;
    // The unit for files that don't specify one.
    domain::LengthUnit fallbackUnit{ domain::LengthUnit::Millimeter };
    // Use fallbackUnit without asking the user.
    bool useFallbackUnit{ false };
};

struct Settings final
{
    // The unit of new documents.
    domain::LengthUnit defaultUnit{ domain::LengthUnit::Millimeter };
    DxfImportSettings dxfImport;
};

Settings loadSettings();

void saveSettings(const Settings& settings);

}   // namespace fcad::app
