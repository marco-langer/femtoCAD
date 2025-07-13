#pragma once

#include <cassert>

struct Coordinate final
{
    double x{};
    double y{};
};

struct Vector2d final
{
    double x{};
    double y{};
};

double norm(const Vector2d& vector);

inline Coordinate operator+(const Coordinate& first, const Coordinate& second)
{
    return Coordinate{ .x = first.x + second.x, .y = first.y + second.y };
}

inline Coordinate operator+(const Coordinate& point, const Vector2d& vector)
{
    return Coordinate{
        .x = point.x + vector.x,
        .y = point.y + vector.y,
    };
}

inline Vector2d operator-(const Coordinate& first, const Coordinate& second)
{
    return Vector2d{ .x = first.x - second.x, .y = first.y - second.y };
}

inline Vector2d operator*(const Vector2d& vector, double scalar)
{
    return Vector2d{ .x = vector.x * scalar, .y = vector.y * scalar };
}

inline Vector2d operator/(const Vector2d& vector, double scalar)
{
    assert(scalar != 0.0);

    return Vector2d{ .x = vector.x / scalar, .y = vector.y / scalar };
}
