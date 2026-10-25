#include "io/DxfImport.hpp"

#include "core/VectorExtensions.hpp"
#include "domain/Layer.hpp"
#include "domain/LengthUnit.hpp"
#include "geo/Arc.hpp"
#include "geo/Circle.hpp"
#include "geo/Line.hpp"
#include "io/DxfImportError.hpp"

#include <opendxf/entities.hpp>
#include <opendxf/header.hpp>
#include <opendxf/headervariables.hpp>
#include <opendxf/ireadstream.hpp>
#include <opendxf/layer.hpp>
#include <opendxf/read.hpp>

#include <QStringList>

#include <expected>
#include <filesystem>
#include <numbers>
#include <optional>
#include <string>
#include <vector>

namespace fcad::io {

namespace {

    QString toName(odxf::LengthUnit dxfUnit)
    {
        switch (dxfUnit) {
        case odxf::LengthUnit::Unitless: return QStringLiteral(u"unitless");
        case odxf::LengthUnit::Inches: return QStringLiteral(u"inches");
        case odxf::LengthUnit::Feet: return QStringLiteral(u"feet");
        case odxf::LengthUnit::Miles: return QStringLiteral(u"miles");
        case odxf::LengthUnit::Millimeters: return QStringLiteral(u"millimeters");
        case odxf::LengthUnit::Centimeters: return QStringLiteral(u"centimeters");
        case odxf::LengthUnit::Meters: return QStringLiteral(u"meters");
        case odxf::LengthUnit::Kilometers: return QStringLiteral(u"kilometers");
        case odxf::LengthUnit::Microinches: return QStringLiteral(u"microinches");
        case odxf::LengthUnit::Mils: return QStringLiteral(u"mils");
        case odxf::LengthUnit::Yards: return QStringLiteral(u"yards");
        case odxf::LengthUnit::Angstroms: return QStringLiteral(u"angstroms");
        case odxf::LengthUnit::Nanometers: return QStringLiteral(u"nanometers");
        case odxf::LengthUnit::Microns: return QStringLiteral(u"micrometers");
        case odxf::LengthUnit::Decimeters: return QStringLiteral(u"decimeters");
        case odxf::LengthUnit::Decameters: return QStringLiteral(u"decameters");
        case odxf::LengthUnit::Hectometers: return QStringLiteral(u"hectometers");
        case odxf::LengthUnit::Gigameters: return QStringLiteral(u"gigameters");
        case odxf::LengthUnit::AstronomicalUnits: return QStringLiteral(u"astronomical units");
        case odxf::LengthUnit::LightYears: return QStringLiteral(u"light years");
        case odxf::LengthUnit::Parsecs: return QStringLiteral(u"parsecs");
        case odxf::LengthUnit::USSurveyFeet: return QStringLiteral(u"US survey feet");
        case odxf::LengthUnit::USSurveyInch: return QStringLiteral(u"US survey inches");
        case odxf::LengthUnit::USSurveyYard: return QStringLiteral(u"US survey yards");
        case odxf::LengthUnit::USSurveyMile: return QStringLiteral(u"US survey miles");
        }

        return QStringLiteral(u"unknown units");
    }

    QString toName(domain::LengthUnit unit)
    {
        switch (unit) {
        case domain::LengthUnit::Micrometer: return QStringLiteral(u"micrometers");
        case domain::LengthUnit::Millimeter: return QStringLiteral(u"millimeters");
        case domain::LengthUnit::Centimeter: return QStringLiteral(u"centimeters");
        case domain::LengthUnit::Decimeter: return QStringLiteral(u"decimeters");
        case domain::LengthUnit::Meter: return QStringLiteral(u"meters");
        case domain::LengthUnit::Kilometer: return QStringLiteral(u"kilometers");
        case domain::LengthUnit::Inch: return QStringLiteral(u"inches");
        case domain::LengthUnit::Foot: return QStringLiteral(u"feet");
        case domain::LengthUnit::Yard: return QStringLiteral(u"yards");
        case domain::LengthUnit::Mile: return QStringLiteral(u"miles");
        }

        return QStringLiteral(u"unknown units");
    }

    // The supported unit a file's values are imported in.
    struct UnitConversion final
    {
        domain::LengthUnit unit;
        // Converts a value in the file's unit into unit.
        double factor{ 1.0 };
    };

    constexpr double kilometersPerAstronomicalUnit{ 149'597'870.7 };
    // US survey units are 1200/3937 m per foot instead of 0.3048 m.
    constexpr double usSurveyToInternational{ 1200.0 / 3937.0 / 0.3048 };

