#pragma once

#include "domain/Document.hpp"

#include <tl/expected.hpp>

#include <QString>

namespace fcad::io {

tl::expected<domain::Document, QString> importDxf(const QString& filePath);

}   // namespace fcad::io
