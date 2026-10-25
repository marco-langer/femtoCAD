#include "io/DxfImport.hpp"

#include "core/VectorExtensions.hpp"
#include "domain/Layer.hpp"
#include "domain/LengthUnit.hpp"
#include "geo/Arc.hpp"
#include "geo/Circle.hpp"
#include "io/DxfImportError.hpp"

#include "DxfColors.hpp"
#include "DxfEntities.hpp"
#include "DxfLinetypes.hpp"
#include "DxfLineweights.hpp"
#include "DxfUnits.hpp"

#include <opendxf/entities.hpp>
#include <opendxf/header.hpp>
#include <opendxf/headervariables.hpp>
#include <opendxf/ireadstream.hpp>
#include <opendxf/layer.hpp>
#include <opendxf/read.hpp>
#include <opendxf/tables.hpp>

#include <QStringList>

#include <cstddef>
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
        domain::Linetypes getLinetypes() && { return std::move(m_linetypes).takeLinetypes(); }
        domain::LinetypeScale linetypeScale() const { return m_linetypeScale; }
        std::vector<domain::Layer> getLayers() && { return std::move(m_layers); }
        std::optional<domain::LengthUnit> unit() const { return m_unit; }
        QStringList getWarnings() &&
        {
            QStringList warnings{ std::move(m_warnings) };
            warnings.append(unsupportedColorWarnings(m_byBlockCount, m_byEntityCount));
            warnings.append(lineweightWarnings(m_lineweightCounts));
            warnings.append(linetypeWarnings(m_linetypeCounts));

            return warnings;
        }

    private:
        // The HEADER section comes before the tables and entities, so m_scale is set before it's
        // used.
        void header(const odxf::Header& header) override
        {
            readLinetypeScale(header);

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

        void readLinetypeScale(const odxf::Header& header)
        {
            const std::expected<std::optional<double>, odxf::HeaderError> scale{
                odxf::lineTypeScale(header)
            };
            if (!scale.has_value()) {
                m_warnings.push_back(
                    QStringLiteral(u"The file's linetype scale is invalid. It uses 1.0 instead.")
                );
                return;
            }

            if (scale.value().has_value()) {
                // lineTypeScale() only returns finite values greater than 0.
                m_linetypeScale = domain::LinetypeScale::fromValue(scale.value().value()).value();
            }
        }

        void lineType(const odxf::LineType& lineType) override
        {
            switch (m_linetypes.add(lineType, m_scale)) {
            case AddedLinetype::Added:
            case AddedLinetype::BuiltIn: break;
            case AddedLinetype::Complex: ++m_linetypeCounts.complexLinetypes; break;
            case AddedLinetype::Invalid: ++m_linetypeCounts.invalidLinetypes; break;
            }
        }

        // DXF writes the LTYPE table before the LAYER table, so the layer's linetype is known.
        void layer(const odxf::Layer& layer) override
        {
            if (!isValidLayerLineweight(layer.lineweight)) {
                ++m_lineweightCounts.invalidLayers;
            }

            domain::Layer& added{ m_layers.emplace_back(toLayer(layer)) };
            const std::optional<domain::LinetypeId> linetype{ m_linetypes.find(layer.lineType) };
            if (linetype.has_value()) {
                added.setLinetype(linetype.value());
            } else {
                ++m_linetypeCounts.unknownLayerLinetypes;
            }
            m_layerNames.push_back(layer.name);
        }

        void arc(const odxf::Arc& arc) override
        {
            const std::optional<std::size_t> layerIndex{ core::indexOf(m_layerNames, arc.layer) };

            if (layerIndex.has_value()) {
                const std::optional<geo::Arc> maybeArc{ toArc(arc, m_scale) };

                if (maybeArc.has_value()) {
                    add(layerIndex.value(), domain::Arc{ maybeArc.value() }, arc);
                }
            }
        }

        void line(const odxf::Line& line) override
        {
            const std::optional<std::size_t> layerIndex{ core::indexOf(m_layerNames, line.layer) };

            if (layerIndex.has_value()) {
                add(layerIndex.value(), domain::Line{ toLine(line, m_scale) }, line);
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
                    add(layerIndex.value(), domain::Circle{ maybeCircle.value() }, circle);
                }
            }
        }

        template <typename Entity>
        void setLinetype(Entity& entity, const odxf::Entity& dxfEntity)
        {
            if (isByBlock(dxfEntity.lineType)) {
                ++m_linetypeCounts.byBlockEntities;
                entity.setLinetype(domain::continuousLinetypeId());
            } else if (!isByLayer(dxfEntity.lineType)) {
                const std::optional<domain::LinetypeId> linetype{
                    m_linetypes.find(dxfEntity.lineType)
                };
                if (!linetype.has_value()) {
                    ++m_linetypeCounts.unknownEntityLinetypes;
                }
                entity.setLinetype(linetype.value_or(domain::continuousLinetypeId()));
            }

            const std::optional<domain::LinetypeScale> scale{
                domain::LinetypeScale::fromValue(dxfEntity.lineTypeScale)
            };
            if (scale.has_value()) {
                entity.setLinetypeScale(scale.value());
            } else {
                ++m_linetypeCounts.invalidEntityScales;
            }
        }

        template <typename Entity>
        void add(std::size_t layerIndex, Entity entity, const odxf::Entity& dxfEntity)
        {
            if (!dxfEntity.trueColor.has_value()) {
                if (dxfEntity.color == byBlockIndex) {
                    ++m_byBlockCount;
                } else if (dxfEntity.color == byEntityIndex) {
                    ++m_byEntityCount;
                }
            }

            if (dxfEntity.lineweight == byBlockLineweightValue) {
                ++m_lineweightCounts.byBlockEntities;
            } else if (!isValidEntityLineweight(dxfEntity.lineweight)) {
                ++m_lineweightCounts.invalidEntities;
            }

            entity.setColor(toEntityColor(dxfEntity.color, dxfEntity.trueColor));
            entity.setLineweight(toEntityLineweight(dxfEntity.lineweight));
            setLinetype(entity, dxfEntity);
            m_layers.at(layerIndex).add(std::move(entity));
        }


        DxfLinetypes m_linetypes;
        domain::LinetypeScale m_linetypeScale{ 1.0 };
        domain::Layers m_layers;
        std::vector<std::string> m_layerNames;
        std::optional<domain::LengthUnit> m_unit;
        // Converts the file's values into m_unit.
        double m_scale{ 1.0 };
        QStringList m_warnings;
        // Entities whose color is ByBlock or ByEntity, for the warnings.
        int m_byBlockCount{ 0 };
        int m_byEntityCount{ 0 };
        LineweightCounts m_lineweightCounts;
        LinetypeCounts m_linetypeCounts;
    };

    ImportedDxf toImportedDxf(ReadStream&& stream)
    {
        const std::optional<domain::LengthUnit> unit{ stream.unit() };
        const domain::LinetypeScale linetypeScale{ stream.linetypeScale() };

        return ImportedDxf{
            .linetypes = std::move(stream).getLinetypes(),
            .linetypeScale = linetypeScale,
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
