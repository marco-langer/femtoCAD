#pragma once

#include "domain/Lineweight.hpp"

#include <QStringList>

namespace fcad::io {

// Lineweights (group code 370) that aren't widths.
inline constexpr int byLayerLineweightValue{ -1 };
inline constexpr int byBlockLineweightValue{ -2 };
inline constexpr int defaultLineweightValue{ -3 };

// Maps a layer's DXF lineweight to a lineweight. Widths above the largest lineweight are clamped
// to it. Default (-3) becomes DefaultLineweight, and so do the other negative values, which aren't
// valid on a layer.
domain::LayerLineweight toLayerLineweight(int value);

// Whether a layer's DXF lineweight is valid. The invalid ones get an import warning.
bool isValidLayerLineweight(int value);

// Like toLayerLineweight(), for an entity. ByLayer (-1) becomes ByLayer, ByBlock (-2) becomes
// DefaultLineweight until blocks exist, and so do the other negative values, which aren't valid.
domain::EntityLineweight toEntityLineweight(int value);

// Whether an entity's DXF lineweight is valid. ByBlock is valid, though not supported yet.
bool isValidEntityLineweight(int value);

// The numbers of layers and entities whose lineweight was replaced, for the import warnings.
struct LineweightCounts final
{
    int invalidLayers{ 0 };
    int byBlockEntities{ 0 };
    int invalidEntities{ 0 };
};

// Import warnings about the replaced lineweights, one per kind.
QStringList lineweightWarnings(const LineweightCounts& counts);

}   // namespace fcad::io
