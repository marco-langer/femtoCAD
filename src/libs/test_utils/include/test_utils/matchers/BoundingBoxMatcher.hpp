#pragma once

#include "core/BoundingBox.hpp"

#include <gmock/gmock.h>

namespace fcad::test_utils {

testing::Matcher<BoundingBox>
IsBoundingBox(const BoundingBox& boundingBox, double maxError = 1.0e-5);

}   // namespace fcad::test_utils
