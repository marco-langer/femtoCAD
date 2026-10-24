#pragma once

#include "geo/BoundingBox.hpp"
#include "geo/Coordinate.hpp"
#include "geo/InfiniteLine.hpp"

#include <optional>

namespace fcad::geo {

struct Line final
{
    Coordinate start;
    Coordinate end;
};

BoundingBox boundingBox(const Line& line);

Coordinate mid(const Line& line);

std::optional<InfiniteLine> perpBisector(const Line& line);

}   // namespace fcad::geo
