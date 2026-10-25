#include "domain/Color.hpp"

#include "core/Overload.hpp"

namespace fcad::domain {

Color resolved(const EntityColor& entityColor, const Color& layerColor)
{
    return std::visit(
        core::Overload{
            [&layerColor](const ByLayer& /* byLayer */) { return layerColor; },
            [](const Rgb& rgb) { return Color{ rgb }; },
            [](const Adaptive& adaptive) { return Color{ adaptive }; },
        },
        entityColor
    );
}

}   // namespace fcad::domain
