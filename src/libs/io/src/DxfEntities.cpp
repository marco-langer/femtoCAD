#include "DxfEntities.hpp"

#include "DxfColors.hpp"
#include "DxfLineweights.hpp"

#include "core/Angle.hpp"

namespace fcad::io {

geo::Coordinate toCoordinate(const odxf::coordinate3d& coordinate3d, double scale)
{
    return geo::Coordinate{ .x = coordinate3d.x * scale, .y = coordinate3d.y * scale };
}

domain::Layer toLayer(const odxf::layer& dxfLayer)
{
    domain::Layer layer{ QString::fromStdString(dxfLayer.name) };

    // A negative color index means the layer is off. Values below -255 aren't colors anyway, and
    // leaving them negative avoids overflowing on the most negative int.
    const bool isOff{ dxfLayer.color < 0 };
    const int colorIndex{ (dxfLayer.color >= -255 && isOff) ? -dxfLayer.color : dxfLayer.color };
    const bool isFrozen{ (dxfLayer.flags & odxf::layer::frozen) != 0 };

    layer.setVisible(!isFrozen && !isOff);
    layer.setColor(toColor(colorIndex, dxfLayer.true_color));
    layer.setLineweight(toLayerLineweight(dxfLayer.lineweight));

    return layer;
}

geo::Line toLine(const odxf::line& dxfLine, double scale)
{
    return geo::Line{
        .start = toCoordinate(dxfLine.start, scale),
        .end = toCoordinate(dxfLine.end, scale),
    };
}

std::optional<geo::Circle> toCircle(const odxf::circle& dxfCircle, double scale)
{
    const std::optional<geo::Radius> radius{ geo::Radius::fromValue(dxfCircle.radius * scale) };
    if (!radius.has_value()) {
        return std::nullopt;
    }

    geo::Circle circle;

    circle.center = toCoordinate(dxfCircle.center, scale);
    circle.radius = radius.value();

    return circle;
}

std::optional<geo::Arc> toArc(const odxf::arc& dxfArc, double scale)
{
    const std::optional<geo::Radius> radius{ geo::Radius::fromValue(dxfArc.radius * scale) };
    if (!radius.has_value()) {
        return std::nullopt;
    }

    geo::Arc arc;

    arc.center = toCoordinate(dxfArc.center, scale);
    arc.startAngle = core::Radians::fromDegrees(dxfArc.start_angle);
    arc.endAngle = core::Radians::fromDegrees(dxfArc.end_angle);
    arc.radius = radius.value();

    return arc;
}

}   // namespace fcad::io
