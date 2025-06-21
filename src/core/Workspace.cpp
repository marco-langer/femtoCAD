#include "core/Workspace.hpp"

#include <ranges>

void Workspace::clear() { m_layers.clear(); }

const std::vector<Layer>& Workspace::layers() const { return m_layers; }

void Workspace::addLayer(Layer layer) { m_layers.push_back(std::move(layer)); }

std::optional<BoundingBox> boundingBox(const Workspace& workspace)
{
    std::optional<BoundingBox> bb;

    for (const Layer& layer : workspace.layers() | std::views::filter(&Layer::visible)) {
        const std::optional<BoundingBox> layerBb{ boundingBox(layer) };
        if (!bb) {
            bb = layerBb;
        } else if (layerBb) {
            bb = united(bb.value(), layerBb.value());
        }
    }

    return bb;
}
