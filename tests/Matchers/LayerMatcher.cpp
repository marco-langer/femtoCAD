#include "LayerMatcher.hpp"

#include "LineMatcher.hpp"

testing::Matcher<Layer> IsLayer(const Layer& expected, double maxError)
{
    return testing::AllOf(testing::Field("lines", &Layer::lines, AreLines(expected.lines, maxError))
    );
}
