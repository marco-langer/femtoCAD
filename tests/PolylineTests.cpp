#include "core/Polyline.hpp"

#include "core/Arc.hpp"
#include "core/Line.hpp"

#include "Matchers/ArcMatcher.hpp"
#include "Matchers/LineMatcher.hpp"
#include "Matchers/PolylineMatcher.hpp"

#include <concepts>
#include <string>
#include <variant>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace {

testing::Matcher<PolylineSegment> IsSegment(const PolylineSegment& expected, double maxError)
{
    return std::visit(
        [maxError]<typename T>(const T& element) -> testing::Matcher<PolylineSegment> {
            if constexpr (std::same_as<Arc, T>) {
                return testing::VariantWith<Arc>(IsArc(element, maxError));
            } else {
                return testing::VariantWith<Line>(IsLine(element, maxError));
            }
        },
        expected
    );
}

testing::Matcher<std::vector<PolylineSegment>>
AreSegments(const std::vector<PolylineSegment>& expected, double maxError = 1.0e-5)
{
    std::vector<testing::Matcher<PolylineSegment>> elementMatchers;
    elementMatchers.reserve(expected.size());

    for (const PolylineSegment& expectedSegment : expected) {
        elementMatchers.push_back(IsSegment(expectedSegment, maxError));
    }

    return testing::ElementsAreArray(std::move(elementMatchers));
}

}   // namespace

struct PolylineExplodeParams
{
    Polyline polyline;
    std::vector<PolylineSegment> expectedSegments;
    std::string testCase;
};

struct PolylineExplodeTest : testing::TestWithParam<PolylineExplodeParams>
{
};

TEST_P(PolylineExplodeTest, Explode)
{
    // Arrange
    const auto& [polyline, expectedSegments, _]{ GetParam() };

    // Act
    const std::vector<PolylineSegment> segments{ explode(polyline) };

    // Assert
    EXPECT_THAT(segments, AreSegments(expectedSegments));
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    PolylineExplodeTests,
    PolylineExplodeTest,
    testing::Values(
        PolylineExplodeParams{
            .polyline = Polyline{
                .vertices = {},
                .closed = false
            },
            .expectedSegments = {},
            .testCase = "Empty_open"
        },
        PolylineExplodeParams{
            .polyline = Polyline{
                .vertices = {},
                .closed = true
            },
            .expectedSegments = {},
            .testCase = "Empty_closed"
        },
        PolylineExplodeParams{
            .polyline = Polyline{
                .vertices = {
                    Vertex{ { 0.0, 0.0 }, 0.0 },
                    Vertex{ { 1.0, 1.0 }, 0.0 }
                },
                .closed = false,
            },
            .expectedSegments = {
                PolylineSegment{
                    Line{ { 0.0, 0.0 }, { 1.0, 1.0 } }
                }
            },
            .testCase = "Single_line_open"
        },
        PolylineExplodeParams{
            .polyline = Polyline{
                .vertices = {
                    Vertex{ { 0.0, 0.0 }, 0.0 },
                    Vertex{ { 1.0, 1.0 }, 0.0 }
                },
                .closed = true
            },
            .expectedSegments = {
                PolylineSegment{
                    Line{ { 0.0, 0.0 }, { 1.0, 1.0 } }
                },
                PolylineSegment{
                    Line{ { 1.0, 1.0 }, { 0.0, 0.0 } }
                }
            },
            .testCase = "Single_line_closed"
        }
    ),
    [](const ::testing::TestParamInfo<PolylineExplodeParams>& paramInfo) {
        return paramInfo.param.testCase;
    }
);
// clang-format on
