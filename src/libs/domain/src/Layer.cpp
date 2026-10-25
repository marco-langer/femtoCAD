#include "domain/Layer.hpp"

#include <algorithm>
#include <ranges>

namespace fcad::domain {

namespace {

    template <typename Geometry>
    std::optional<geo::BoundingBox> boundingBox(const std::vector<Entity<Geometry>>& entities)
    {
        if (entities.empty()) {
            return std::nullopt;
        }

        geo::BoundingBox bb{ boundingBox(entities.front()) };

        for (const Entity<Geometry>& entity : entities | std::views::drop(1)) {
            bb = geo::united(bb, boundingBox(entity));
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
