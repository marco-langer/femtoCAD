#include "domain/Linetype.hpp"

#include "core/Overload.hpp"

#include <QStringView>
#include <QUuid>

#include <algorithm>
#include <utility>
#include <variant>

namespace fcad::domain {

namespace {

    // Characters DXF doesn't allow in table names.
    constexpr QStringView forbiddenCharacters{ u"<>/\\\":;?*|,=`" };

}   // namespace

LinetypeId continuousLinetypeId()
{
    return LinetypeId{ QUuid{ "{08934405-ac18-4672-b728-95a00029920e}" } };
}

std::optional<LinetypeName> LinetypeName::fromValue(QString name)
{
    if (name.isEmpty() || name.size() > maximumLength || name.trimmed() != name) {
        return std::nullopt;
    }

    if (name.compare(u"ByLayer", Qt::CaseInsensitive) == 0
        || name.compare(u"ByBlock", Qt::CaseInsensitive) == 0) {
        return std::nullopt;
    }

    for (const QChar character : forbiddenCharacters) {
        if (name.contains(character)) {
            return std::nullopt;
        }
    }

    return LinetypeName{ std::move(name) };
}

const QString& LinetypeName::value() const { return m_value; }

bool LinetypeName::operator==(const LinetypeName& other) const
{
    return m_value.compare(other.m_value, Qt::CaseInsensitive) == 0;
}

LinetypeName::LinetypeName(QString name)
    : m_value{ std::move(name) }
{}

DashPattern DashPattern::continuous() { return DashPattern{ PatternElements{} }; }

std::optional<DashPattern> DashPattern::fromElements(PatternElements elements)
{
    if (elements.empty()) {
        return continuous();
    }

    const bool isVisible{ std::ranges::any_of(elements, [](const PatternElement& element) {
        return !std::holds_alternative<Gap>(element);
    }) };

    double length{ 0.0 };
    for (const PatternElement& element : elements) {
        length += std::visit(
            core::Overload{
                [](const Dash& dash) { return dash.length.value(); },
                [](const Gap& gap) { return gap.length.value(); },
                [](const Dot& /* dot */) { return 0.0; },
            },
            element
        );
    }

    if (!isVisible || length <= 0.0) {
        return std::nullopt;
    }

    return DashPattern{ std::move(elements) };
}

bool DashPattern::isContinuous() const { return m_elements.empty(); }

const PatternElements& DashPattern::elements() const { return m_elements; }

DashPattern::DashPattern(PatternElements elements)
    : m_elements{ std::move(elements) }
{}

Linetype::Linetype(LinetypeDefinition definition)
    : Linetype{ LinetypeId::generate(), std::move(definition) }
{}

Linetype Linetype::continuous()
{
    return Linetype{
        continuousLinetypeId(),
        LinetypeDefinition{
            .name = LinetypeName::fromValue(QStringLiteral(u"Continuous")).value(),
            .pattern = DashPattern::continuous(),
        },
    };
}

LinetypeId Linetype::id() const { return m_id; }

const LinetypeName& Linetype::name() const { return m_definition.name; }

const DashPattern& Linetype::pattern() const { return m_definition.pattern; }

LinetypeId resolved(const EntityLinetype& entityLinetype, LinetypeId layerLinetype)
{
    return std::visit(
        core::Overload{
            [layerLinetype](const ByLayer& /* byLayer */) { return layerLinetype; },
            [](LinetypeId own) { return own; },
        },
        entityLinetype
    );
}

Linetype::Linetype(LinetypeId id, LinetypeDefinition definition)
    : m_id{ id }
    , m_definition{ std::move(definition) }
{}

}   // namespace fcad::domain
