#include "BoundingBoxMatcher.hpp"

#include "CoordinateMatcher.hpp"

testing::Matcher<BoundingBox> IsBoundingBox(const BoundingBox& boundingBox, double maxError)
{
    return testing::AllOf(
        testing::Field("min", &BoundingBox::min, IsCoordinate(boundingBox.min, maxError)),
        testing::Field("max", &BoundingBox::max, IsCoordinate(boundingBox.max, maxError))
    );
}
