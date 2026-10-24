#include "LayerMatcher.hpp"

#include "test_utils/matchers/ArcMatcher.hpp"
#include "test_utils/matchers/CircleMatcher.hpp"
#include "test_utils/matchers/LineMatcher.hpp"

#include <vector>

testing::Matcher<fcad::domain::Layer> IsLayer(const fcad::domain::Layer& expected, double maxError)
{
    return testing::AllOf(
        testing::Field("name", &fcad::domain::Layer::name, expected.name),
        testing::Field("visible", &fcad::domain::Layer::visible, expected.visible),
        testing::Field(
            "arcs", &fcad::domain::Layer::arcs, fcad::test_utils::AreArcs(expected.arcs, maxError)
        ),
        testing::Field(
            "lines",
            &fcad::domain::Layer::lines,
            fcad::test_utils::AreLines(expected.lines, maxError)
        ),
        testing::Field(
            "circles",
            &fcad::domain::Layer::circles,
            fcad::test_utils::AreCircles(expected.circles, maxError)
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
