#pragma once

#include "BoundingBox.hpp"
#include "Coordinate.hpp"

#include <vector>

struct Circle final
{
    Coordinate center;
    double radius;
};

using Circles = std::vector<Circle>;

BoundingBox boundingBox(const Circle& circle);

Coordinate nearestPoint(const Circle& circle, const Coordinate& position);
