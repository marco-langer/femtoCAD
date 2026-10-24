#include "geo/Arc.hpp"

#include <algorithm>
#include <optional>

namespace fcad::geo {

namespace {

    // TODO consider to generalize this
    // (maybe with some small vector (boost::small_vector) lookup cache?)
    class TrigonometryCache
    {
    public:
        explicit TrigonometryCache(core::Radians angle)
            : m_angle{ angle }
        {}

        double sin()
        {
            if (m_sin.has_value()) {
                return m_sin.value();
            }

            m_sin = core::sin(m_angle);

            return m_sin.value();
        }

        double cos()
        {
            if (m_cos.has_value()) {
                return m_cos.value();
            }

            m_cos = core::cos(m_angle);

            return m_cos.value();
        }

    private:
        core::Radians m_angle;
        std::optional<double> m_sin;
        std::optional<double> m_cos;
    };

}   // namespace

BoundingBox boundingBox(const Arc& arc)
{
    auto isAngleInArc{ [&arc](core::Radians angle) {
        if (arc.startAngle <= arc.endAngle) {
            return angle >= arc.startAngle && angle <= arc.endAngle;
        }

        return angle >= arc.startAngle || angle <= arc.endAngle;
    } };

    TrigonometryCache startAngle{ arc.startAngle };
    TrigonometryCache endAngle{ arc.endAngle };

    return {
        .min = {
            .x = isAngleInArc(core::Radians::pi)
                ? arc.center.x - arc.radius
                : arc.center.x + std::min(startAngle.cos(), endAngle.cos()),
            .y = isAngleInArc(core::Radians::piThreeHalf)
                ? arc.center.y - arc.radius
                : arc.center.y + std::min(startAngle.sin(), endAngle.sin()),
        },
        .max = {
            .x = isAngleInArc(core::Radians::zero)
                ? arc.center.x + arc.radius
                : arc.center.x + std::max(startAngle.cos(), endAngle.cos()),
            .y = isAngleInArc(core::Radians::piHalf)
                ? arc.center.y + arc.radius
                : arc.center.y + std::max(startAngle.sin(), endAngle.sin()),
        },
    };
}

}   // namespace fcad::geo
