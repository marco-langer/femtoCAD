#include "domain/Document.hpp"

#include <gsl/assert>

#include <ranges>
#include <utility>

namespace fcad::domain {

// TODO: Expects() terminates on a violated precondition. How programming errors are checked and
// reported is still an open decision of ADR 0003.

Document::Document(LengthUnit unit)
    : m_unit{ unit }
{}

Document::Document(LengthUnit unit, Linetypes linetypes)
    : m_unit{ unit }
    , m_linetypes{ std::move(linetypes) }
{}

LengthUnit Document::unit() const { return m_unit; }

const Linetypes& Document::linetypes() const& { return m_linetypes; }

LinetypeScale Document::linetypeScale() const { return m_linetypeScale; }

void Document::setLinetypeScale(LinetypeScale scale) { m_linetypeScale = scale; }

const Layers& Document::layers() const& { return m_layers; }

void Document::addLayer(Layer layer)
{
    Expects(m_linetypes.contains(layer.linetype()));

    m_layers.push_back(std::move(layer));
}

void Document::addLayers(Layers&& layers)
{
    for (Layer& layer : layers) {
        addLayer(std::move(layer));
    }
}

std::optional<geo::BoundingBox> boundingBox(const Document& document)
{
    std::optional<geo::BoundingBox> bb;

    for (const Layer& layer : document.layers() | std::views::filter(&Layer::isVisible)) {
        bb = geo::united(bb, boundingBox(layer));
    }

    return bb;
}

}   // namespace fcad::domain
