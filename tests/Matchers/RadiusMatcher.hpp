#pragma once

#include "core/primitives/Radius.hpp"

#include <gmock/gmock.h>

testing::Matcher<Radius> IsRadius(Radius expected, double maxError = 1.0e-5);
