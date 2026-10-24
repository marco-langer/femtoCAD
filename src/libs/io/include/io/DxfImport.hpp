#pragma once

#include "domain/Document.hpp"

#include <expected>

#include <QString>

namespace fcad::io {

std::expected<domain::Document, QString> importDxf(const QString& filePath);

}   // namespace fcad::io
