#pragma once

#include "domain/Layer.hpp"

#include <gmock/gmock.h>

testing::Matcher<fcad::domain::Layer>
IsLayer(const fcad::domain::Layer& expected, double maxError = 1.0e-5);

testing::Matcher<fcad::domain::Layers>
AreLayers(const fcad::domain::Layers& expected, double maxError = 1.0e-5);
