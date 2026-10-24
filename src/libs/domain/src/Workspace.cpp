#include "domain/Workspace.hpp"

namespace fcad::domain {

const std::optional<Document>& Workspace::document() const& { return m_document; }

void Workspace::setDocument(Document document) { m_document = std::move(document); }

}   // namespace fcad::domain
