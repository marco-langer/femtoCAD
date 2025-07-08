#include "CoordinateMatcher.hpp"

testing::Matcher<Coordinate> IsCoordinate(const Coordinate& coordinate, double maxError)
{
    return testing::AllOf(
        testing::Field("x", &Coordinate::x, testing::DoubleNear(coordinate.x, maxError)),
        testing::Field("y", &Coordinate::y, testing::DoubleNear(coordinate.y, maxError))
    );
}
