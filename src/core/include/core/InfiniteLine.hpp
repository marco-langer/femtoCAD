#pragma once

#include "Coordinate.hpp"

#include <optional>

struct InfiniteLine final
{
    Coordinate point;
    Vector2d direction;
};

std::optional<Coordinate> intersection(const InfiniteLine& first, const InfiniteLine& second);
