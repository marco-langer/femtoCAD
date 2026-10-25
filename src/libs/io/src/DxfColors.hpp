#pragma once

#include "domain/Color.hpp"

#include <optional>

namespace fcad::io {

// Maps a DXF color index (group code 62) and true color (group code 420) to a color. The true
// color takes precedence; only its low 24 bits (0xRRGGBB) are used. Index 7 becomes Adaptive.
// Indexes outside 1-255 aren't colors and also become Adaptive.
// TODO: Invalid values are mapped silently. Report them as import warnings once that's needed.
domain::Color toColor(int index, std::optional<int> trueColor);

}   // namespace fcad::io
