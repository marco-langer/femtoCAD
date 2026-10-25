#include "DxfLineweights.hpp"

#include "domain/Lineweight.hpp"

#include <QStringList>

#include <gtest/gtest.h>

#include <limits>

namespace {

const fcad::domain::LayerLineweight defaultLayerLineweight{ fcad::domain::DefaultLineweight{} };

}   // namespace

TEST(DxfLineweightsTest, ToLayerLineweight)
{
    EXPECT_EQ(
        fcad::io::toLayerLineweight(0),
        fcad::domain::LayerLineweight{ fcad::domain::Lineweight{ 0 } }
    );
    EXPECT_EQ(
        fcad::io::toLayerLineweight(27),
        fcad::domain::LayerLineweight{ fcad::domain::Lineweight{ 27 } }
    );
    EXPECT_EQ(
        fcad::io::toLayerLineweight(211),
        fcad::domain::LayerLineweight{ fcad::domain::Lineweight{ 211 } }
    );
    EXPECT_EQ(fcad::io::toLayerLineweight(-3), defaultLayerLineweight);
}

TEST(DxfLineweightsTest, ToLayerLineweightClampsLargeValues)
{
    const fcad::domain::LayerLineweight largest{ fcad::domain::Lineweight{ 211 } };

    EXPECT_EQ(fcad::io::toLayerLineweight(212), largest);
    EXPECT_EQ(fcad::io::toLayerLineweight(std::numeric_limits<int>::max()), largest);
}

TEST(DxfLineweightsTest, ToLayerLineweightInvalidIsDefault)
{
    EXPECT_EQ(fcad::io::toLayerLineweight(-1), defaultLayerLineweight);
    EXPECT_EQ(fcad::io::toLayerLineweight(-2), defaultLayerLineweight);
    EXPECT_EQ(fcad::io::toLayerLineweight(-4), defaultLayerLineweight);
    EXPECT_EQ(fcad::io::toLayerLineweight(std::numeric_limits<int>::min()), defaultLayerLineweight);
}

TEST(DxfLineweightsTest, IsValidLayerLineweight)
{
    EXPECT_TRUE(fcad::io::isValidLayerLineweight(0));
    EXPECT_TRUE(fcad::io::isValidLayerLineweight(300));
    EXPECT_TRUE(fcad::io::isValidLayerLineweight(-3));

    EXPECT_FALSE(fcad::io::isValidLayerLineweight(-1));
    EXPECT_FALSE(fcad::io::isValidLayerLineweight(-2));
    EXPECT_FALSE(fcad::io::isValidLayerLineweight(-4));
}

TEST(DxfLineweightsTest, LineweightWarningsNone)
{
    EXPECT_TRUE(fcad::io::lineweightWarnings({}).isEmpty());
}

TEST(DxfLineweightsTest, LineweightWarningsSingular)
{
    const QStringList warnings{ fcad::io::lineweightWarnings({ .invalidLayers = 1 }) };

    ASSERT_EQ(warnings.size(), 1);
    EXPECT_TRUE(warnings.at(0).startsWith(u"1 layer has an invalid lineweight"));
}

TEST(DxfLineweightsTest, LineweightWarningsPlural)
{
    const QStringList warnings{ fcad::io::lineweightWarnings({ .invalidLayers = 2 }) };

    ASSERT_EQ(warnings.size(), 1);
    EXPECT_TRUE(warnings.at(0).startsWith(u"2 layers have an invalid lineweight"));
}
