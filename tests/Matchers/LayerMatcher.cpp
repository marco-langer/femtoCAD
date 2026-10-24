#include "LayerMatcher.hpp"

#include "test_utils/matchers/ArcMatcher.hpp"
#include "test_utils/matchers/CircleMatcher.hpp"
#include "test_utils/matchers/LineMatcher.hpp"

#include <vector>

testing::Matcher<Layer> IsLayer(const Layer& expected, double maxError)
{
    return testing::AllOf(
        testing::Field("name", &Layer::name, expected.name),
        testing::Field("visible", &Layer::visible, expected.visible),
        testing::Field("arcs", &Layer::arcs, fcad::test_utils::AreArcs(expected.arcs, maxError)),
        testing::Field(
            "lines", &Layer::lines, fcad::test_utils::AreLines(expected.lines, maxError)
        ),
        testing::Field(
            "circles", &Layer::circles, fcad::test_utils::AreCircles(expected.circles, maxError)
        )
    );
}

testing::Matcher<Layers> AreLayers(const Layers& expected, double maxError)
{
    std::vector<testing::Matcher<Layer>> elementMatchers;
    elementMatchers.reserve(expected.size());
    for (const Layer& layer : expected) {
        elementMatchers.push_back(IsLayer(layer, maxError));
    }

    return testing::ElementsAreArray(std::move(elementMatchers));
}
