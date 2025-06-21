#pragma once

#include "BoundingBox.hpp"
#include "Layer.hpp"

#include <optional>
#include <vector>

class Workspace
{
public:
    void clear();

    const std::vector<Layer>& layers() const;
    void addLayer(Layer layer);

private:
    std::vector<Layer> m_layers;
};

std::optional<BoundingBox> boundingBox(const Workspace& workspace);
