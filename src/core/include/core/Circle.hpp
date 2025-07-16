#pragma once

#include "BoundingBox.hpp"
#include "Coordinate.hpp"
#include "primitives/Radius.hpp"

#include <vector>

struct Circle final
{
    Coordinate center;
    Radius radius;
};

using Circles = std::vector<Circle>;

BoundingBox boundingBox(const Circle& circle);

Coordinate nearestPoint(const Circle& circle, const Coordinate& position);
