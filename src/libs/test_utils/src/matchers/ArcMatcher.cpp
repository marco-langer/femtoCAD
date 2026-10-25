#include "test_utils/matchers/ArcMatcher.hpp"

#include "test_utils/matchers/CoordinateMatcher.hpp"
#include "test_utils/matchers/RadiusMatcher.hpp"

namespace fcad::test_utils {

testing::Matcher<geo::Arc> IsArc(const geo::Arc& expected, double maxError)
{
    return testing::AllOf(
        testing::Field("center", &geo::Arc::center, IsCoordinate(expected.center, maxError)),
        testing::Field("startAngle", &geo::Arc::startAngle, expected.startAngle),
        testing::Field("endAngle", &geo::Arc::endAngle, expected.endAngle),
        testing::Field("radius", &geo::Arc::radius, IsRadius(expected.radius, maxError))
    );
}

}   // namespace fcad::test_utils
