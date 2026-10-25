#include "test_utils/matchers/LineMatcher.hpp"

#include "test_utils/matchers/CoordinateMatcher.hpp"

namespace fcad::test_utils {

testing::Matcher<geo::Line> IsLine(const geo::Line& expected, double maxError)
{
    return testing::AllOf(
        testing::Field("start", &geo::Line::start, IsCoordinate(expected.start, maxError)),
        testing::Field("end", &geo::Line::end, IsCoordinate(expected.end, maxError))
    );
}

}   // namespace fcad::test_utils
