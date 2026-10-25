#include "Settings.hpp"

#include "domain/LengthUnit.hpp"
#include "domain/Lineweight.hpp"

#include <QSettings>

#include <optional>

namespace fcad::app {

namespace {

    QString toKey(domain::LengthUnit unit)
    {
        switch (unit) {
        case domain::LengthUnit::Micrometer: return QStringLiteral(u"micrometer");
        case domain::LengthUnit::Millimeter: return QStringLiteral(u"millimeter");
        case domain::LengthUnit::Centimeter: return QStringLiteral(u"centimeter");
        case domain::LengthUnit::Decimeter: return QStringLiteral(u"decimeter");
        case domain::LengthUnit::Meter: return QStringLiteral(u"meter");
        case domain::LengthUnit::Kilometer: return QStringLiteral(u"kilometer");
        case domain::LengthUnit::Inch: return QStringLiteral(u"inch");
        case domain::LengthUnit::Foot: return QStringLiteral(u"foot");
        case domain::LengthUnit::Yard: return QStringLiteral(u"yard");
        case domain::LengthUnit::Mile: return QStringLiteral(u"mile");
        }

        return QString{};
    }

    std::optional<domain::LengthUnit> fromKey(const QString& key)
    {
        for (const domain::LengthUnit unit : domain::lengthUnits) {
            if (toKey(unit) == key) {
                return unit;
            }
        }

        return std::nullopt;
    }

    domain::LengthUnit
    loadUnit(const QSettings& qsettings, const QString& key, domain::LengthUnit defaultUnit)
    {
        return fromKey(qsettings.value(key).toString()).value_or(defaultUnit);
    }

    domain::Lineweight
    loadLineweight(const QSettings& qsettings, const QString& key, domain::Lineweight fallback)
    {
        bool isInt{ false };
        const int value{ qsettings.value(key).toInt(&isInt) };
        if (!isInt) {
            return fallback;
        }

        return domain::Lineweight::fromValue(value).value_or(fallback);
    }

}   // namespace

Settings loadSettings()
{
    Settings settings;

    QSettings qsettings{ "femtoCAD", "femtoCAD" };

    settings.defaultUnit = loadUnit(qsettings, "defaultUnit", settings.defaultUnit);
    settings.defaultLineweightWidth =
        loadLineweight(qsettings, "defaultLineweightWidth", settings.defaultLineweightWidth);

    qsettings.beginGroup("dxfImport");

    QVariant lastImportPath{ qsettings.value("lastImportPath") };
    if (lastImportPath.canConvert<QString>()) {
        settings.dxfImport.lastImportPath = lastImportPath.toString();
    }

    settings.dxfImport.fallbackUnit =
        loadUnit(qsettings, "fallbackUnit", settings.dxfImport.fallbackUnit);
    settings.dxfImport.useFallbackUnit =
        qsettings.value("useFallbackUnit", settings.dxfImport.useFallbackUnit).toBool();

    qsettings.endGroup();

    qsettings.beginGroup("view");

    settings.view.showAxes = qsettings.value("showAxes", settings.view.showAxes).toBool();
    settings.view.showGrid = qsettings.value("showGrid", settings.view.showGrid).toBool();
    settings.view.showLineweights =
        qsettings.value("showLineweights", settings.view.showLineweights).toBool();

    qsettings.endGroup();

    return settings;
}

void saveSettings(const Settings& settings)
{
    QSettings qsettings{ "femtoCAD", "femtoCAD" };

    qsettings.setValue("defaultUnit", toKey(settings.defaultUnit));
    qsettings.setValue("defaultLineweightWidth", settings.defaultLineweightWidth.value());

    qsettings.beginGroup("dxfImport");

    if (settings.dxfImport.lastImportPath.has_value()) {
        qsettings.setValue("lastImportPath", settings.dxfImport.lastImportPath.value());
    }

    qsettings.setValue("fallbackUnit", toKey(settings.dxfImport.fallbackUnit));
    qsettings.setValue("useFallbackUnit", settings.dxfImport.useFallbackUnit);

    qsettings.endGroup();

    qsettings.beginGroup("view");

    qsettings.setValue("showAxes", settings.view.showAxes);
    qsettings.setValue("showGrid", settings.view.showGrid);
    qsettings.setValue("showLineweights", settings.view.showLineweights);

    qsettings.endGroup();
}

}   // namespace fcad::app
