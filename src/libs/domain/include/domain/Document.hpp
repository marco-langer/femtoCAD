#pragma once

#include "domain/Layer.hpp"
#include "domain/LengthUnit.hpp"
#include "geo/BoundingBox.hpp"

#include <optional>

namespace fcad::domain {

class Document final
{
public:
    explicit Document(LengthUnit unit);
    Document(const Document&) = delete;
    Document(Document&&) = default;
    ~Document() = default;

    Document& operator=(const Document&) = delete;
    Document& operator=(Document&&) = default;

    // The unit of all coordinates and lengths in the document.
    LengthUnit unit() const;

    const Layers& layers() const&;
    void addLayer(Layer layer);
    void addLayers(Layers&& layers);

private:
    LengthUnit m_unit;
    Layers m_layers;
};

std::optional<geo::BoundingBox> boundingBox(const Document& document);

}   // namespace fcad::domain
