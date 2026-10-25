#pragma once

#include "domain/LengthUnit.hpp"

#include <opendxf/headervariables.hpp>

#include <QString>

#include <optional>

namespace fcad::io {

// The supported unit a file's values are imported in.
struct UnitConversion final
{
    domain::LengthUnit unit;
    // Converts a value in the file's unit into unit.
    double factor{ 1.0 };
};

// Empty if the unit isn't specified.
std::optional<UnitConversion> toUnitConversion(odxf::LengthUnit dxfUnit);

// Names for messages, e.g. "decameters".
QString toName(odxf::LengthUnit dxfUnit);
QString toName(domain::LengthUnit unit);

}   // namespace fcad::io
