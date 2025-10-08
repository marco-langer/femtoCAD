#include "CircleMatcher.hpp"

#include "CoordinateMatcher.hpp"
#include "RadiusMatcher.hpp"

testing::Matcher<Circle> IsCircle(const Circle& expected, double maxError)
{
    return testing::AllOf(
        testing::Field("center", &Circle::center, IsCoordinate(expected.center, maxError)),
        testing::Field("radius", &Circle::radius, IsRadius(expected.radius, maxError))
    );
}

testing::Matcher<const std::optional<Circle>&>
IsCircle(const std::optional<Circle>& expected, double maxError)
{
    if (expected.has_value()) {
        return testing::Optional(IsCircle(expected.value(), maxError));
    }

    return testing::Eq(std::nullopt);
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
