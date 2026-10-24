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

testing::Matcher<std::vector<geo::Line>>
AreLines(const std::vector<geo::Line>& expected, double maxError)
{
    std::vector<testing::Matcher<geo::Line>> elementMatchers;
    elementMatchers.reserve(expected.size());
    for (const geo::Line& line : expected) {
        elementMatchers.push_back(IsLine(line, maxError));
    }

    return testing::ElementsAreArray(elementMatchers);
}

}   // namespace fcad::test_utils
