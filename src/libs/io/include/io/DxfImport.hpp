#pragma once

#include "core/Result.hpp"
#include "domain/Layer.hpp"
#include "domain/LengthUnit.hpp"

#include <QString>
#include <QStringList>

#include <optional>

namespace fcad::io {

struct ImportedDxf final
{
    domain::Layers layers;
    // Empty if the file doesn't specify a unit. A unit femtoCAD doesn't support is converted to
    // the closest supported one, and the layers are scaled accordingly.
    std::optional<domain::LengthUnit> unit;
    // Things the user should know about the import, e.g. a converted unit.
    QStringList warnings;
};

core::Result<ImportedDxf> importDxf(const QString& filePath);

}   // namespace fcad::io
