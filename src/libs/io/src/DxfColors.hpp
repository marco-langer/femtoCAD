#pragma once

#include "domain/Color.hpp"

#include <QStringList>

#include <optional>

namespace fcad::io {

// Maps a DXF color index (group code 62) and true color (group code 420) to a color. The true
// color takes precedence; only its low 24 bits (0xRRGGBB) are used. Index 7 becomes Adaptive.
// Indexes outside 1-255 aren't colors and also become Adaptive.
// TODO: Invalid values are mapped silently. Report them as import warnings once that's needed.
domain::Color toColor(int index, std::optional<int> trueColor);

// Color indexes that aren't colors.
inline constexpr int byBlockIndex{ 0 };
inline constexpr int byLayerIndex{ 256 };
inline constexpr int byEntityIndex{ 257 };

// Like toColor(), for an entity. ByLayer (256) and ByEntity (257) become ByLayer, ByBlock (0)
// becomes Adaptive until blocks exist. Invalid indexes become ByLayer, like a missing one.
domain::EntityColor toEntityColor(int index, std::optional<int> trueColor);

// Import warnings about the entities whose ByBlock or ByEntity color was replaced, one per kind.
QStringList unsupportedColorWarnings(int byBlockCount, int byEntityCount);

}   // namespace fcad::io
