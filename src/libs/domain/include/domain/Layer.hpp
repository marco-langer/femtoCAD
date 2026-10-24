#pragma once

#include "core/Arc.hpp"
#include "core/BoundingBox.hpp"
#include "core/Circle.hpp"
#include "core/Line.hpp"

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
    Arcs arcs;
    std::vector<Line> lines;
    Circles circles;
};

using Layers = std::vector<Layer>;

std::optional<BoundingBox> boundingBox(const Layer& layer);

}   // namespace fcad::domain
