#pragma once

#include "BoundingBox.hpp"
#include "Coordinate.hpp"
#include "primitives/Radius.hpp"

#include <optional>
#include <vector>

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
