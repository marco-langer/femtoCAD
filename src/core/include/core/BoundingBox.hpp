#pragma once

#include "Coordinate.hpp"

#include <concepts>
#include <optional>

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

std::optional<BoundingBox>
united(const std::optional<BoundingBox>& first, const std::optional<BoundingBox>& second);

template <typename... Ts>
requires (std::same_as<std::optional<BoundingBox>, Ts>&&...) std::optional<BoundingBox> united(
    const std::optional<BoundingBox>& first,
    const std::optional<BoundingBox>& second,
    const Ts&... rest
)
{
    std::optional<BoundingBox> bb{ united(first, second) };

    if constexpr (sizeof...(Ts) > 0) {
        bb = united(bb, united(second, rest...));
    }

    return bb;
}

bool contains(const BoundingBox& boundingBox, const Coordinate& point);
