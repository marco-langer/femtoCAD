#pragma once

#include <gsl/narrow>

#include <cmath>
#include <compare>
#include <concepts>
#include <numbers>

enum class AngleUnit { Radian, Degree };

template <AngleUnit Unit, std::floating_point F>
[[nodiscard]] constexpr F normalized(F value) noexcept
{
    constexpr F twoPi{ [] {
        if constexpr (Unit == AngleUnit::Degree) {
            return F{ 360.0 };
        } else {
            return 2 * std::numbers::pi_v<F>;
        }
    }() };

    if (value >= F{} && value <= twoPi) {
        return value;
    }

    const F rest{ std::fmod(value, twoPi) };

    return rest > F{} ? rest : rest + twoPi;
}

template <AngleUnit Unit, std::floating_point F>
class BasicAngle final
{
public:
    BasicAngle() = default;

    constexpr explicit BasicAngle(std::floating_point auto value) noexcept
        : m_value{ normalized<Unit>(gsl::narrow_cast<F>(value)) }
    {}

    template <AngleUnit Unit2>
    requires(Unit2 != Unit) constexpr explicit BasicAngle(const BasicAngle<Unit2, F>& other
    ) noexcept
    {
        if constexpr (Unit2 == AngleUnit::Degree) {
            m_value = other.m_value / F{ 180.0 } * std::numbers::pi_v<F>;
        } else {
            m_value = other.m_value / std::numbers::pi_v<F> * F{ 180.0 };
        }
    }

    [[nodiscard]] constexpr auto operator<=>(const BasicAngle&) const = default;

    [[nodiscard]] static constexpr BasicAngle fromDegrees(F value) noexcept
    {
        const F normalizedValue{ normalized<AngleUnit::Degree>(value) };

        BasicAngle angle;
        if constexpr (Unit == AngleUnit::Radian) {
            angle.m_value = normalizedValue / F{ 180.0 } * std::numbers::pi_v<F>;
        } else {
            angle.m_value = normalizedValue;
        }

        return angle;
    }

    [[nodiscard]] constexpr F value() const noexcept { return m_value; }

    static constexpr BasicAngle zero{};

    static constexpr BasicAngle piHalf{ [] {
        BasicAngle angle;

        if constexpr (Unit == AngleUnit::Radian) {
            angle.m_value = std::numbers::pi_v<F> / F{ 2.0 };
        } else {
            angle.m_value = F{ 900.0 };
        }

        return angle;
    }() };

    static constexpr BasicAngle pi{ [] {
        BasicAngle angle;

        if constexpr (Unit == AngleUnit::Radian) {
            angle.m_value = std::numbers::pi_v<F>;
        } else {
            angle.m_value = F{ 180.0 };
        }

        return angle;
    }() };

    static constexpr BasicAngle piThreeHalf{ [] {
        BasicAngle angle;

        if constexpr (Unit == AngleUnit::Radian) {
            angle.m_value = F{ 3.0 } / 2 * std::numbers::pi_v<F>;
        } else {
            angle.m_value = F{ 270.0 };
        }

        return angle;
    }() };

private:
    template <AngleUnit Unit2, std::floating_point F2>
    friend class BasicAngle;

    F m_value{};
};

template <AngleUnit Unit>
using Angle = BasicAngle<Unit, double>;

using Degrees = Angle<AngleUnit::Degree>;
using Radians = Angle<AngleUnit::Radian>;

[[nodiscard]] inline double sin(Radians angle) { return std::sin(angle.value()); }

[[nodiscard]] inline double cos(Radians angle) { return std::cos(angle.value()); }
