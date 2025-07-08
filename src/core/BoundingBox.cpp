#include "core/BoundingBox.hpp"

BoundingBox united(const BoundingBox& first, const BoundingBox& second)
{
    return BoundingBox{
        .min =
            Coordinate{
                .x = first.min.x < second.min.x ? first.min.x : second.min.x,
                .y = first.min.y < second.min.y ? first.min.y : second.min.y,
            },
        .max =
            Coordinate{
                .x = first.max.x > second.max.x ? first.max.x : second.max.x,
                .y = first.max.y > second.max.y ? first.max.y : second.max.y,
            },
    };
}

std::optional<BoundingBox>
united(const std::optional<BoundingBox>& first, const std::optional<BoundingBox>& second)
{
    if (first.has_value()) {
        if (second.has_value()) {
            return united(first.value(), second.value());
        }

        return first;
    }

    return second;
}

bool contains(const BoundingBox& boundingBox, const Coordinate& point)
{
    return boundingBox.min.x <= point.x && boundingBox.max.x >= point.x
        && boundingBox.min.y <= point.y && boundingBox.max.y >= point.y;
}
