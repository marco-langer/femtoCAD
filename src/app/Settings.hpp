#pragma once

#include "domain/LengthUnit.hpp"
#include "domain/Lineweight.hpp"

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

struct ViewSettings final
{
    // Draw the x and y axes through the origin.
    bool showAxes{ true };
    // Draw the dot grid.
    bool showGrid{ true };
    // Draw lines with their lineweight. Otherwise all lines are equally thin.
    bool showLineweights{ true };
};

struct Settings final
{
    // The unit of new documents.
    domain::LengthUnit defaultUnit{ domain::LengthUnit::Millimeter };
    // What DefaultLineweight stands for.
    domain::Lineweight defaultLineweightWidth{ 25 };
    DxfImportSettings dxfImport;
    ViewSettings view;
};

Settings loadSettings();

void saveSettings(const Settings& settings);

}   // namespace fcad::app
