#pragma once

#include "Coordinate.hpp"

struct BoundingBox
{
    Coordinate min;
    Coordinate max;
};

inline double getWidth(const BoundingBox& boundingBox)
{
    return boundingBox.max.x - boundingBox.min.x;
}

inline double getHeight(const BoundingBox& boundingBox)
{
    return boundingBox.max.y - boundingBox.min.y;
}

BoundingBox united(const BoundingBox& first, const BoundingBox& second);

bool contains(const BoundingBox& boundingBox, const Coordinate& point);
