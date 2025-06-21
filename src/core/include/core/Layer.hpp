#pragma once

#include "BoundingBox.hpp"
#include "Circle.hpp"
#include "Line.hpp"

#include <optional>
#include <string>
#include <vector>

struct Layer
{
    explicit Layer(std::string name)
        : name{ std::move(name) }
    {
    }

    std::string name;
    bool visible{ true };
    std::vector<Line> lines;
    std::vector<Circle> circles;
};

std::optional<BoundingBox> boundingBox(const Layer& layer);
