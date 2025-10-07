#include "core/Polyline.hpp"

namespace {

PolylineSegment createSegment(const Vertex& first, const Vertex& second)
{
    if (first.bulge == 0.0) {
        return Line{ .start = first.position, .end = second.position };
    }

    return {};
}

}   // namespace

std::vector<PolylineSegment> explode(const Polyline& polyline)
{
    if (polyline.vertices.empty()) {
        return {};
    }

    std::vector<PolylineSegment> entities;
    entities.reserve(polyline.vertices.size() + polyline.closed ? 1 : 0);

    for (std::size_t i = 0; i < polyline.vertices.size() - 1; ++i) {
        const Vertex& first{ polyline.vertices[i] };
        const Vertex& second{ polyline.vertices[i + 1] };

        entities.push_back(createSegment(first, second));
    }

    if (polyline.closed) {
        entities.push_back(createSegment(polyline.vertices.back(), polyline.vertices.front()));
    }

    return entities;
}
