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
#include <opendxf/ireadstream.hpp>
#include <opendxf/layer.hpp>
#include <opendxf/read.hpp>

#include <filesystem>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace fcad::io {

namespace {

    // Values of the $INSUNITS header variable.
    enum class DxfUnit {
        Unitless = 0,
        Inches = 1,
        Feet = 2,
        Miles = 3,
        Millimeters = 4,
        Centimeters = 5,
        Meters = 6,
        Kilometers = 7,
        Microinches = 8,
        Mils = 9,
        Yards = 10,
        Angstroms = 11,
        Nanometers = 12,
        Microns = 13,
        Decimeters = 14,
        Decameters = 15,
        Hectometers = 16,
        Gigameters = 17,
        AstronomicalUnits = 18,
        LightYears = 19,
        Parsecs = 20,
        UsSurveyFeet = 21,
        UsSurveyInches = 22,
        UsSurveyYards = 23,
        UsSurveyMiles = 24,
    };

    std::optional<domain::LengthUnit> toLengthUnit(int dxfUnit)
    {
        switch (static_cast<DxfUnit>(dxfUnit)) {
        case DxfUnit::Inches: return domain::LengthUnit::Inch;
        case DxfUnit::Feet: return domain::LengthUnit::Foot;
        case DxfUnit::Miles: return domain::LengthUnit::Mile;
        case DxfUnit::Millimeters: return domain::LengthUnit::Millimeter;
        case DxfUnit::Centimeters: return domain::LengthUnit::Centimeter;
        case DxfUnit::Meters: return domain::LengthUnit::Meter;
        case DxfUnit::Kilometers: return domain::LengthUnit::Kilometer;
        case DxfUnit::Yards: return domain::LengthUnit::Yard;
        case DxfUnit::Microns: return domain::LengthUnit::Micrometer;
        case DxfUnit::Decimeters: return domain::LengthUnit::Decimeter;
        default: return std::nullopt;
        }
    }

    std::optional<domain::LengthUnit> toLengthUnit(const odxf::Header& header)
    {
        const auto entry{ header.entries.find("$INSUNITS") };
        if (entry == header.entries.end()) {
            return std::nullopt;
        }

        const int* dxfUnit{ std::get_if<int>(&entry->second) };
        if (dxfUnit == nullptr) {
            return std::nullopt;
        }

        return toLengthUnit(*dxfUnit);
    }

    std::filesystem::path toStdFilePath(const QString& filePath)
    {
        return reinterpret_cast<const char8_t*>(filePath.toUtf8().constData());
    }

    geo::Coordinate toCoordinate(const odxf::Coordinate3d& coordinate3d)
    {
        return geo::Coordinate{ .x = coordinate3d.x, .y = coordinate3d.y };
    }

    domain::Layer toLayer(const odxf::Layer& dxfLayer)
    {
        domain::Layer layer{ QString::fromStdString(dxfLayer.name) };
        layer.visible = ((dxfLayer.flags & odxf::Layer::Flags::Frozen) == 0);

        return layer;
    }

    geo::Line toLine(const odxf::Line& dxfLine)
    {
        return geo::Line{
            .start = toCoordinate(dxfLine.start),
            .end = toCoordinate(dxfLine.end),
        };
    }

    std::optional<geo::Circle> toCircle(const odxf::Circle& dxfCircle)
    {
        const std::optional<geo::Radius> radius{ geo::Radius::fromValue(dxfCircle.radius) };
        if (!radius.has_value()) {
            return std::nullopt;
        }

        geo::Circle circle;

        circle.center = toCoordinate(dxfCircle.center);
        circle.radius = radius.value();

        return circle;
    }

    std::optional<geo::Arc> toArc(const odxf::Arc& dxfArc)
    {
        const std::optional<geo::Radius> radius{ geo::Radius::fromValue(dxfArc.radius) };
        if (!radius.has_value()) {
            return std::nullopt;
        }

        geo::Arc arc;

        arc.center = toCoordinate(dxfArc.center);
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

    private:
        void header(const odxf::Header& header) override { m_unit = toLengthUnit(header); }

        void layer(const odxf::Layer& layer) override
        {
            m_layers.push_back(toLayer(layer));
            m_layerNames.push_back(layer.name);
        }

        void arc(const odxf::Arc& arc) override
        {
            const std::optional<std::size_t> layerIndex{ core::indexOf(m_layerNames, arc.layer) };

            if (layerIndex.has_value()) {
                const std::optional<geo::Arc> maybeArc{ toArc(arc) };

                if (maybeArc.has_value()) {
                    m_layers.at(layerIndex.value()).arcs.push_back(maybeArc.value());
                }
            }
        }

        void line(const odxf::Line& line) override
        {
            const std::optional<std::size_t> layerIndex{ core::indexOf(m_layerNames, line.layer) };

            if (layerIndex.has_value()) {
                m_layers.at(layerIndex.value()).lines.push_back(toLine(line));
            }
        }

        void circle(const odxf::Circle& circle) override
        {
            const std::optional<std::size_t> layerIndex{
                core::indexOf(m_layerNames, circle.layer)
            };

            if (layerIndex.has_value()) {
                const std::optional<geo::Circle> maybeCircle{ toCircle(circle) };

                if (maybeCircle.has_value()) {
                    m_layers.at(layerIndex.value()).circles.push_back(maybeCircle.value());
                }
            }
        }

        domain::Layers m_layers;
        std::vector<std::string> m_layerNames;
        std::optional<domain::LengthUnit> m_unit;
    };

    ImportedDxf toImportedDxf(ReadStream&& stream)
    {
        const std::optional<domain::LengthUnit> unit{ stream.unit() };

        return ImportedDxf{ .layers = std::move(stream).getLayers(), .unit = unit };
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
