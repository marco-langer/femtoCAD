#pragma once

#include "core/Polyline.hpp"

#include <gmock/gmock.h>

testing::Matcher<Polyline> IsPolyline(const Polyline& expected, double maxError = 1.0e-5);
