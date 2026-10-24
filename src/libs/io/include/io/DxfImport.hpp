#pragma once

#include "core/Result.hpp"
#include "domain/Document.hpp"

#include <QString>

namespace fcad::io {

core::Result<domain::Document> importDxf(const QString& filePath);

}   // namespace fcad::io