    // Empty if the unit isn't specified.
    std::optional<UnitConversion> toUnitConversion(odxf::LengthUnit dxfUnit)
    {
        using domain::LengthUnit;

        switch (dxfUnit) {
        case odxf::LengthUnit::Unitless: return std::nullopt;
        case odxf::LengthUnit::Inches: return UnitConversion{ .unit = LengthUnit::Inch };
        case odxf::LengthUnit::Feet: return UnitConversion{ .unit = LengthUnit::Foot };
        case odxf::LengthUnit::Miles: return UnitConversion{ .unit = LengthUnit::Mile };
        case odxf::LengthUnit::Millimeters: return UnitConversion{ .unit = LengthUnit::Millimeter };
        case odxf::LengthUnit::Centimeters: return UnitConversion{ .unit = LengthUnit::Centimeter };
        case odxf::LengthUnit::Meters: return UnitConversion{ .unit = LengthUnit::Meter };
        case odxf::LengthUnit::Kilometers: return UnitConversion{ .unit = LengthUnit::Kilometer };
        case odxf::LengthUnit::Microinches:
            return UnitConversion{ .unit = LengthUnit::Inch, .factor = 1e-6 };
        case odxf::LengthUnit::Mils:
            return UnitConversion{ .unit = LengthUnit::Inch, .factor = 1e-3 };
        case odxf::LengthUnit::Yards: return UnitConversion{ .unit = LengthUnit::Yard };
        case odxf::LengthUnit::Angstroms:
            return UnitConversion{ .unit = LengthUnit::Micrometer, .factor = 1e-4 };
        case odxf::LengthUnit::Nanometers:
            return UnitConversion{ .unit = LengthUnit::Micrometer, .factor = 1e-3 };
        case odxf::LengthUnit::Microns: return UnitConversion{ .unit = LengthUnit::Micrometer };
        case odxf::LengthUnit::Decimeters: return UnitConversion{ .unit = LengthUnit::Decimeter };
        case odxf::LengthUnit::Decameters:
            return UnitConversion{ .unit = LengthUnit::Meter, .factor = 10.0 };
        case odxf::LengthUnit::Hectometers:
            return UnitConversion{ .unit = LengthUnit::Meter, .factor = 100.0 };
        case odxf::LengthUnit::Gigameters:
            return UnitConversion{ .unit = LengthUnit::Kilometer, .factor = 1e6 };
        case odxf::LengthUnit::AstronomicalUnits:
            return UnitConversion{ .unit = LengthUnit::Kilometer,
                                   .factor = kilometersPerAstronomicalUnit };
        case odxf::LengthUnit::LightYears:
            // The IAU light year: 9'460'730'472'580'800 m.
            return UnitConversion{ .unit = LengthUnit::Kilometer, .factor = 9'460'730'472'580.8 };
        case odxf::LengthUnit::Parsecs:
            // 648000 / pi astronomical units.
            return UnitConversion{ .unit = LengthUnit::Kilometer,
                                   .factor = 648'000.0 / std::numbers::pi
                                           * kilometersPerAstronomicalUnit };
        case odxf::LengthUnit::USSurveyFeet:
            return UnitConversion{ .unit = LengthUnit::Foot, .factor = usSurveyToInternational };
        case odxf::LengthUnit::USSurveyInch:
            return UnitConversion{ .unit = LengthUnit::Inch, .factor = usSurveyToInternational };
        case odxf::LengthUnit::USSurveyYard:
            return UnitConversion{ .unit = LengthUnit::Yard, .factor = usSurveyToInternational };
        case odxf::LengthUnit::USSurveyMile:
            return UnitConversion{ .unit = LengthUnit::Mile, .factor = usSurveyToInternational };
        }

        // Treat codes outside the known range like a missing unit.
        return std::nullopt;
    }

    std::filesystem::path toStdFilePath(const QString& filePath)
    {
        return reinterpret_cast<const char8_t*>(filePath.toUtf8().constData());
    }

    geo::Coordinate toCoordinate(const odxf::Coordinate3d& coordinate3d, double scale)
    {
        return geo::Coordinate{ .x = coordinate3d.x * scale, .y = coordinate3d.y * scale };
    }

    domain::Layer toLayer(const odxf::Layer& dxfLayer)
    {
        domain::Layer layer{ QString::fromStdString(dxfLayer.name) };
        layer.visible = ((dxfLayer.flags & odxf::Layer::Flags::Frozen) == 0);

        return layer;
    }

    geo::Line toLine(const odxf::Line& dxfLine, double scale)
    {
        return geo::Line{
            .start = toCoordinate(dxfLine.start, scale),
            .end = toCoordinate(dxfLine.end, scale),
        };
    }

    std::optional<geo::Circle> toCircle(const odxf::Circle& dxfCircle, double scale)
    {
        const std::optional<geo::Radius> radius{ geo::Radius::fromValue(dxfCircle.radius * scale) };
        if (!radius.has_value()) {
            return std::nullopt;
        }

        geo::Circle circle;

        circle.center = toCoordinate(dxfCircle.center, scale);
        circle.radius = radius.value();

        return circle;
    }

