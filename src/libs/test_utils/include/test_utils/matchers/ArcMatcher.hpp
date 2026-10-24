#pragma once

#include "core/Arc.hpp"

#include <gmock/gmock.h>

namespace fcad::test_utils {

testing::Matcher<Arc> IsArc(const Arc& expected, double maxError = 1.0e-5);

testing::Matcher<Arcs> AreArcs(const Arcs& expected, double maxError = 1.0e-5);

}   // namespace fcad::test_utils
