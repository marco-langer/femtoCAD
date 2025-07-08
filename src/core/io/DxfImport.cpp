#include "core/io/DxfImport.hpp"

#include "core/Layer.hpp"
#include "core/Line.hpp"

#include <opendxf/entities.hpp>
#include <opendxf/ireadstream.hpp>
#include <opendxf/read.hpp>

#include <filesystem>
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

class ReadStream final : public odxf::IReadStream
{
public:
    // TODO
    ReadStream() { m_layers.emplace_back("Layer 0"); }

    std::vector<Layer> getLayers() && { return std::move(m_layers); }

    void line(const odxf::Line& line) override
    {
        m_layers.front().lines.push_back(Line{
            .start = toCoordinate(line.start),
            .end = toCoordinate(line.end),
        });
    }

private:
    std::vector<Layer> m_layers;
};

void updateWorkspace(Workspace& workspace, ReadStream& stream)
{
    workspace.clear();
    workspace.addLayers(std::move(stream).getLayers());
}

}   // namespace

tl::expected<void, QString> importDxf(Workspace& workspace, const QString& filePath)
{
    ReadStream stream;

    return odxf::read(stream, toStdFilePath(filePath))
        .transform([&workspace, &stream]() { updateWorkspace(workspace, stream); })
        .map_error([](const odxf::Error& error) { return QString::fromStdString(error.what); });
}
