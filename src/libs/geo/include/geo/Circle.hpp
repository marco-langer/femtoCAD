#pragma once

#include "geo/BoundingBox.hpp"
#include "geo/Coordinate.hpp"
#include "geo/Radius.hpp"

#include <optional>
#include <vector>

namespace fcad::geo {

struct Circle final
{
    Coordinate center;
    Radius radius;
};

using Circles = std::vector<Circle>;

BoundingBox boundingBox(const Circle& circle);

Coordinate nearestPoint(const Circle& circle, const Coordinate& position);

std::optional<Circle>
circleFromPoints(const Coordinate& first, const Coordinate& second, const Coordinate& third);

}   // namespace fcad::geo
