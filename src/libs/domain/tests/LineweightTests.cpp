#include "domain/Lineweight.hpp"

#include <gtest/gtest.h>

namespace {

const fcad::domain::LayerLineweight thin{ fcad::domain::Lineweight{ 13 } };
const fcad::domain::LayerLineweight thick{ fcad::domain::Lineweight{ 50 } };
const fcad::domain::LayerLineweight defaultLineweight{ fcad::domain::DefaultLineweight{} };

}   // namespace

TEST(LineweightTest, resolvedByLayerIsTheLayerLineweight)
{
    EXPECT_EQ(fcad::domain::resolved(fcad::domain::ByLayer{}, thin), thin);
    EXPECT_EQ(
        fcad::domain::resolved(fcad::domain::ByLayer{}, defaultLineweight), defaultLineweight
    );
}

TEST(LineweightTest, resolvedOwnLineweightIgnoresTheLayerLineweight)
{
    EXPECT_EQ(fcad::domain::resolved(fcad::domain::Lineweight{ 50 }, thin), thick);
    EXPECT_EQ(fcad::domain::resolved(fcad::domain::DefaultLineweight{}, thin), defaultLineweight);
}
