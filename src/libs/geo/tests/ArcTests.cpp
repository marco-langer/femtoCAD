#include "geo/Arc.hpp"

#include "test_utils/matchers/BoundingBoxMatcher.hpp"

#include <fmt/core.h>
#include <gtest/gtest.h>

struct ArcBoundingBoxParams final
{
    fcad::core::Radians startAngle;
    fcad::core::Radians endAngle;
    fcad::geo::BoundingBox expectedBoundingBox;
};

struct ArcBoundingBoxFixture : public testing::TestWithParam<ArcBoundingBoxParams>
{
};

TEST_P(ArcBoundingBoxFixture, boundingBox)
{
    // Arrange
    const auto& [startAngle, endAngle, expectedBoundingBox]{ GetParam() };

    const fcad::geo::Arc arc{
        .center = { 1.0, 1.0 },
        .startAngle = startAngle,
        .endAngle = endAngle,
        .radius = fcad::geo::Radius{ 1.0 },
    };

    // Act
    const fcad::geo::BoundingBox arcBoundingBox{ boundingBox(arc) };

    // Assert
    EXPECT_THAT(arcBoundingBox, fcad::test_utils::IsBoundingBox(expectedBoundingBox));
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    ArcBoundingBox,
    ArcBoundingBoxFixture,
    testing::Values(
        ArcBoundingBoxParams{
            .startAngle = fcad::core::Radians::fromDegrees(0.0),
            .endAngle = fcad::core::Radians::fromDegrees(90.0),
            .expectedBoundingBox = {
                .min = { 1.0, 1.0 },
                .max = { 2.0, 2.0 },
            },
        },
        ArcBoundingBoxParams{
            .startAngle = fcad::core::Radians::fromDegrees(0.0),
            .endAngle = fcad::core::Radians::fromDegrees(180.0),
            .expectedBoundingBox = {
                .min = { 0.0, 1.0 },
                .max = { 2.0, 2.0 },
            },
        },
        ArcBoundingBoxParams{
            .startAngle = fcad::core::Radians::fromDegrees(0.0),
            .endAngle = fcad::core::Radians::fromDegrees(270.0),
            .expectedBoundingBox = {
                .min = { 0.0, 0.0 },
                .max = { 2.0, 2.0 },
            },
        },
        ArcBoundingBoxParams{
            .startAngle = fcad::core::Radians::fromDegrees(0.0),
            .endAngle = fcad::core::Radians::fromDegrees(360.0),
            .expectedBoundingBox = {
                .min = { 0.0, 0.0 },
                .max = { 2.0, 2.0 },
            },
        },
        ArcBoundingBoxParams{
            .startAngle = fcad::core::Radians::fromDegrees(0.0),
            .endAngle = fcad::core::Radians::fromDegrees(0.0),
            .expectedBoundingBox = {
                .min = { 2.0, 1.0 },
                .max = { 2.0, 1.0 },
            },
        },
        ArcBoundingBoxParams{
            .startAngle = fcad::core::Radians::fromDegrees(90.0),
            .endAngle = fcad::core::Radians::fromDegrees(180.0),
            .expectedBoundingBox = {
                .min = { 0.0, 1.0 },
                .max = { 1.0, 2.0 },
            },
        },
        ArcBoundingBoxParams{
            .startAngle = fcad::core::Radians::fromDegrees(90.0),
            .endAngle = fcad::core::Radians::fromDegrees(270.0),
            .expectedBoundingBox = {
                .min = { 0.0, 0.0 },
                .max = { 1.0, 2.0 },
            },
        },
        ArcBoundingBoxParams{
            .startAngle = fcad::core::Radians::fromDegrees(90.0),
            .endAngle = fcad::core::Radians::fromDegrees(360.0),
            .expectedBoundingBox = {
                .min = { 0.0, 0.0 },
                .max = { 2.0, 2.0 },
            },
        },
        ArcBoundingBoxParams{
            .startAngle = fcad::core::Radians::fromDegrees(90.0),
            .endAngle = fcad::core::Radians::fromDegrees(90.0),
            .expectedBoundingBox = {
                .min = { 1.0, 2.0 },
                .max = { 1.0, 2.0 },
            },
        },
        ArcBoundingBoxParams{
            .startAngle = fcad::core::Radians::fromDegrees(180.0),
            .endAngle = fcad::core::Radians::fromDegrees(270.0),
            .expectedBoundingBox = {
                .min = { 0.0, 0.0 },
                .max = { 1.0, 1.0 },
            },
        },
        ArcBoundingBoxParams{
            .startAngle = fcad::core::Radians::fromDegrees(180.0),
            .endAngle = fcad::core::Radians::fromDegrees(360.0),
            .expectedBoundingBox = {
                .min = { 0.0, 0.0 },
                .max = { 2.0, 1.0 },
            },
        },
        ArcBoundingBoxParams{
            .startAngle = fcad::core::Radians::fromDegrees(180.0),
            .endAngle = fcad::core::Radians::fromDegrees(90.0),
            .expectedBoundingBox = {
                .min = { 0.0, 0.0 },
                .max = { 2.0, 2.0 },
            },
        },
        ArcBoundingBoxParams{
            .startAngle = fcad::core::Radians::fromDegrees(180.0),
            .endAngle = fcad::core::Radians::fromDegrees(180.0),
            .expectedBoundingBox = {
                .min = { 0.0, 1.0 },
                .max = { 0.0, 1.0 },
            },
        },
        ArcBoundingBoxParams{
            .startAngle = fcad::core::Radians::fromDegrees(270.0),
            .endAngle = fcad::core::Radians::fromDegrees(360.0),
            .expectedBoundingBox = {
                .min = { 1.0, 0.0 },
                .max = { 2.0, 1.0 },
            },
        },
        ArcBoundingBoxParams{
            .startAngle = fcad::core::Radians::fromDegrees(270.0),
            .endAngle = fcad::core::Radians::fromDegrees(90.0),
            .expectedBoundingBox = {
                .min = { 1.0, 0.0 },
                .max = { 2.0, 2.0 },
            },
        },
        ArcBoundingBoxParams{
            .startAngle = fcad::core::Radians::fromDegrees(270.0),
            .endAngle = fcad::core::Radians::fromDegrees(180.0),
            .expectedBoundingBox = {
                .min = { 0.0, 0.0 },
                .max = { 2.0, 2.0 },
            },
        },
        ArcBoundingBoxParams{
            .startAngle = fcad::core::Radians::fromDegrees(270.0),
            .endAngle = fcad::core::Radians::fromDegrees(270.0),
            .expectedBoundingBox = {
                .min = { 1.0, 0.0 },
                .max = { 1.0, 0.0 },
            },
        },
        ArcBoundingBoxParams{
            .startAngle = fcad::core::Radians::fromDegrees(30.0),
            .endAngle = fcad::core::Radians::fromDegrees(60.0),
            .expectedBoundingBox = {
                .min = { 1.5, 1.5 },
                .max = { 1.866025404, 1.866025404 },
            },
        },
        ArcBoundingBoxParams{
            .startAngle = fcad::core::Radians::fromDegrees(30.0),
            .endAngle = fcad::core::Radians::fromDegrees(120.0),
            .expectedBoundingBox = {
                .min = { 0.5, 1.5 },
                .max = { 1.866025404, 2.0 },
            },
        },
        ArcBoundingBoxParams{
            .startAngle = fcad::core::Radians::fromDegrees(60.0),
            .endAngle = fcad::core::Radians::fromDegrees(150.0),
            .expectedBoundingBox = {
                .min = { 1.0 - 0.866025404, 1.5 },
                .max = { 1.5, 2.0 },
            },
        },
        ArcBoundingBoxParams{
            .startAngle = fcad::core::Radians::fromDegrees(60.0),
            .endAngle = fcad::core::Radians::fromDegrees(210.0),
            .expectedBoundingBox = {
                .min = { 0.0, 0.5 },
                .max = { 1.5, 2.0 },
            },
        },
        ArcBoundingBoxParams{
            .startAngle = fcad::core::Radians::fromDegrees(60.0),
            .endAngle = fcad::core::Radians::fromDegrees(330.0),
            .expectedBoundingBox = {
                .min = { 0.0, 0.0 },
                .max = { 1.866025404, 2.0 },
            },
        },
        ArcBoundingBoxParams{
            .startAngle = fcad::core::Radians::fromDegrees(30.0),
            .endAngle = fcad::core::Radians::fromDegrees(300.0),
            .expectedBoundingBox = {
                .min = { 0.0, 0.0 },
                .max = { 1.866025404, 2.0 },
            },
        }
    ),
    [](const testing::TestParamInfo<ArcBoundingBoxParams>& paramInfo) {
        return fmt::format(
            "{}_{}",
            fcad::core::Degrees{ paramInfo.param.startAngle }.value(),
            fcad::core::Degrees{ paramInfo.param.endAngle }.value()
        );
    }
);
// clang-format on