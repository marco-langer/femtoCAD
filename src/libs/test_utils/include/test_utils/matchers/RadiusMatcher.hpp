#pragma once

#include "geo/Radius.hpp"

#include <gmock/gmock.h>

namespace fcad::test_utils {

testing::Matcher<geo::Radius> IsRadius(geo::Radius expected, double maxError = 1.0e-5);

}   // namespace fcad::test_utils
