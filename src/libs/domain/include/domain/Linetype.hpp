#pragma once

#include "core/Positive.hpp"
#include "core/StrongUuid.hpp"
#include "domain/ByLayer.hpp"

#include <QString>

#include <optional>
#include <variant>
#include <vector>

namespace fcad::domain {

using LinetypeId = core::StrongUuid<struct LinetypeIdTag>;

/** The id of the built-in linetype Continuous, the same in every document (ADR 0005). */
LinetypeId continuousLinetypeId();

/**
 * A linetype's name. It isn't empty, has no surrounding spaces, isn't ByLayer or ByBlock in any
 * case, and has none of the characters DXF forbids in names, so every name can be exported.
 */
class LinetypeName final
{
public:
    static constexpr qsizetype maximumLength{ 255 };

    [[nodiscard]] static std::optional<LinetypeName> fromValue(QString name);

    /** As given, for display. */
    const QString& value() const;

    /** Without regard to case, as in DXF. */
    bool operator==(const LinetypeName& other) const;

private:
    explicit LinetypeName(QString name);

    QString m_value;
};

/** The length of a dash or a gap, in the document's unit. */
using PatternLength = core::Positive<double, struct PatternLengthTag>;

struct Dash final
{
    PatternLength length;

    bool operator==(const Dash&) const = default;
};

struct Gap final
{
    PatternLength length;

    bool operator==(const Gap&) const = default;
};

struct Dot final
{
    bool operator==(const Dot&) const = default;
};

using PatternElement = std::variant<Dash, Gap, Dot>;

using PatternElements = std::vector<PatternElement>;

/**
 * A linetype's dash pattern, repeated along a line from its start. It's either continuous, without
 * elements, or it has at least one dash or dot and a length greater than 0, so that a line with it
 * is visible and the pattern can repeat.
 */
class DashPattern final
{
public:
    [[nodiscard]] static DashPattern continuous();

    /** Empty if the elements are only gaps, or only dots. No elements give continuous(). */
    [[nodiscard]] static std::optional<DashPattern> fromElements(PatternElements elements);

    bool isContinuous() const;
    const PatternElements& elements() const;

private:
    explicit DashPattern(PatternElements elements);

    PatternElements m_elements;
};

/**
 * A linetype independent of any document: its name and its dash pattern (ADR 0005). Unlike a
 * Linetype it has no id, so copying it is harmless.
 */
struct LinetypeDefinition final
{
    LinetypeName name;
    DashPattern pattern;
};

/**
 * A linetype definition with an id, the resource a document owns. Move-only like the entities, so
 * that no two linetypes share an id.
 */
class Linetype final
{
public:
    /** Gives the linetype a new id. */
    explicit Linetype(LinetypeDefinition definition);
    Linetype(const Linetype&) = delete;
    Linetype(Linetype&&) = default;
    ~Linetype() = default;

    Linetype& operator=(const Linetype&) = delete;
    Linetype& operator=(Linetype&&) = default;

    /** The built-in linetype Continuous, with continuousLinetypeId(). */
    [[nodiscard]] static Linetype continuous();

    LinetypeId id() const;
    const LinetypeName& name() const;
    const DashPattern& pattern() const;

private:
    Linetype(LinetypeId id, LinetypeDefinition definition);

    LinetypeId m_id;
    LinetypeDefinition m_definition;
};

/** Scales all dash patterns of a document, or those of one entity. */
using LinetypeScale = core::Positive<double, struct LinetypeScaleTag>;

using EntityLinetype = std::variant<ByLayer, LinetypeId>;

/** The linetype an entity is drawn with: its own, or for ByLayer its layer's. */
LinetypeId resolved(const EntityLinetype& entityLinetype, LinetypeId layerLinetype);

}   // namespace fcad::domain
