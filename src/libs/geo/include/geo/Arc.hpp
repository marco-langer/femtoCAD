#pragma once

#include "core/Angle.hpp"
#include "geo/BoundingBox.hpp"
#include "geo/Coordinate.hpp"
#include "geo/Radius.hpp"

#include <vector>

namespace fcad::geo {

struct Arc final
{
    Coordinate center;
    core::Radians startAngle;
    core::Radians endAngle;
    Radius radius;
};

using Arcs = std::vector<Arc>;

BoundingBox boundingBox(const Arc& arc);

}   // namespace fcad::geo
