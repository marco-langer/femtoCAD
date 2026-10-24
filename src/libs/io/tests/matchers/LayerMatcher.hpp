#pragma once

#include "core/Layer.hpp"

#include <gmock/gmock.h>

testing::Matcher<Layer> IsLayer(const Layer& expected, double maxError = 1.0e-5);

testing::Matcher<Layers> AreLayers(const Layers& expected, double maxError = 1.0e-5);
