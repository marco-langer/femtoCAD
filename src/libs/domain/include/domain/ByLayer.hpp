#pragma once

namespace fcad::domain {

// The entity's layer's value, e.g. its color, lineweight or linetype.
struct ByLayer final
{
    bool operator==(const ByLayer&) const = default;
};

}   // namespace fcad::domain
