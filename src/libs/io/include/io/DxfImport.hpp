#pragma once

#include "core/Workspace.hpp"

#include <tl/expected.hpp>

#include <QString>

namespace fcad::io {

tl::expected<void, QString> importDxf(Workspace& workspace, const QString& filePath);

}   // namespace fcad::io
