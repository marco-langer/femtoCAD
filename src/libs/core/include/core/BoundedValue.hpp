#pragma once

#include <algorithm>
#include <compare>
#include <concepts>
#include <optional>
#include <stdexcept>

namespace fcad::core {

// An integer that always lies in [Min, Max].
template <std::integral T, T Min, T Max, typename Tag>
    requires(Min <= Max)
class BoundedValue final
{
public:
    static constexpr T min{ Min };
    static constexpr T max{ Max };

    consteval explicit BoundedValue(T value)
        : m_value{ value }
    {
        if (!isInRange(value)) {
            throw std::invalid_argument{ "BoundedValue: value is out of range" };
        }
    }

    [[nodiscard]] static constexpr std::optional<BoundedValue> fromValue(T value) noexcept
    {
        if (!isInRange(value)) {
            return std::nullopt;
        }

        return BoundedValue{ Unchecked{}, value };
    }

    [[nodiscard]] static constexpr BoundedValue clamped(T value) noexcept
    {
        return BoundedValue{ Unchecked{}, std::clamp(value, Min, Max) };
    }

    constexpr auto operator<=>(const BoundedValue&) const = default;

    constexpr T value() const noexcept { return m_value; }

private:
    struct Unchecked
    {
    };

    constexpr BoundedValue(Unchecked, T value) noexcept
        : m_value{ value }
    {}

    static constexpr bool isInRange(T value) noexcept { return value >= Min && value <= Max; }

    T m_value;
};

}   // namespace fcad::core
