#pragma once

#include "BoundingBox.hpp"
#include "Coordinate.hpp"
#include "InfiniteLine.hpp"

#include <optional>

struct Line final
{
    Coordinate start;
    Coordinate end;
};

BoundingBox boundingBox(const Line& line);

Coordinate mid(const Line& line);

std::optional<InfiniteLine> perpBisector(const Line& line);
