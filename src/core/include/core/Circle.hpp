#pragma once

#include "BoundingBox.hpp"
#include "Coordinate.hpp"

struct Circle
{
    Coordinate center;
    double radius;
};

BoundingBox boundingBox(const Circle& circle);

Coordinate nearestPoint(const Circle& circle, const Coordinate& position);
