#include "domain/Workspace.hpp"

#include "core/VectorExtensions.hpp"

#include <ranges>

namespace fcad::domain {

void Workspace::clear() { m_layers.clear(); }

const Layers& Workspace::layers() const& { return m_layers; }

void Workspace::addLayer(Layer layer) { m_layers.push_back(std::move(layer)); }

void Workspace::addLayers(Layers&& layers) { core::appendRange(m_layers, std::move(layers)); }

std::optional<geo::BoundingBox> boundingBox(const Workspace& workspace)
{
    std::optional<geo::BoundingBox> bb;

    for (const Layer& layer : workspace.layers() | std::views::filter(&Layer::visible)) {
        bb = geo::united(bb, boundingBox(layer));
    }

    return bb;
}

}   // namespace fcad::domain
