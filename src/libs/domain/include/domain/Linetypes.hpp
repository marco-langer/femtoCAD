#pragma once

#include "domain/Linetype.hpp"

#include <cstddef>
#include <optional>
#include <vector>

namespace fcad::domain {

/**
 * The linetypes of a document. Always contains the built-in linetype Continuous, first, and no two
 * linetypes with the same id or name. It only gives read access to its linetypes, so that these
 * invariants can't be broken from outside.
 */
class Linetypes final
{
public:
    using const_iterator = std::vector<Linetype>::const_iterator;

    /** Only Continuous. */
    Linetypes();

    /** Adds a linetype with a new id and returns the id. Empty if the name is already taken. */
    [[nodiscard]] std::optional<LinetypeId> tryAdd(LinetypeDefinition definition);

    bool contains(LinetypeId id) const;

    /** @pre The linetype is in the collection. */
    const Linetype& at(LinetypeId id) const;

    const_iterator begin() const;
    const_iterator end() const;
    std::size_t size() const;

private:
    // Compares names without regard to case, see LinetypeName.
    bool contains(const LinetypeName& name) const;

    std::vector<Linetype> m_linetypes;
};

}   // namespace fcad::domain
