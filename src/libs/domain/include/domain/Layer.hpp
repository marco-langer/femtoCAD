#pragma once

#include "core/StrongUuid.hpp"
#include "domain/Color.hpp"
#include "domain/Entity.hpp"
#include "domain/Lineweight.hpp"
#include "geo/BoundingBox.hpp"

#include <QString>

#include <optional>
#include <vector>

namespace fcad::domain {

using LayerId = core::StrongUuid<struct LayerIdTag>;

// Move-only like the entities; clone() gives the copy and all its entities new ids.
class Layer final
{
public:
    explicit Layer(QString name);
    Layer(const Layer&) = delete;
    Layer(Layer&&) = default;
    ~Layer() = default;

    Layer& operator=(const Layer&) = delete;
    Layer& operator=(Layer&&) = default;

    [[nodiscard]] Layer clone() const;

    LayerId id() const;

    const QString& name() const;
    void setName(QString name);

    bool isVisible() const;
    void setVisible(bool visible);

    const Color& color() const;
    void setColor(Color color);

    const LayerLineweight& lineweight() const;
    void setLineweight(LayerLineweight lineweight);

    const Arcs& arcs() const;
    const Circles& circles() const;
    const Lines& lines() const;

    void add(Arc arc);
    void add(Circle circle);
    void add(Line line);

private:
    LayerId m_id;
    QString m_name;
    bool m_visible{ true };
    Color m_color{ Adaptive{} };
    LayerLineweight m_lineweight{ DefaultLineweight{} };
    Arcs m_arcs;
    Circles m_circles;
    Lines m_lines;
};

using Layers = std::vector<Layer>;

std::optional<geo::BoundingBox> boundingBox(const Layer& layer);

}   // namespace fcad::domain
