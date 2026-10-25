#include "DxfLinetypes.hpp"

#include <QString>
#include <QStringView>

#include <algorithm>
#include <cmath>

namespace fcad::io {

namespace {

    // Plural forms of the warnings, which share their structure.
    QString countWarning(int count, QStringView singular, QStringView plural)
    {
        return count == 1 ? singular.toString() : plural.arg(QString::number(count));
    }

}   // namespace

std::optional<domain::DashPattern>
toDashPattern(const odxf::LineTypeElements& elements, double scale)
{
    domain::PatternElements patternElements;
    patternElements.reserve(elements.size());

    for (const odxf::LineTypeElement& element : elements) {
        if (element.length == 0.0) {
            patternElements.push_back(domain::Dot{});
            continue;
        }

        const std::optional<domain::PatternLength> length{
            domain::PatternLength::fromValue(std::abs(element.length) * scale)
        };
        if (!length.has_value()) {
            return std::nullopt;
        }

        if (element.length > 0.0) {
            patternElements.push_back(domain::Dash{ length.value() });
        } else {
            patternElements.push_back(domain::Gap{ length.value() });
        }
    }

    return domain::DashPattern::fromElements(std::move(patternElements));
}

bool isComplex(const odxf::LineType& dxfLinetype)
{
    return std::ranges::any_of(dxfLinetype.elements, [](const odxf::LineTypeElement& element) {
        return element.complexFlags != 0;
    });
}

QStringList linetypeWarnings(const LinetypeCounts& counts)
{
    QStringList warnings;

    if (counts.complexLinetypes > 0) {
        warnings.push_back(countWarning(
            counts.complexLinetypes,
            u"1 linetype contains texts or shapes, which femtoCAD doesn't support. Lines using it "
            u"are drawn continuous.",
            u"%1 linetypes contain texts or shapes, which femtoCAD doesn't support. Lines using "
            u"them are drawn continuous."
        ));
    }

    if (counts.invalidLinetypes > 0) {
        warnings.push_back(countWarning(
            counts.invalidLinetypes,
            u"1 linetype has an invalid or duplicate name or pattern and was skipped. Lines using "
            u"it are drawn continuous.",
            u"%1 linetypes have an invalid or duplicate name or pattern and were skipped. Lines "
            u"using them are drawn continuous."
        ));
    }

    if (counts.unknownLayerLinetypes > 0) {
        warnings.push_back(countWarning(
            counts.unknownLayerLinetypes,
            u"1 layer uses an unknown linetype. It is drawn continuous.",
            u"%1 layers use an unknown linetype. They are drawn continuous."
        ));
    }

    return warnings;
}

AddedLinetype DxfLinetypes::add(const odxf::LineType& dxfLinetype, double scale)
{
    const QString name{ QString::fromStdString(dxfLinetype.name) };
    if (name.compare(u"BYLAYER", Qt::CaseInsensitive) == 0
        || name.compare(u"BYBLOCK", Qt::CaseInsensitive) == 0) {
        return AddedLinetype::BuiltIn;
    }

    std::optional<domain::LinetypeName> linetypeName{ domain::LinetypeName::fromValue(name) };
    if (!linetypeName.has_value()) {
        return AddedLinetype::Invalid;
    }

    if (linetypeName.value() == domain::Linetype::continuous().name()) {
        return AddedLinetype::BuiltIn;
    }

    if (find(linetypeName.value()).has_value()) {
        return AddedLinetype::Invalid;
    }

    if (isComplex(dxfLinetype)) {
        m_ids.emplace_back(std::move(linetypeName.value()), domain::continuousLinetypeId());
        return AddedLinetype::Complex;
    }

    std::optional<domain::DashPattern> pattern{ toDashPattern(dxfLinetype.elements, scale) };
    if (!pattern.has_value()) {
        return AddedLinetype::Invalid;
    }

    const std::optional<domain::LinetypeId> id{ m_linetypes.tryAdd(
        domain::LinetypeDefinition{ .name = linetypeName.value(),
                                    .pattern = std::move(pattern.value()) }
    ) };
    if (!id.has_value()) {
        return AddedLinetype::Invalid;
    }
    m_ids.emplace_back(std::move(linetypeName.value()), id.value());

    return AddedLinetype::Added;
}

std::optional<domain::LinetypeId> DxfLinetypes::find(const std::string& name) const
{
    const std::optional<domain::LinetypeName> linetypeName{
        domain::LinetypeName::fromValue(QString::fromStdString(name))
    };
    if (!linetypeName.has_value()) {
        return std::nullopt;
    }

    return find(linetypeName.value());
}

std::optional<domain::LinetypeId> DxfLinetypes::find(const domain::LinetypeName& name) const
{
    const auto it{
        std::ranges::find(m_ids, name, &std::pair<domain::LinetypeName, domain::LinetypeId>::first)
    };
    if (it == m_ids.end()) {
        return std::nullopt;
    }

    return it->second;
}

domain::Linetypes DxfLinetypes::takeLinetypes() && { return std::move(m_linetypes); }

}   // namespace fcad::io
