#include "domain/Layer.hpp"

#include <algorithm>
#include <ranges>

namespace fcad::domain {

namespace {

    template <typename Geometry>
    std::optional<geo::BoundingBox> boundingBox(const std::vector<Entity<Geometry>>& entities)
    {
        if (entities.empty()) {
            return std::nullopt;
        }

        geo::BoundingBox bb{ boundingBox(entities.front()) };

        for (const Entity<Geometry>& entity : entities | std::views::drop(1)) {
            bb = geo::united(bb, boundingBox(entity));
        }

        return bb;
    }

}   // namespace

Layer::Layer(QString name)
    : m_id{ LayerId::generate() }
    , m_name{ std::move(name) }
{}

Layer Layer::clone() const
{
    Layer layer{ m_name };
    layer.m_visible = m_visible;
    layer.m_color = m_color;
    layer.m_lineweight = m_lineweight;

    for (const Arc& arc : m_arcs) {
        layer.add(arc.clone());
    }
    for (const Circle& circle : m_circles) {
        layer.add(circle.clone());
    }
    for (const Line& line : m_lines) {
        layer.add(line.clone());
    }

    return layer;
}

LayerId Layer::id() const { return m_id; }

const QString& Layer::name() const { return m_name; }

void Layer::setName(QString name) { m_name = std::move(name); }

bool Layer::isVisible() const { return m_visible; }

void Layer::setVisible(bool visible) { m_visible = visible; }

const Color& Layer::color() const { return m_color; }

void Layer::setColor(Color color) { m_color = color; }

const LayerLineweight& Layer::lineweight() const { return m_lineweight; }

void Layer::setLineweight(LayerLineweight lineweight) { m_lineweight = lineweight; }

const Arcs& Layer::arcs() const { return m_arcs; }

const Circles& Layer::circles() const { return m_circles; }

const Lines& Layer::lines() const { return m_lines; }

void Layer::add(Arc arc) { m_arcs.push_back(std::move(arc)); }

void Layer::add(Circle circle) { m_circles.push_back(std::move(circle)); }

void Layer::add(Line line) { m_lines.push_back(std::move(line)); }

std::optional<geo::BoundingBox> boundingBox(const Layer& layer)
{
    return geo::united(
        boundingBox(layer.arcs()), boundingBox(layer.lines()), boundingBox(layer.circles())
    );
};

}   // namespace fcad::domain
