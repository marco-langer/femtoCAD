#include "core/Arc.hpp"

#include "Matchers/BoundingBoxMatcher.hpp"

#include <fmt/core.h>
#include <gtest/gtest.h>

struct ArcBoundingBoxParams final
{
    Radians startAngle;
    Radians endAngle;
    BoundingBox expectedBoundingBox;
};

struct ArcBoundingBoxFixture : public testing::TestWithParam<ArcBoundingBoxParams>
{
};

TEST_P(ArcBoundingBoxFixture, boundingBox)
{
    // Arrange
    const auto& [startAngle, endAngle, expectedBoundingBox]{ GetParam() };

    const Arc arc{
        .center = { 1.0, 1.0 },
        .startAngle = startAngle,
        .endAngle = endAngle,
        .radius = Radius{ 1.0 },
    };

    // Act
    const BoundingBox arcBoundingBox{ boundingBox(arc) };

    // Assert
    EXPECT_THAT(arcBoundingBox, IsBoundingBox(expectedBoundingBox));
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    ArcBoundingBox,
    ArcBoundingBoxFixture,
    testing::Values(
        ArcBoundingBoxParams{
            .startAngle = Radians::fromDegrees(0.0),
            .endAngle = Radians::fromDegrees(90.0),
            .expectedBoundingBox = {
                .min = { 1.0, 1.0 },
                .max = { 2.0, 2.0 },
            },
        },
        ArcBoundingBoxParams{
            .startAngle = Radians::fromDegrees(0.0),
            .endAngle = Radians::fromDegrees(180.0),
            .expectedBoundingBox = {
                .min = { 0.0, 1.0 },
                .max = { 2.0, 2.0 },
            },
        },
        ArcBoundingBoxParams{
            .startAngle = Radians::fromDegrees(0.0),
            .endAngle = Radians::fromDegrees(270.0),
            .expectedBoundingBox = {
                .min = { 0.0, 0.0 },
                .max = { 2.0, 2.0 },
            },
        },
        ArcBoundingBoxParams{
            .startAngle = Radians::fromDegrees(0.0),
            .endAngle = Radians::fromDegrees(360.0),
            .expectedBoundingBox = {
                .min = { 0.0, 0.0 },
                .max = { 2.0, 2.0 },
            },
        },
        ArcBoundingBoxParams{
            .startAngle = Radians::fromDegrees(0.0),
            .endAngle = Radians::fromDegrees(0.0),
            .expectedBoundingBox = {
                .min = { 2.0, 1.0 },
                .max = { 2.0, 1.0 },
            },
        },
        ArcBoundingBoxParams{
            .startAngle = Radians::fromDegrees(90.0),
            .endAngle = Radians::fromDegrees(180.0),
            .expectedBoundingBox = {
                .min = { 0.0, 1.0 },
                .max = { 1.0, 2.0 },
            },
        },
        ArcBoundingBoxParams{
            .startAngle = Radians::fromDegrees(90.0),
            .endAngle = Radians::fromDegrees(270.0),
            .expectedBoundingBox = {
                .min = { 0.0, 0.0 },
                .max = { 1.0, 2.0 },
            },
        },
        ArcBoundingBoxParams{
            .startAngle = Radians::fromDegrees(90.0),
            .endAngle = Radians::fromDegrees(360.0),
            .expectedBoundingBox = {
                .min = { 0.0, 0.0 },
                .max = { 2.0, 2.0 },
            },
        },
        ArcBoundingBoxParams{
            .startAngle = Radians::fromDegrees(90.0),
            .endAngle = Radians::fromDegrees(90.0),
            .expectedBoundingBox = {
                .min = { 1.0, 2.0 },
                .max = { 1.0, 2.0 },
            },
        },
        ArcBoundingBoxParams{
            .startAngle = Radians::fromDegrees(180.0),
            .endAngle = Radians::fromDegrees(270.0),
            .expectedBoundingBox = {
                .min = { 0.0, 0.0 },
                .max = { 1.0, 1.0 },
            },
        },
        ArcBoundingBoxParams{
            .startAngle = Radians::fromDegrees(180.0),
            .endAngle = Radians::fromDegrees(360.0),
            .expectedBoundingBox = {
                .min = { 0.0, 0.0 },
                .max = { 2.0, 1.0 },
            },
        },
        ArcBoundingBoxParams{
            .startAngle = Radians::fromDegrees(180.0),
            .endAngle = Radians::fromDegrees(90.0),
            .expectedBoundingBox = {
                .min = { 0.0, 0.0 },
                .max = { 2.0, 2.0 },
            },
        },
        ArcBoundingBoxParams{
            .startAngle = Radians::fromDegrees(180.0),
            .endAngle = Radians::fromDegrees(180.0),
            .expectedBoundingBox = {
                .min = { 0.0, 1.0 },
                .max = { 0.0, 1.0 },
            },
        },
        ArcBoundingBoxParams{
            .startAngle = Radians::fromDegrees(270.0),
            .endAngle = Radians::fromDegrees(360.0),
            .expectedBoundingBox = {
                .min = { 1.0, 0.0 },
                .max = { 2.0, 1.0 },
            },
        },
        ArcBoundingBoxParams{
            .startAngle = Radians::fromDegrees(270.0),
            .endAngle = Radians::fromDegrees(90.0),
            .expectedBoundingBox = {
                .min = { 1.0, 0.0 },
                .max = { 2.0, 2.0 },
            },
        },
        ArcBoundingBoxParams{
            .startAngle = Radians::fromDegrees(270.0),
            .endAngle = Radians::fromDegrees(180.0),
            .expectedBoundingBox = {
                .min = { 0.0, 0.0 },
                .max = { 2.0, 2.0 },
            },
        },
        ArcBoundingBoxParams{
            .startAngle = Radians::fromDegrees(270.0),
            .endAngle = Radians::fromDegrees(270.0),
            .expectedBoundingBox = {
                .min = { 1.0, 0.0 },
                .max = { 1.0, 0.0 },
            },
        },
        ArcBoundingBoxParams{
            .startAngle = Radians::fromDegrees(30.0),
            .endAngle = Radians::fromDegrees(60.0),
            .expectedBoundingBox = {
                .min = { 1.5, 1.5 },
                .max = { 1.866025404, 1.866025404 },
            },
        },
        ArcBoundingBoxParams{
            .startAngle = Radians::fromDegrees(30.0),
            .endAngle = Radians::fromDegrees(120.0),
            .expectedBoundingBox = {
                .min = { 0.5, 1.5 },
                .max = { 1.866025404, 2.0 },
            },
        },
        ArcBoundingBoxParams{
            .startAngle = Radians::fromDegrees(60.0),
            .endAngle = Radians::fromDegrees(150.0),
            .expectedBoundingBox = {
                .min = { 1.0 - 0.866025404, 1.5 },
                .max = { 1.5, 2.0 },
            },
        },
        ArcBoundingBoxParams{
            .startAngle = Radians::fromDegrees(60.0),
            .endAngle = Radians::fromDegrees(210.0),
            .expectedBoundingBox = {
                .min = { 0.0, 0.5 },
                .max = { 1.5, 2.0 },
            },
        },
        ArcBoundingBoxParams{
            .startAngle = Radians::fromDegrees(60.0),
            .endAngle = Radians::fromDegrees(330.0),
            .expectedBoundingBox = {
                .min = { 0.0, 0.0 },
                .max = { 1.866025404, 2.0 },
            },
        },
        ArcBoundingBoxParams{
            .startAngle = Radians::fromDegrees(30.0),
            .endAngle = Radians::fromDegrees(300.0),
            .expectedBoundingBox = {
                .min = { 0.0, 0.0 },
                .max = { 1.866025404, 2.0 },
            },
        }
    ),
    [](const testing::TestParamInfo<ArcBoundingBoxParams>& paramInfo) {
        return fmt::format(
            "{}_{}",
            Degrees{ paramInfo.param.startAngle }.value(),
            Degrees{ paramInfo.param.endAngle }.value()
        );
    }
);
// clang-format on