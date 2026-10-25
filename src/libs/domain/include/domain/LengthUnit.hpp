#pragma once

#include <array>

namespace fcad::domain {

enum class LengthUnit {
    Micrometer,
    Millimeter,
    Centimeter,
    Decimeter,
    Meter,
    Kilometer,
    Inch,
    Foot,
    Yard,
    Mile,
};

// All length units, in the order of the enum: metric, then imperial, each by size.
inline constexpr std::array lengthUnits{
    LengthUnit::Micrometer, LengthUnit::Millimeter, LengthUnit::Centimeter, LengthUnit::Decimeter,
    LengthUnit::Meter,      LengthUnit::Kilometer,  LengthUnit::Inch,       LengthUnit::Foot,
    LengthUnit::Yard,       LengthUnit::Mile,
};

}   // namespace fcad::domain
