#pragma once

#include "core/BoundedValue.hpp"
#include "domain/ByLayer.hpp"

#include <variant>

namespace fcad::domain {

// The width a line is drawn with, in hundredths of a millimeter. It's a width on screen or paper,
// so it doesn't depend on the document's length unit or the zoom.
using Lineweight = core::BoundedValue<int, 0, 211, struct LineweightTag>;

// The default lineweight. It's an application setting, not part of the document, so whoever draws
// it picks the actual width.
struct DefaultLineweight final
{
    bool operator==(const DefaultLineweight&) const = default;
};

using LayerLineweight = std::variant<DefaultLineweight, Lineweight>;

using EntityLineweight = std::variant<ByLayer, DefaultLineweight, Lineweight>;

// The lineweight an entity is drawn with: its own, or for ByLayer its layer's.
LayerLineweight
resolved(const EntityLineweight& entityLineweight, const LayerLineweight& layerLineweight);

}   // namespace fcad::domain
