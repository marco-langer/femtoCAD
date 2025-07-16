#pragma once

#include "core/BoundingBox.hpp"
#include "core/Coordinate.hpp"
#include "core/primitives/Radius.hpp"
#include "core/units/Angle.hpp"

#include <vector>

struct Arc final
{
    Coordinate center;
    Radians startAngle;
    Radians endAngle;
    Radius radius;
};

using Arcs = std::vector<Arc>;

BoundingBox boundingBox(const Arc& arc);
