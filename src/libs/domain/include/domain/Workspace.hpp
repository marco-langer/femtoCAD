#pragma once

#include "domain/Layer.hpp"
#include "geo/BoundingBox.hpp"

#include <optional>
#include <vector>

namespace fcad::domain {

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

std::optional<geo::BoundingBox> boundingBox(const Workspace& workspace);

}   // namespace fcad::domain
