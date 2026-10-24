#pragma once

#include "geo/Circle.hpp"

#include <gmock/gmock.h>

#include <optional>

namespace fcad::test_utils {

testing::Matcher<geo::Circle> IsCircle(const geo::Circle& expected, double maxError = 1.0e-5);

testing::Matcher<const std::optional<geo::Circle>&>
IsCircle(const std::optional<geo::Circle>& expected, double maxError = 1.0e-5);

testing::Matcher<geo::Circles> AreCircles(const geo::Circles& expected, double maxError = 1.0e-5);

}   // namespace fcad::test_utils
