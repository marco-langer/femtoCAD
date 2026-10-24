#include "io/DxfImportError.hpp"

#include <string>

namespace fcad::io {

namespace {

    class DxfImportCategory final : public std::error_category
    {
    public:
        const char* name() const noexcept override { return "dxf-import"; }

        std::string message(int value) const override
        {
            switch (static_cast<DxfImportError>(value)) {
            case DxfImportError::CannotOpenFile: return "cannot open the file";
            case DxfImportError::InvalidFile: return "invalid DXF file";
            }

            return "unknown DXF import error";
        }
    };

}   // namespace

const std::error_category& dxfImportCategory() noexcept
{
    static const DxfImportCategory category;

    return category;
}

std::error_code make_error_code(DxfImportError error) noexcept
{
    return { static_cast<int>(error), dxfImportCategory() };
}

}   // namespace fcad::io
