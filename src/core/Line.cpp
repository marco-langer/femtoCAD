#include "core/Line.hpp"

BoundingBox boundingBox(const Line& line)
{
    BoundingBox bb;

    bb.min.x = line.start.x < line.end.x ? line.start.x : line.end.x;
    bb.min.y = line.start.y < line.end.y ? line.start.y : line.end.y;
    bb.max.x = line.start.x > line.end.x ? line.start.x : line.end.x;
    bb.max.y = line.start.y > line.end.y ? line.start.y : line.end.y;

    return bb;
}

Coordinate mid(const Line& line)
{
    return Coordinate{
        .x = line.start.x + 0.5 * (line.end.x - line.start.x),
        .y = line.start.y + 0.5 * (line.end.y - line.start.y),
    };
}

std::optional<InfiniteLine> perpBisector(const Line& line)
{
    const Coordinate midPoint{ mid(line) };

    const Vector2d lineDirection{ line.end - line.start };
    const double length{ norm(lineDirection) };
    if (length == 0.0) {
        return std::nullopt;
    }

    return InfiniteLine{ .point = midPoint, .direction = perp(lineDirection) / length };
}
