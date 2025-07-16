#pragma once

#include <fmt/core.h>

#include <compare>
#include <concepts>
#include <optional>
#include <stdexcept>

template <std::floating_point F, typename Tag>
class NonNegative
{
public:
    NonNegative() = default;

    // TODO C++23: constexpr
    explicit NonNegative(F value)
        : m_value{ value }
    {
        if (value < F{}) {
            throw std::invalid_argument{ fmt::format("value is negative: {}", value) };
        }
    }

    [[nodiscard]] static constexpr std::optional<NonNegative> fromValue(F value) noexcept
    {
        if (value < F{}) {
            return std::nullopt;
        }

        NonNegative result;
        result.m_value = value;

        return result;
    }

    constexpr auto operator<=>(const NonNegative&) const = default;

    constexpr operator F() const noexcept { return m_value; }

    constexpr F value() const noexcept { return m_value; }

private:
    F m_value{};
};
