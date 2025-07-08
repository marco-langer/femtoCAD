#pragma once

#include "core/Layer.hpp"

#include <gmock/gmock.h>

testing::Matcher<Layer> IsLayer(const Layer& expected, double maxError = 1.0e-5);
