#pragma once

#include "domain/Layer.hpp"
#include "geo/BoundingBox.hpp"

#include <optional>

namespace fcad::domain {

class Document final
{
public:
    Document() = default;
    Document(const Document&) = delete;
    Document(Document&&) = default;
    ~Document() = default;

    Document& operator=(const Document&) = delete;
    Document& operator=(Document&&) = default;

    const Layers& layers() const&;
    void addLayer(Layer layer);
    void addLayers(Layers&& layers);

private:
    Layers m_layers;
};

std::optional<geo::BoundingBox> boundingBox(const Document& document);

}   // namespace fcad::domain
