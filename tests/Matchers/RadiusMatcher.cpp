#include "RadiusMatcher.hpp"

testing::Matcher<Radius> IsRadius(Radius expected, double maxError)
{
    return testing::Property(
        "value", &Radius::value, testing::DoubleNear(expected.value(), maxError)
    );
}
