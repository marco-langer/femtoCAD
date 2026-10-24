#include "domain/Layer.hpp"

#include <algorithm>
#include <ranges>

namespace fcad::domain {

namespace {

    template <typename T>
    std::optional<geo::BoundingBox> boundingBox(const std::vector<T>& geometries)
    {
        if (geometries.empty()) {
            return std::nullopt;
        }

        geo::BoundingBox bb{ geo::boundingBox(geometries.front()) };

        for (const T& geometry : geometries | std::views::drop(1)) {
            bb = geo::united(bb, geo::boundingBox(geometry));
        }

        return bb;
    }

}   // namespace

std::optional<geo::BoundingBox> boundingBox(const Layer& layer)
{
    return geo::united(
        boundingBox(layer.arcs), boundingBox(layer.lines), boundingBox(layer.circles)
    );
};

}   // namespace fcad::domain
