#pragma once

#include "geo/Coordinate.hpp"

#include <optional>

namespace fcad::geo {

struct InfiniteLine final
{
    Coordinate point;
    Vector2d direction;
};

std::optional<Coordinate> intersection(const InfiniteLine& first, const InfiniteLine& second);

}   // namespace fcad::geo
