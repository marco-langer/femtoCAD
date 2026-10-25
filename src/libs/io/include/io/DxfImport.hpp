#pragma once

#include "core/Result.hpp"
#include "domain/Document.hpp"
#include "domain/Layer.hpp"
#include "domain/LengthUnit.hpp"
#include "domain/Linetype.hpp"
#include "domain/Linetypes.hpp"

#include <QString>
#include <QStringList>

#include <optional>

namespace fcad::io {

struct ImportedDxf final
{
    // The file's linetypes, with the built-in Continuous. The layers refer to them.
    domain::Linetypes linetypes;
    domain::LinetypeScale linetypeScale{ 1.0 };
    domain::Layers layers;
    // Empty if the file doesn't specify a unit. A unit femtoCAD doesn't support is converted to
    // the closest supported one, and the layers and linetypes are scaled accordingly.
    std::optional<domain::LengthUnit> unit;
    // Things the user should know about the import, e.g. a converted unit.
    QStringList warnings;
};

core::Result<ImportedDxf> importDxf(const QString& filePath);

/**
 * The document of an imported file: its linetypes, linetype scale and layers, in unit. That's the
 * file's unit, or for a file without one the unit the user picked. The warnings aren't used.
 */
domain::Document toDocument(ImportedDxf&& imported, domain::LengthUnit unit);

}   // namespace fcad::io
