#include "core/Circle.hpp"

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
