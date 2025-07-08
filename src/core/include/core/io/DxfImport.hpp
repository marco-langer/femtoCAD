#pragma once

#include "core/Workspace.hpp"

#include <tl/expected.hpp>

#include <QString>

tl::expected<void, QString> importDxf(Workspace& workspace, const QString& filePath);
