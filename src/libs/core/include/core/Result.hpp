#pragma once

#include "core/Error.hpp"

#include <expected>

namespace fcad::core {

template <typename T>
using Result = std::expected<T, Error>;

}   // namespace fcad::core
