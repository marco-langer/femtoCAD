#include "core/Coordinate.hpp"

#include <cmath>

double norm(const Vector2d& vector) { return std::sqrt(vector.x * vector.x + vector.y * vector.y); }
