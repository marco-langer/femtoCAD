#include "domain/Lineweight.hpp"

#include "core/Overload.hpp"

namespace fcad::domain {

LayerLineweight
resolved(const EntityLineweight& entityLineweight, const LayerLineweight& layerLineweight)
{
    return std::visit(
        core::Overload{
            [&layerLineweight](const ByLayer& /* byLayer */) { return layerLineweight; },
            [](const DefaultLineweight& defaultLineweight) {
                return LayerLineweight{ defaultLineweight };
            },
            [](const Lineweight& lineweight) { return LayerLineweight{ lineweight }; },
        },
        entityLineweight
    );
}

}   // namespace fcad::domain
