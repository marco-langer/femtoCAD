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
