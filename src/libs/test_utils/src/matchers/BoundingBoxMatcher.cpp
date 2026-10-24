#include "test_utils/matchers/BoundingBoxMatcher.hpp"

#include "test_utils/matchers/CoordinateMatcher.hpp"

namespace fcad::test_utils {

testing::Matcher<geo::BoundingBox>
IsBoundingBox(const geo::BoundingBox& boundingBox, double maxError)
{
    return testing::AllOf(
        testing::Field("min", &geo::BoundingBox::min, IsCoordinate(boundingBox.min, maxError)),
        testing::Field("max", &geo::BoundingBox::max, IsCoordinate(boundingBox.max, maxError))
    );
}

}   // namespace fcad::test_utils
