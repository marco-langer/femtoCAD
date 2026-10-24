#pragma once

#include "geo/Line.hpp"

#include <gmock/gmock.h>

#include <vector>

namespace fcad::test_utils {

testing::Matcher<geo::Line> IsLine(const geo::Line& expected, double maxError = 1.0e-5);

testing::Matcher<std::vector<geo::Line>>
AreLines(const std::vector<geo::Line>& expected, double maxError = 1.0e-5);

}   // namespace fcad::test_utils
