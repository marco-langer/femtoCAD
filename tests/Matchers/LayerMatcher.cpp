#include "LayerMatcher.hpp"

#include "ArcMatcher.hpp"
#include "CircleMatcher.hpp"
#include "LineMatcher.hpp"

#include <vector>

testing::Matcher<Layer> IsLayer(const Layer& expected, double maxError)
{
    return testing::AllOf(
        testing::Field("name", &Layer::name, expected.name),
        testing::Field("visible", &Layer::visible, expected.visible),
        testing::Field("arcs", &Layer::arcs, AreArcs(expected.arcs, maxError)),
        testing::Field("lines", &Layer::lines, AreLines(expected.lines, maxError)),
        testing::Field("circles", &Layer::circles, AreCircles(expected.circles, maxError))
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