    std::optional<geo::Arc> toArc(const odxf::Arc& dxfArc, double scale)
    {
        const std::optional<geo::Radius> radius{ geo::Radius::fromValue(dxfArc.radius * scale) };
        if (!radius.has_value()) {
            return std::nullopt;
        }

        geo::Arc arc;

        arc.center = toCoordinate(dxfArc.center, scale);
        arc.startAngle = core::Radians::fromDegrees(dxfArc.startAngle);
        arc.endAngle = core::Radians::fromDegrees(dxfArc.endAngle);
        arc.radius = radius.value();

        return arc;
    }

    class ReadStream final : public odxf::IReadStream
    {
    public:
        std::vector<domain::Layer> getLayers() && { return std::move(m_layers); }
        std::optional<domain::LengthUnit> unit() const { return m_unit; }
        QStringList getWarnings() && { return std::move(m_warnings); }

    private:
        // The HEADER section comes before the entities, so m_scale is set before it's used.
        void header(const odxf::Header& header) override
        {
            const std::expected<std::optional<odxf::LengthUnit>, odxf::HeaderError> dxfUnit{
                odxf::insertionUnits(header)
            };
            if (!dxfUnit.has_value()) {
                m_warnings.push_back(
                    QStringLiteral(u"The file's length unit is invalid and was ignored.")
                );
                return;
            }

            if (!dxfUnit.value().has_value()) {
                return;
            }

            const std::optional<UnitConversion> conversion{
                toUnitConversion(dxfUnit.value().value())
            };
            if (!conversion.has_value()) {
                return;
            }

            m_unit = conversion.value().unit;
            m_scale = conversion.value().factor;

            if (m_scale != 1.0) {
                m_warnings.push_back(
                    QStringView{ u"The file is in %1, which femtoCAD doesn't support. It was "
                                 u"converted to %2." }
                        .arg(toName(dxfUnit.value().value()), toName(m_unit.value()))
                );
            }
        }

        void layer(const odxf::Layer& layer) override
        {
            m_layers.push_back(toLayer(layer));
            m_layerNames.push_back(layer.name);
        }

        void arc(const odxf::Arc& arc) override
        {
            const std::optional<std::size_t> layerIndex{ core::indexOf(m_layerNames, arc.layer) };

            if (layerIndex.has_value()) {
                const std::optional<geo::Arc> maybeArc{ toArc(arc, m_scale) };

                if (maybeArc.has_value()) {
                    m_layers.at(layerIndex.value()).arcs.push_back(maybeArc.value());
                }
            }
        }

        void line(const odxf::Line& line) override
        {
            const std::optional<std::size_t> layerIndex{ core::indexOf(m_layerNames, line.layer) };

            if (layerIndex.has_value()) {
                m_layers.at(layerIndex.value()).lines.push_back(toLine(line, m_scale));
            }
        }

        void circle(const odxf::Circle& circle) override
        {
            const std::optional<std::size_t> layerIndex{
                core::indexOf(m_layerNames, circle.layer)
            };

            if (layerIndex.has_value()) {
                const std::optional<geo::Circle> maybeCircle{ toCircle(circle, m_scale) };

                if (maybeCircle.has_value()) {
                    m_layers.at(layerIndex.value()).circles.push_back(maybeCircle.value());
                }
            }
        }

        domain::Layers m_layers;
        std::vector<std::string> m_layerNames;
        std::optional<domain::LengthUnit> m_unit;
        // Converts the file's values into m_unit.
        double m_scale{ 1.0 };
        QStringList m_warnings;
    };

    ImportedDxf toImportedDxf(ReadStream&& stream)
    {
        const std::optional<domain::LengthUnit> unit{ stream.unit() };

        return ImportedDxf{
            .layers = std::move(stream).getLayers(),
            .unit = unit,
            .warnings = std::move(stream).getWarnings(),
        };
    }

    QString toErrorMessage(const odxf::Error& error)
    {
        if (error.lineNumber.has_value()) {
            if (!error.what.empty()) {
                return QStringView{ u"Line %1: %2" }.arg(
                    QString::number(error.lineNumber.value()), QString::fromStdString(error.what)
                );
            }

            return QStringView{ u"Line %1" }.arg(QString::number(error.lineNumber.value()));
        }

        return QString::fromStdString(error.what);
    }

    DxfImportError toErrorCode(odxf::Error::Type type)
    {
        switch (type) {
        case odxf::Error::Type::FileOpenError: return DxfImportError::CannotOpenFile;
        case odxf::Error::Type::InvalidFile: return DxfImportError::InvalidFile;
        }

        return DxfImportError::InvalidFile;
    }

    core::Error toError(const odxf::Error& error)
    {
        return core::Error{ .code = toErrorCode(error.type), .message = toErrorMessage(error) };
    }

}   // namespace

core::Result<ImportedDxf> importDxf(const QString& filePath)
{
    ReadStream stream;

    return odxf::read(stream, toStdFilePath(filePath))
        .transform([&stream] { return toImportedDxf(std::move(stream)); })
        .transform_error([](const odxf::Error& error) { return toError(error); });
}

}   // namespace fcad::io
