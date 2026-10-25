#pragma once

#include "geo/Line.hpp"

#include <gmock/gmock.h>

namespace fcad::test_utils {

testing::Matcher<geo::Line> IsLine(const geo::Line& expected, double maxError = 1.0e-5);

}   // namespace fcad::test_utils
