#pragma once

#include <system_error>
#include <type_traits>

namespace fcad::io {

// Starts at 1: an error_code with value 0 means success.
enum class DxfImportError { CannotOpenFile = 1, InvalidFile };

const std::error_category& dxfImportCategory() noexcept;

std::error_code make_error_code(DxfImportError error) noexcept;

}   // namespace fcad::io

template <>
struct std::is_error_code_enum<fcad::io::DxfImportError> : std::true_type
{
};
