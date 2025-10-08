#pragma once

#include "core/Circle.hpp"

#include <gmock/gmock.h>

#include <optional>

testing::Matcher<Circle> IsCircle(const Circle& expected, double maxError = 1.0e-5);

testing::Matcher<const std::optional<Circle>&>
IsCircle(const std::optional<Circle>& expected, double maxError = 1.0e-5);

testing::Matcher<Circles> AreCircles(const Circles& expected, double maxError = 1.0e-5);
