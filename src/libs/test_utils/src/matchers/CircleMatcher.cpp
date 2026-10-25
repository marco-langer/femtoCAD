#include "test_utils/matchers/CircleMatcher.hpp"

#include "test_utils/matchers/CoordinateMatcher.hpp"
#include "test_utils/matchers/RadiusMatcher.hpp"

namespace fcad::test_utils {

testing::Matcher<geo::Circle> IsCircle(const geo::Circle& expected, double maxError)
{
    return testing::AllOf(
        testing::Field("center", &geo::Circle::center, IsCoordinate(expected.center, maxError)),
        testing::Field("radius", &geo::Circle::radius, IsRadius(expected.radius, maxError))
    );
}

testing::Matcher<const std::optional<geo::Circle>&>
IsCircle(const std::optional<geo::Circle>& expected, double maxError)
{
    if (expected.has_value()) {
        return testing::Optional(IsCircle(expected.value(), maxError));
    }

    return testing::Eq(std::nullopt);
}

}   // namespace fcad::test_utils
