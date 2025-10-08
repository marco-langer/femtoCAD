#include "CoordinateMatcher.hpp"

testing::Matcher<Coordinate> IsCoordinate(const Coordinate& coordinate, double maxError)
{
    return testing::AllOf(
        testing::Field("x", &Coordinate::x, testing::DoubleNear(coordinate.x, maxError)),
        testing::Field("y", &Coordinate::y, testing::DoubleNear(coordinate.y, maxError))
    );
}

testing::Matcher<const std::optional<Coordinate>&>
IsCoordinate(const std::optional<Coordinate>& expected, double maxError)
{
    if (expected.has_value()) {
        return testing::Optional(IsCoordinate(expected.value(), maxError));
    }

    return testing::Eq(std::nullopt);
}
