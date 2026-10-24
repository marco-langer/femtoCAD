#include "test_utils/matchers/RadiusMatcher.hpp"

namespace fcad::test_utils {

testing::Matcher<Radius> IsRadius(Radius expected, double maxError)
{
    return testing::Property(
        "value", &Radius::value, testing::DoubleNear(expected.value(), maxError)
    );
}

}   // namespace fcad::test_utils
