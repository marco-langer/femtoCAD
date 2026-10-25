#include "LayerMatcher.hpp"

#include "test_utils/matchers/ArcMatcher.hpp"
#include "test_utils/matchers/CircleMatcher.hpp"
#include "test_utils/matchers/LineMatcher.hpp"

#include <vector>

namespace {

// Compares the geometries only, since the ids are random.
template <typename Geometry, typename GeometryMatcher>
testing::Matcher<std::vector<fcad::domain::Entity<Geometry>>>
AreEntities(const std::vector<fcad::domain::Entity<Geometry>>& expected, GeometryMatcher isGeometry)
{
    std::vector<testing::Matcher<fcad::domain::Entity<Geometry>>> elementMatchers;
    elementMatchers.reserve(expected.size());
    for (const fcad::domain::Entity<Geometry>& entity : expected) {
        elementMatchers.push_back(
            testing::Property(
                "geometry", &fcad::domain::Entity<Geometry>::geometry, isGeometry(entity.geometry())
            )
        );
    }

    return testing::ElementsAreArray(std::move(elementMatchers));
}

}   // namespace

testing::Matcher<fcad::domain::Layer> IsLayer(const fcad::domain::Layer& expected, double maxError)
{
    return testing::AllOf(
        testing::Field("name", &fcad::domain::Layer::name, expected.name),
        testing::Field("visible", &fcad::domain::Layer::visible, expected.visible),
        testing::Field("color", &fcad::domain::Layer::color, expected.color),
        testing::Field(
            "arcs",
            &fcad::domain::Layer::arcs,
            AreEntities(
                expected.arcs,
                [maxError](const fcad::geo::Arc& arc) {
                    return fcad::test_utils::IsArc(arc, maxError);
                }
            )
        ),
        testing::Field(
            "lines",
            &fcad::domain::Layer::lines,
            AreEntities(
                expected.lines,
                [maxError](const fcad::geo::Line& line) {
                    return fcad::test_utils::IsLine(line, maxError);
                }
            )
        ),
        testing::Field(
            "circles",
            &fcad::domain::Layer::circles,
            AreEntities(expected.circles, [maxError](const fcad::geo::Circle& circle) {
                return fcad::test_utils::IsCircle(circle, maxError);
            })
        )
    );
}

testing::Matcher<fcad::domain::Layers>
AreLayers(const fcad::domain::Layers& expected, double maxError)
{
    std::vector<testing::Matcher<fcad::domain::Layer>> elementMatchers;
    elementMatchers.reserve(expected.size());
    for (const fcad::domain::Layer& layer : expected) {
        elementMatchers.push_back(IsLayer(layer, maxError));
    }

    return testing::ElementsAreArray(std::move(elementMatchers));
}
