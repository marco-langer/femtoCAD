#pragma once

#include "BoundingBox.hpp"
#include "Coordinate.hpp"

struct Line
{
    Coordinate start;
    Coordinate end;
};

BoundingBox boundingBox(const Line& line);
