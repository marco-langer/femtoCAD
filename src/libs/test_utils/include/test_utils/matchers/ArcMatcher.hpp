#pragma once

#include "geo/Arc.hpp"

#include <gmock/gmock.h>

namespace fcad::test_utils {

testing::Matcher<geo::Arc> IsArc(const geo::Arc& expected, double maxError = 1.0e-5);

}   // namespace fcad::test_utils
