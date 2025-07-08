#include "core/Layer.hpp"

#include <algorithm>
#include <ranges>

namespace {

template <typename T>
std::optional<BoundingBox> boundingBox(const std::vector<T>& geometries)
{
    if (geometries.empty()) {
        return std::nullopt;
    }

    BoundingBox bb{ boundingBox(geometries.front()) };

    for (const T& geometry : geometries | std::views::drop(1)) {
        bb = united(bb, boundingBox(geometry));
    }

    return bb;
}

}   // namespace

std::optional<BoundingBox> boundingBox(const Layer& layer)
{
    return united(boundingBox(layer.lines), boundingBox(layer.circles));
};
