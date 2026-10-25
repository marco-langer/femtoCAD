#include "domain/Document.hpp"

#include <ranges>

namespace fcad::domain {

Document::Document(LengthUnit unit)
    : m_unit{ unit }
{}

LengthUnit Document::unit() const { return m_unit; }

const Layers& Document::layers() const& { return m_layers; }

void Document::addLayer(Layer layer) { m_layers.push_back(std::move(layer)); }

void Document::addLayers(Layers&& layers) { m_layers.append_range(layers | std::views::as_rvalue); }

std::optional<geo::BoundingBox> boundingBox(const Document& document)
{
    std::optional<geo::BoundingBox> bb;

    for (const Layer& layer : document.layers() | std::views::filter(&Layer::isVisible)) {
        bb = geo::united(bb, boundingBox(layer));
    }

    return bb;
}

}   // namespace fcad::domain
