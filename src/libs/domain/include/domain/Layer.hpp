#pragma once

#include "domain/Color.hpp"
#include "geo/Arc.hpp"
#include "geo/BoundingBox.hpp"
#include "geo/Circle.hpp"
#include "geo/Line.hpp"

#include <QString>

#include <optional>
#include <vector>

namespace fcad::domain {

struct Layer final
{
    explicit Layer(QString layerName)
        : name{ std::move(layerName) }
    {}

    QString name;
    bool visible{ true };
    Color color{ Adaptive{} };
    geo::Arcs arcs;
    std::vector<geo::Line> lines;
    geo::Circles circles;
};

using Layers = std::vector<Layer>;

std::optional<geo::BoundingBox> boundingBox(const Layer& layer);

}   // namespace fcad::domain
