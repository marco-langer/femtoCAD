#pragma once

#include "domain/Document.hpp"

#include <optional>

namespace fcad::domain {

class Workspace final
{
public:
    // Empty until a document is created or imported.
    const std::optional<Document>& document() const&;
    // Assigns in place, so references to document() stay valid.
    void setDocument(Document document);

private:
    std::optional<Document> m_document;
};

}   // namespace fcad::domain
