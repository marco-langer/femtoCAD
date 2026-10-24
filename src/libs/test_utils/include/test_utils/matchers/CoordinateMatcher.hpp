#pragma once

#include "geo/Coordinate.hpp"

#include <gmock/gmock.h>

#include <optional>

namespace fcad::test_utils {

testing::Matcher<geo::Coordinate>
IsCoordinate(const geo::Coordinate& coordinate, double maxError = 1.0e-5);

testing::Matcher<const std::optional<geo::Coordinate>&>
IsCoordinate(const std::optional<geo::Coordinate>& expected, double maxError = 1.0e-5);

}   // namespace fcad::test_utils
