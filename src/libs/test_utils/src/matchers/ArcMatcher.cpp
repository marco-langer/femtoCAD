#include "test_utils/matchers/ArcMatcher.hpp"

#include "test_utils/matchers/CoordinateMatcher.hpp"
#include "test_utils/matchers/RadiusMatcher.hpp"

#include <vector>

namespace fcad::test_utils {

testing::Matcher<Arc> IsArc(const Arc& expected, double maxError)
{
    return testing::AllOf(
        testing::Field("center", &Arc::center, IsCoordinate(expected.center, maxError)),
        testing::Field("startAngle", &Arc::startAngle, expected.startAngle),
        testing::Field("endAngle", &Arc::endAngle, expected.endAngle),
        testing::Field("radius", &Arc::radius, IsRadius(expected.radius, maxError))
    );
}

testing::Matcher<Arcs> AreArcs(const Arcs& expected, double maxError)
{
    std::vector<testing::Matcher<Arc>> elementMatchers;
    elementMatchers.reserve(expected.size());
    for (const Arc& expectedArc : expected) {
        elementMatchers.push_back(IsArc(expectedArc, maxError));
    }

    return testing::ElementsAreArray(std::move(elementMatchers));
}

}   // namespace fcad::test_utils
