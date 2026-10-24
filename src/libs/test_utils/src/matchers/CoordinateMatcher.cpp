#include "test_utils/matchers/CoordinateMatcher.hpp"

namespace fcad::test_utils {

testing::Matcher<geo::Coordinate> IsCoordinate(const geo::Coordinate& coordinate, double maxError)
{
    return testing::AllOf(
        testing::Field("x", &geo::Coordinate::x, testing::DoubleNear(coordinate.x, maxError)),
        testing::Field("y", &geo::Coordinate::y, testing::DoubleNear(coordinate.y, maxError))
    );
}

testing::Matcher<const std::optional<geo::Coordinate>&>
IsCoordinate(const std::optional<geo::Coordinate>& expected, double maxError)
{
    if (expected.has_value()) {
        return testing::Optional(IsCoordinate(expected.value(), maxError));
    }

    return testing::Eq(std::nullopt);
}

}   // namespace fcad::test_utils
