#pragma once

#include "domain/Layer.hpp"
#include "geo/Arc.hpp"
#include "geo/Circle.hpp"
#include "geo/Coordinate.hpp"
#include "geo/Line.hpp"

#include <opendxf/coordinate.hpp>
#include <opendxf/entities.hpp>
#include <opendxf/layer.hpp>

#include <optional>

namespace fcad::io {

// The functions taking a scale multiply all coordinates and lengths by it.

geo::Coordinate toCoordinate(const odxf::Coordinate3d& coordinate3d, double scale);

domain::Layer toLayer(const odxf::Layer& dxfLayer);

geo::Line toLine(const odxf::Line& dxfLine, double scale);

// Empty if the radius is negative.
std::optional<geo::Circle> toCircle(const odxf::Circle& dxfCircle, double scale);

// Empty if the radius is negative.
std::optional<geo::Arc> toArc(const odxf::Arc& dxfArc, double scale);

}   // namespace fcad::io
