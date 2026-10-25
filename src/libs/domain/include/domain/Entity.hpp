#pragma once

#include "core/StrongUuid.hpp"
#include "domain/Color.hpp"
#include "geo/Arc.hpp"
#include "geo/BoundingBox.hpp"
#include "geo/Circle.hpp"
#include "geo/Line.hpp"

#include <utility>
#include <vector>

namespace fcad::domain {

using EntityId = core::StrongUuid<struct EntityIdTag>;

// A geometry with an identity, see ADR 0001. Move-only, so that no two entities share an id;
// clone() creates a copy with a new id.
template <typename Geometry>
class Entity final
{
public:
    explicit Entity(Geometry geometry)
        : m_id{ EntityId::generate() }
        , m_geometry{ std::move(geometry) }
    {}

    Entity(const Entity&) = delete;
    Entity(Entity&&) = default;
    ~Entity() = default;

    Entity& operator=(const Entity&) = delete;
    Entity& operator=(Entity&&) = default;

    [[nodiscard]] Entity clone() const
    {
        Entity entity{ m_geometry };
        entity.m_color = m_color;

        return entity;
    }

    EntityId id() const { return m_id; }

    const Geometry& geometry() const { return m_geometry; }
    void setGeometry(Geometry geometry) { m_geometry = std::move(geometry); }

    const EntityColor& color() const { return m_color; }
    void setColor(EntityColor color) { m_color = color; }

private:
    EntityId m_id;
    Geometry m_geometry;
    EntityColor m_color{ ByLayer{} };
};

using Arc = Entity<geo::Arc>;
using Circle = Entity<geo::Circle>;
using Line = Entity<geo::Line>;

using Arcs = std::vector<Arc>;
using Circles = std::vector<Circle>;
using Lines = std::vector<Line>;

template <typename Geometry>
geo::BoundingBox boundingBox(const Entity<Geometry>& entity)
{
    return geo::boundingBox(entity.geometry());
}

}   // namespace fcad::domain
