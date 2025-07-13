#include "CircleMatcher.hpp"

#include "CoordinateMatcher.hpp"

testing::Matcher<Circle> IsCircle(const Circle& expected, double maxError)
{
    return testing::AllOf(
        testing::Field("center", &Circle::center, IsCoordinate(expected.center, maxError)),
        testing::Field("radius", &Circle::radius, testing::DoubleNear(expected.radius, maxError))
    );
}

testing::Matcher<Circles> AreCircles(const Circles& expected, double maxError)
{
    std::vector<testing::Matcher<Circle>> elementMatchers;
    elementMatchers.reserve(expected.size());
    for (const Circle& expectedCircle : expected) {
        elementMatchers.push_back(IsCircle(expectedCircle, maxError));
    }

    return testing::ElementsAreArray(std::move(elementMatchers));
}
