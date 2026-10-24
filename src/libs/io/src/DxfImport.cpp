#include "io/DxfImport.hpp"

#include "core/VectorExtensions.hpp"
#include "domain/Document.hpp"
#include "domain/Layer.hpp"
#include "geo/Arc.hpp"
#include "geo/Circle.hpp"
#include "geo/Line.hpp"

#include <opendxf/entities.hpp>
#include <opendxf/ireadstream.hpp>
#include <opendxf/layer.hpp>
#include <opendxf/read.hpp>

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

    private:
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
    };

    domain::Document toDocument(ReadStream&& stream)
    {
        domain::Document document;
        document.addLayers(std::move(stream).getLayers());

        return document;
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

}   // namespace

tl::expected<domain::Document, QString> importDxf(const QString& filePath)
{
    ReadStream stream;

    return odxf::read(stream, toStdFilePath(filePath))
        .transform([&stream] { return toDocument(std::move(stream)); })
        .map_error([](const odxf::Error& error) { return toErrorMessage(error); });
}

}   // namespace fcad::io
