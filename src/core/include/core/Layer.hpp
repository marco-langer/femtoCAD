#pragma once

#include "Arc.hpp"
#include "BoundingBox.hpp"
#include "Circle.hpp"
#include "Line.hpp"

#include <QString>

#include <optional>
#include <vector>

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
