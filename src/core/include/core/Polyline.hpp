#pragma once

#include "Arc.hpp"
#include "Coordinate.hpp"
#include "Line.hpp"

#include <optional>
#include <variant>
#include <vector>

struct Vertex final
{
    Coordinate position;
    double bulge{ 0.0 };
};

struct Polyline final
{
    std::vector<Vertex> vertices;
    bool closed{ false };
};

using Polylines = std::vector<Polyline>;
using PolylineSegment = std::variant<Line, Arc>;

std::vector<PolylineSegment> explode(const Polyline& polyline);
