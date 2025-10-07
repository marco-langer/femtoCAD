#include "PolylineMatcher.hpp"

#include "CoordinateMatcher.hpp"

#include <vector>

namespace {

testing::Matcher<Vertex> IsVertex(const Vertex& expected, double maxError)
{
    return testing::AllOf(
        testing::Field("position", &Vertex::position, IsCoordinate(expected.position, maxError)),
        testing::Field("bulge", &Vertex::bulge, testing::DoubleNear(expected.bulge, maxError))
    );
}

testing::Matcher<std::vector<Vertex>>
AreVertices(const std::vector<Vertex>& expected, double maxError)
{
    std::vector<testing::Matcher<Vertex>> elementMatchers;
    elementMatchers.reserve(expected.size());
    for (const Vertex& vertex : expected) {
        elementMatchers.push_back(IsVertex(vertex, maxError));
    }

    return testing::ElementsAreArray(std::move(elementMatchers));
}

}   // namespace

testing::Matcher<Polyline> IsPolyline(const Polyline& expected, double maxError)
{
    return testing::AllOf(
        testing::Field("vertices", &Polyline::vertices, AreVertices(expected.vertices, maxError)),
        testing::Field("closed", &Polyline::closed, expected.closed)
    );
}
