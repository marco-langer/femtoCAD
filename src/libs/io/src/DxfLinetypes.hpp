#pragma once

#include "domain/Linetype.hpp"
#include "domain/Linetypes.hpp"

#include <opendxf/tables.hpp>

#include <QStringList>

#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace fcad::io {

/**
 * Maps a DXF dash pattern (group codes 49) to a pattern with every length multiplied by scale.
 * Positive lengths become dashes, negative ones gaps and 0 dots. Empty if a length isn't finite or
 * the elements aren't a valid DashPattern, e.g. only gaps.
 */
std::optional<domain::DashPattern>
toDashPattern(const odxf::LineTypeElements& elements, double scale);

/** Whether the linetype embeds texts or shapes, which femtoCAD doesn't support. */
bool isComplex(const odxf::LineType& dxfLinetype);

/** The numbers of linetypes and references that were replaced, for the import warnings. */
struct LinetypeCounts final
{
    int complexLinetypes{ 0 };
    int invalidLinetypes{ 0 };
    int unknownLayerLinetypes{ 0 };
};

/** Import warnings about the replaced linetypes and references, one per kind. */
QStringList linetypeWarnings(const LinetypeCounts& counts);

/** What became of an entry of the LTYPE table. */
enum class AddedLinetype {
    Added,
    /** CONTINUOUS, or a BYLAYER or BYBLOCK entry, which some files contain. */
    BuiltIn,
    /** Embeds texts or shapes; references to it get Continuous. */
    Complex,
    /** An invalid name or pattern, or the name of an earlier linetype. */
    Invalid,
};

/** The file's linetypes as resources, and the ids of their names, for one import. */
class DxfLinetypes final
{
public:
    /** Adds a linetype of the LTYPE table, with its lengths multiplied by scale. */
    AddedLinetype add(const odxf::LineType& dxfLinetype, double scale);

    /** The id for a name a layer or an entity refers to. Empty if no linetype has that name. */
    std::optional<domain::LinetypeId> find(const std::string& name) const;

    /** The linetypes for the document, with Continuous. */
    domain::Linetypes takeLinetypes() &&;

private:
    std::optional<domain::LinetypeId> find(const domain::LinetypeName& name) const;

    std::vector<std::pair<domain::LinetypeName, domain::LinetypeId>> m_ids{
        { domain::Linetype::continuous().name(), domain::continuousLinetypeId() },
    };
    domain::Linetypes m_linetypes;
};

}   // namespace fcad::io
