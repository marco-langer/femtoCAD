#include "domain/Linetypes.hpp"

#include <gsl/assert>

#include <algorithm>
#include <utility>

namespace fcad::domain {

// TODO: Expects() terminates on a violated precondition. How programming errors are checked and
// reported is still an open decision of ADR 0003.

Linetypes::Linetypes() { m_linetypes.push_back(Linetype::continuous()); }

std::optional<LinetypeId> Linetypes::tryAdd(LinetypeDefinition definition)
{
    if (contains(definition.name)) {
        return std::nullopt;
    }

    return m_linetypes.emplace_back(std::move(definition)).id();
}

bool Linetypes::contains(LinetypeId id) const
{
    return std::ranges::contains(m_linetypes, id, &Linetype::id);
}

const Linetype& Linetypes::at(LinetypeId id) const
{
    const auto it{ std::ranges::find(m_linetypes, id, &Linetype::id) };
    Expects(it != m_linetypes.end());

    return *it;
}

Linetypes::const_iterator Linetypes::begin() const { return m_linetypes.begin(); }

Linetypes::const_iterator Linetypes::end() const { return m_linetypes.end(); }

std::size_t Linetypes::size() const { return m_linetypes.size(); }

bool Linetypes::contains(const LinetypeName& name) const
{
    return std::ranges::contains(m_linetypes, name, &Linetype::name);
}

}   // namespace fcad::domain
