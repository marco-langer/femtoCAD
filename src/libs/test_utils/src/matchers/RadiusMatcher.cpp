#include "test_utils/matchers/RadiusMatcher.hpp"

namespace fcad::test_utils {

testing::Matcher<geo::Radius> IsRadius(geo::Radius expected, double maxError)
{
    return testing::Property(
        "value", &geo::Radius::value, testing::DoubleNear(expected.value(), maxError)
    );
}

}   // namespace fcad::test_utils
