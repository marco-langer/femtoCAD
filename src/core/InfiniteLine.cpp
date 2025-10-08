#include "core/InfiniteLine.hpp"

std::optional<Coordinate> intersection(const InfiniteLine& first, const InfiniteLine& second)
{
    const Vector2d& u{ first.direction };
    const Vector2d& v{ second.direction };
    const Vector2d w{ first.point - second.point };

    const double denominator{ v.x * u.y - v.y * u.x };

    if (denominator == 0.0) {
        return std::nullopt;
    }

    const double numerator{ v.y * w.x - v.x * w.y };

    const double s{ numerator / denominator };

    return first.point + s * first.direction;
}
