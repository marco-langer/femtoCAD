#pragma once

#include "geo/BoundingBox.hpp"

#include <gmock/gmock.h>

namespace fcad::test_utils {

testing::Matcher<geo::BoundingBox>
IsBoundingBox(const geo::BoundingBox& boundingBox, double maxError = 1.0e-5);

}   // namespace fcad::test_utils
