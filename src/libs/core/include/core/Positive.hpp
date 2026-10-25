#pragma once

#include <cmath>
#include <compare>
#include <concepts>
#include <optional>
#include <stdexcept>

namespace fcad::core {

/** A finite floating-point value greater than 0. */
template <std::floating_point F, typename Tag>
class Positive final
{
public:
    consteval explicit Positive(F value)
        : m_value{ value }
    {
        if (!isPositive(value)) {
            throw std::invalid_argument{ "Positive: value isn't finite and greater than 0" };
        }
    }

    [[nodiscard]] static constexpr std::optional<Positive> fromValue(F value) noexcept
    {
        if (!isPositive(value)) {
            return std::nullopt;
        }

        return Positive{ Unchecked{}, value };
    }

    constexpr auto operator<=>(const Positive&) const = default;

    constexpr F value() const noexcept { return m_value; }

private:
    struct Unchecked
    {
    };

    constexpr Positive(Unchecked, F value) noexcept
        : m_value{ value }
    {}

    static constexpr bool isPositive(F value) noexcept
    {
        return value > F{} && std::isfinite(value);
    }

    F m_value;
};

}   // namespace fcad::core
