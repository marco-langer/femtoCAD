#pragma once

#include "core/Result.hpp"
#include "domain/Layer.hpp"
#include "domain/LengthUnit.hpp"

#include <QString>

#include <optional>

namespace fcad::io {

struct ImportedDxf final
{
    domain::Layers layers;
    // Empty if the file doesn't specify a unit.
    std::optional<domain::LengthUnit> unit;
};

core::Result<ImportedDxf> importDxf(const QString& filePath);

}   // namespace fcad::io
