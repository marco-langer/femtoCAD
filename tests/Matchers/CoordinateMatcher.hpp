#pragma once

#include "core/Coordinate.hpp"

#include <gmock/gmock.h>

testing::Matcher<Coordinate> IsCoordinate(const Coordinate& coordinate, double maxError = 1.0e-5);
