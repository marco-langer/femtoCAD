#pragma once

#include "core/Line.hpp"

#include <gmock/gmock.h>

#include <vector>

testing::Matcher<const Line&> IsLine(const Line& expected, double maxError = 1.0e-5);

testing::Matcher<std::vector<Line>>
AreLines(const std::vector<Line>& expected, double maxError = 1.0e-5);
