#include "core/Layer.hpp"

#include <algorithm>
#include <ranges>

namespace {
std::optional<BoundingBox> boundingBox(const std::vector<Line>& lines)
{
    if (lines.empty()) {
        return std::nullopt;
    }

    BoundingBox bb{ boundingBox(lines.front()) };

    for (const Line& line : lines | std::views::drop(1)) {
        bb = united(bb, boundingBox(line));
    }

    return bb;
}
}   // namespace

std::optional<BoundingBox> boundingBox(const Layer& layer) { return boundingBox(layer.lines); }
