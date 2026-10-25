#include "io/DxfImport.hpp"

#include "core/VectorExtensions.hpp"
#include "domain/Layer.hpp"
#include "domain/LengthUnit.hpp"
#include "geo/Arc.hpp"
#include "geo/Circle.hpp"
#include "io/DxfImportError.hpp"

#include "DxfEntities.hpp"
#include "DxfUnits.hpp"

#include <opendxf/entities.hpp>
#include <opendxf/header.hpp>
#include <opendxf/headervariables.hpp>
#include <opendxf/ireadstream.hpp>
#include <opendxf/layer.hpp>
#include <opendxf/read.hpp>

#include <QStringList>

#include <expected>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace fcad::io {

namespace {

    std::filesystem::path toStdFilePath(const QString& filePath)
    {
        return reinterpret_cast<const char8_t*>(filePath.toUtf8().constData());
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
