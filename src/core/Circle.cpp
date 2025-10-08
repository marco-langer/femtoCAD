#include "core/Circle.hpp"

#include "core/Line.hpp"

BoundingBox boundingBox(const Circle& circle)
{
    return BoundingBox{ .min = Coordinate{ .x = circle.center.x - circle.radius,
                                           .y = circle.center.y - circle.radius },
                        .max = Coordinate{ .x = circle.center.x + circle.radius,
                                           .y = circle.center.y + circle.radius } };
}

Coordinate nearestPoint(const Circle& circle, const Coordinate& position)
{
    const Vector2d diff{ position - circle.center };
    const double distance{ norm(diff) };
    if (distance == 0.0) {
        return circle.center + Coordinate{ circle.radius, 0.0 };
    }

    const Vector2d direction{ diff / distance };

    return circle.center + direction * circle.radius;
}

std::optional<Circle>
circleFromPoints(const Coordinate& first, const Coordinate& second, const Coordinate& third)
{
    const Line chord1{ first, second };
    const Line chord2{ first, third };

    const std::optional<InfiniteLine> maybeBisector1{ perpBisector(chord1) };
    const std::optional<InfiniteLine> maybeBisector2{ perpBisector(chord2) };
    if (!maybeBisector1.has_value() || !maybeBisector2.has_value()) {
        return std::nullopt;
    }

    const std::optional<Coordinate> maybeIntersection{
        intersection(maybeBisector1.value(), maybeBisector2.value())
    };
    if (!maybeIntersection.has_value()) {
        return std::nullopt;
    }

    const Coordinate& center{ maybeIntersection.value() };
    const double radius{ norm(center - first) };

    return Circle{ .center = center, .radius = Radius{ radius } };
}
