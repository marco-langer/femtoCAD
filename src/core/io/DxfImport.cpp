#include "core/io/DxfImport.hpp"

#include "core/Layer.hpp"
#include "core/Line.hpp"
#include "core/utility/VectorExtensions.hpp"

#include <opendxf/entities.hpp>
#include <opendxf/ireadstream.hpp>
#include <opendxf/layer.hpp>
#include <opendxf/read.hpp>

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace {

std::filesystem::path toStdFilePath(const QString& filePath)
{
    return reinterpret_cast<const char8_t*>(filePath.toUtf8().constData());
}

Coordinate toCoordinate(const odxf::Coordinate3d& coordinate3d)
{
    return Coordinate{ .x = coordinate3d.x, .y = coordinate3d.y };
}

Layer toLayer(const odxf::Layer& dxfLayer)
{
    Layer layer{ QString::fromStdString(dxfLayer.name) };
    layer.visible = ((dxfLayer.flags & odxf::Layer::Flags::Frozen) == 0);

    return layer;
}

Line toLine(const odxf::Line& dxfLine)
{
    return Line{
        .start = toCoordinate(dxfLine.start),
        .end = toCoordinate(dxfLine.end),
    };
}

class ReadStream final : public odxf::IReadStream
{
public:
    std::vector<Layer> getLayers() && { return std::move(m_layers); }

private:
    void layer(const odxf::Layer& layer) override
    {
        m_layers.push_back(toLayer(layer));
        m_layerNames.push_back(layer.name);
    }

    void line(const odxf::Line& line) override
    {
        const std::optional<std::size_t> layerIndex{ indexOf(m_layerNames, line.layer) };

        if (layerIndex.has_value()) {
            m_layers.at(layerIndex.value()).lines.push_back(toLine(line));
        }
    }

    Layers m_layers;
    std::vector<std::string> m_layerNames;
};

void updateWorkspace(Workspace& workspace, ReadStream& stream)
{
    workspace.clear();
    workspace.addLayers(std::move(stream).getLayers());
}

QString toErrorMessage(const odxf::Error& error)
{
    if (error.lineNumber.has_value()) {
        return QStringView{ u"Line '%1': %2" }.arg(
            QString::number(error.lineNumber.value()), QString::fromStdString(error.what)
        );
    }

    return QString::fromStdString(error.what);
}

}   // namespace

tl::expected<void, QString> importDxf(Workspace& workspace, const QString& filePath)
{
    ReadStream stream;

    return odxf::read(stream, toStdFilePath(filePath))
        .transform([&workspace, &stream] { updateWorkspace(workspace, stream); })
        .map_error([](const odxf::Error& error) { return toErrorMessage(error); });
}
