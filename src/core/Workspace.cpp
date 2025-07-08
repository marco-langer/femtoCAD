#include "core/Workspace.hpp"

#include "core/utility/VectorExtensions.hpp"

#include <ranges>

void Workspace::clear() { m_layers.clear(); }

const std::vector<Layer>& Workspace::layers() const& { return m_layers; }

void Workspace::addLayer(Layer layer) { m_layers.push_back(std::move(layer)); }

void Workspace::addLayers(std::vector<Layer>&& layers) { appendRange(m_layers, std::move(layers)); }

std::optional<BoundingBox> boundingBox(const Workspace& workspace)
{
    std::optional<BoundingBox> bb;

    for (const Layer& layer : workspace.layers() | std::views::filter(&Layer::visible)) {
        bb = united(bb, boundingBox(layer));
    }

    return bb;
}
