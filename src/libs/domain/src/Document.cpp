#include "domain/Document.hpp"

#include "core/VectorExtensions.hpp"

#include <ranges>

namespace fcad::domain {

const Layers& Document::layers() const& { return m_layers; }

void Document::addLayer(Layer layer) { m_layers.push_back(std::move(layer)); }

void Document::addLayers(Layers&& layers) { core::appendRange(m_layers, std::move(layers)); }

std::optional<geo::BoundingBox> boundingBox(const Document& document)
{
    std::optional<geo::BoundingBox> bb;

    for (const Layer& layer : document.layers() | std::views::filter(&Layer::visible)) {
        bb = geo::united(bb, boundingBox(layer));
    }

    return bb;
}

}   // namespace fcad::domain
