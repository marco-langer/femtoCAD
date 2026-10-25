#pragma once

#include <cstdint>
#include <variant>

namespace fcad::domain {

// A fixed color, 8 bits per channel.
struct Rgb final
{
    std::uint8_t r{};
    std::uint8_t g{};
    std::uint8_t b{};

    bool operator==(const Rgb&) const = default;
};

// A color that contrasts with whatever it's drawn on, e.g. white on a dark background and black on
// a light one. Whoever draws it picks the actual color.
struct Adaptive final
{
    bool operator==(const Adaptive&) const = default;
};

using Color = std::variant<Rgb, Adaptive>;

// The entity's layer's value, e.g. its color.
struct ByLayer final
{
    bool operator==(const ByLayer&) const = default;
};

using EntityColor = std::variant<ByLayer, Rgb, Adaptive>;

// The color an entity is drawn in: its own, or for ByLayer its layer's.
Color resolved(const EntityColor& entityColor, const Color& layerColor);

}   // namespace fcad::domain
