#pragma once

#include "core/BoundingBox.hpp"
#include "core/Coordinate.hpp"
#include "core/units/Angle.hpp"

#include <vector>

struct Arc final
{
    Coordinate center;
    Radians startAngle;
    Radians endAngle;
    double radius{ 0.0 };
};

using Arcs = std::vector<Arc>;

BoundingBox boundingBox(const Arc& arc);
