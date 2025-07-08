#include "LineMatcher.hpp"

#include "CoordinateMatcher.hpp"

testing::Matcher<Line> IsLine(const Line& expected, double maxError)
{
    return testing::AllOf(
        testing::Field("start", &Line::start, IsCoordinate(expected.start, maxError)),
        testing::Field("end", &Line::end, IsCoordinate(expected.end, maxError))
    );
}

testing::Matcher<std::vector<Line>> AreLines(const std::vector<Line>& expected, double maxError)
{
    std::vector<testing::Matcher<Line>> elementMatchers;
    elementMatchers.reserve(expected.size());
    for (const Line& line : expected) {
        elementMatchers.push_back(IsLine(line, maxError));
    }

    return testing::ElementsAreArray(elementMatchers);
}
