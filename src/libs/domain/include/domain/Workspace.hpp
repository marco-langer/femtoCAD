#pragma once

#include "domain/Document.hpp"

namespace fcad::domain {

class Workspace final
{
public:
    const Document& document() const&;
    // Move-assigns, so references to document() stay valid.
    void setDocument(Document document);

private:
    Document m_document;
};

}   // namespace fcad::domain
