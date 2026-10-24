#pragma once

#include "core/primitives/Radius.hpp"

#include <gmock/gmock.h>

namespace fcad::test_utils {

testing::Matcher<Radius> IsRadius(Radius expected, double maxError = 1.0e-5);

}   // namespace fcad::test_utils
