#pragma once

#include "domain/Workspace.hpp"

#include <tl/expected.hpp>

#include <QString>

namespace fcad::io {

tl::expected<void, QString> importDxf(domain::Workspace& workspace, const QString& filePath);

}   // namespace fcad::io
