#pragma once

#include "domain/Lineweight.hpp"

#include <QStringList>

namespace fcad::io {

// The lineweight (group code 370) that stands for the application's default.
inline constexpr int defaultLineweightValue{ -3 };

// Maps a layer's DXF lineweight to a lineweight. Widths above the largest lineweight are clamped
// to it. Default (-3) becomes DefaultLineweight, and so do the other negative values, which aren't
// valid on a layer.
domain::LayerLineweight toLayerLineweight(int value);

// Whether a layer's DXF lineweight is valid. The invalid ones get an import warning.
bool isValidLayerLineweight(int value);

// The numbers of layers and entities whose lineweight was replaced, for the import warnings.
struct LineweightCounts final
{
    int invalidLayers{ 0 };
};

// Import warnings about the replaced lineweights, one per kind.
QStringList lineweightWarnings(const LineweightCounts& counts);

}   // namespace fcad::io
