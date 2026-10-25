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
    : m_name{ std::move(name) }
{}

const QString& Layer::name() const { return m_name; }

void Layer::setName(QString name) { m_name = std::move(name); }

bool Layer::isVisible() const { return m_visible; }

void Layer::setVisible(bool visible) { m_visible = visible; }

const Color& Layer::color() const { return m_color; }

void Layer::setColor(Color color) { m_color = color; }

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
