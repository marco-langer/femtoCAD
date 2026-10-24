#include "test_utils/matchers/BoundingBoxMatcher.hpp"

#include "test_utils/matchers/CoordinateMatcher.hpp"

namespace fcad::test_utils {

testing::Matcher<BoundingBox> IsBoundingBox(const BoundingBox& boundingBox, double maxError)
{
    return testing::AllOf(
        testing::Field("min", &BoundingBox::min, IsCoordinate(boundingBox.min, maxError)),
        testing::Field("max", &BoundingBox::max, IsCoordinate(boundingBox.max, maxError))
    );
}

}   // namespace fcad::test_utils
