#pragma once

#include "BoundingBox.hpp"
#include "Layer.hpp"

#include <optional>
#include <vector>

class Workspace final
{
public:
    void clear();

    const Layers& layers() const&;
    void addLayer(Layer layer);
    void addLayers(Layers&& layers);

private:
    Layers m_layers;
};

std::optional<BoundingBox> boundingBox(const Workspace& workspace);
