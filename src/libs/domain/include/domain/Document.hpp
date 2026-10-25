#pragma once

#include "domain/Layer.hpp"
#include "domain/LengthUnit.hpp"
#include "domain/Linetype.hpp"
#include "domain/Linetypes.hpp"
#include "geo/BoundingBox.hpp"

#include <optional>

namespace fcad::domain {

class Document final
{
public:
    /** Only with the built-in linetypes. */
    explicit Document(LengthUnit unit);
    Document(LengthUnit unit, Linetypes linetypes);
    Document(const Document&) = delete;
    Document(Document&&) = default;
    ~Document() = default;

    Document& operator=(const Document&) = delete;
    Document& operator=(Document&&) = default;

    // The unit of all coordinates and lengths in the document.
    LengthUnit unit() const;

    const Linetypes& linetypes() const&;

    /** Scales the dash patterns of all linetypes in the document. */
    LinetypeScale linetypeScale() const;
    void setLinetypeScale(LinetypeScale scale);

    const Layers& layers() const&;

    /**
     * @pre The layer's linetype is in the document.
     * @pre The linetypes of the layer's entities are in the document, unless they are ByLayer.
     */
    void addLayer(Layer layer);

    /**
     * @pre The linetype of each layer is in the document.
     * @pre The linetypes of the layers' entities are in the document, unless they are ByLayer.
     */
    void addLayers(Layers&& layers);

private:
    // ByLayer counts as contained, since it refers to no linetype.
    bool contains(const EntityLinetype& linetype) const;

    LengthUnit m_unit;
    Linetypes m_linetypes;
    LinetypeScale m_linetypeScale{ 1.0 };
    Layers m_layers;
};

std::optional<geo::BoundingBox> boundingBox(const Document& document);

}   // namespace fcad::domain
