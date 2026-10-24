#pragma once

#include "core/Coordinate.hpp"

#include <gmock/gmock.h>

#include <optional>

namespace fcad::test_utils {

testing::Matcher<Coordinate> IsCoordinate(const Coordinate& coordinate, double maxError = 1.0e-5);

testing::Matcher<const std::optional<Coordinate>&>
IsCoordinate(const std::optional<Coordinate>& expected, double maxError = 1.0e-5);

}   // namespace fcad::test_utils
