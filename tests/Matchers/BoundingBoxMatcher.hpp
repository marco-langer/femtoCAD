#pragma once

#include "core/BoundingBox.hpp"

#include <gmock/gmock.h>

testing::Matcher<BoundingBox>
IsBoundingBox(const BoundingBox& boundingBox, double maxError = 1.0e-5);
