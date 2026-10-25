#pragma once

#include "domain/Color.hpp"
#include "domain/Entity.hpp"
#include "geo/BoundingBox.hpp"

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
    Arcs arcs;
    Lines lines;
    Circles circles;
};

using Layers = std::vector<Layer>;

std::optional<geo::BoundingBox> boundingBox(const Layer& layer);

}   // namespace fcad::domain
