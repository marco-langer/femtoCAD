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

}   // namespace fcad::domain
