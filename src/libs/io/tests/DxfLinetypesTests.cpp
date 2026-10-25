#include "DxfLinetypes.hpp"

#include "domain/Linetype.hpp"

#include <opendxf/tables.hpp>

#include <QStringList>

#include <gtest/gtest.h>

#include <limits>
#include <optional>
#include <string>
#include <utility>

namespace {

odxf::LineType dxfLinetype(std::string name, odxf::LineTypeElements elements)
{
    return odxf::LineType{
        .name = std::move(name),
        .displayName = "A linetype",
        .flags = 0,
        .elements = std::move(elements),
    };
}

const odxf::LineTypeElements dxfDashed{ { .length = 0.5 }, { .length = -0.25 } };

}   // namespace

TEST(DxfLinetypesTest, ToDashPattern)
{
    const std::optional<fcad::domain::DashPattern> pattern{ fcad::io::toDashPattern(
        { { .length = 0.5 }, { .length = -0.25 }, { .length = 0.0 }, { .length = -0.25 } }, 1.0
    ) };

    const fcad::domain::PatternElements expected{
        fcad::domain::Dash{ fcad::domain::PatternLength{ 0.5 } },
        fcad::domain::Gap{ fcad::domain::PatternLength{ 0.25 } },
        fcad::domain::Dot{},
        fcad::domain::Gap{ fcad::domain::PatternLength{ 0.25 } },
    };
    ASSERT_TRUE(pattern.has_value());
    EXPECT_EQ(pattern.value().elements(), expected);
}

TEST(DxfLinetypesTest, ToDashPatternScaled)
{
    const std::optional<fcad::domain::DashPattern> pattern{
        fcad::io::toDashPattern(dxfDashed, 10.0)
    };

    const fcad::domain::PatternElements expected{
        fcad::domain::Dash{ fcad::domain::PatternLength{ 5.0 } },
        fcad::domain::Gap{ fcad::domain::PatternLength{ 2.5 } },
    };
    ASSERT_TRUE(pattern.has_value());
    EXPECT_EQ(pattern.value().elements(), expected);
}

TEST(DxfLinetypesTest, ToDashPatternEmptyIsContinuous)
{
    const std::optional<fcad::domain::DashPattern> pattern{ fcad::io::toDashPattern({}, 1.0) };

    ASSERT_TRUE(pattern.has_value());
    EXPECT_TRUE(pattern.value().isContinuous());
}

TEST(DxfLinetypesTest, ToDashPatternInvalidPattern)
{
    EXPECT_FALSE(fcad::io::toDashPattern({ { .length = -1.0 } }, 1.0).has_value());
    EXPECT_FALSE(
        fcad::io::toDashPattern({ { .length = 0.0 }, { .length = 0.0 } }, 1.0).has_value()
    );
}

TEST(DxfLinetypesTest, ToDashPatternInvalidLength)
{
    EXPECT_FALSE(
        fcad::io::toDashPattern({ { .length = std::numeric_limits<double>::infinity() } }, 1.0)
            .has_value()
    );
    EXPECT_FALSE(
        fcad::io::toDashPattern({ { .length = std::numeric_limits<double>::quiet_NaN() } }, 1.0)
            .has_value()
    );
}

TEST(DxfLinetypesTest, IsComplex)
{
    EXPECT_FALSE(fcad::io::isComplex(dxfLinetype("DASHED", dxfDashed)));
    EXPECT_TRUE(
        fcad::io::isComplex(dxfLinetype(
            "GAS", { { .length = 0.5 }, { .length = -0.2, .complexFlags = 2 }, { .length = -0.25 } }
        ))
    );
}

TEST(DxfLinetypesTest, AddAndFind)
{
    // Arrange
    fcad::io::DxfLinetypes linetypes;

    // Act
    const fcad::io::AddedLinetype added{ linetypes.add(dxfLinetype("DASHED", dxfDashed), 2.0) };

    // Assert
    EXPECT_EQ(added, fcad::io::AddedLinetype::Added);
    const std::optional<fcad::domain::LinetypeId> id{ linetypes.find("dashed") };
    ASSERT_TRUE(id.has_value());

    const fcad::domain::Linetypes taken{ std::move(linetypes).takeLinetypes() };
    ASSERT_EQ(taken.size(), 2);
    const fcad::domain::Linetype& linetype{ taken.at(id.value()) };
    EXPECT_EQ(linetype.name().value(), u"DASHED");
    const fcad::domain::PatternElements expected{
        fcad::domain::Dash{ fcad::domain::PatternLength{ 1.0 } },
        fcad::domain::Gap{ fcad::domain::PatternLength{ 0.5 } },
    };
    EXPECT_EQ(linetype.pattern().elements(), expected);
}

TEST(DxfLinetypesTest, FindContinuousWithoutTheTableEntry)
{
    const fcad::io::DxfLinetypes linetypes;

    EXPECT_EQ(linetypes.find("CONTINUOUS"), fcad::domain::continuousLinetypeId());
    EXPECT_EQ(linetypes.find("Continuous"), fcad::domain::continuousLinetypeId());
    EXPECT_EQ(linetypes.find("DASHED"), std::nullopt);
    EXPECT_EQ(linetypes.find("BYLAYER"), std::nullopt);
    EXPECT_EQ(linetypes.find(""), std::nullopt);
}

TEST(DxfLinetypesTest, AddBuiltIn)
{
    fcad::io::DxfLinetypes linetypes;

    EXPECT_EQ(linetypes.add(dxfLinetype("CONTINUOUS", {}), 1.0), fcad::io::AddedLinetype::BuiltIn);
    EXPECT_EQ(linetypes.add(dxfLinetype("ByLayer", {}), 1.0), fcad::io::AddedLinetype::BuiltIn);
    EXPECT_EQ(linetypes.add(dxfLinetype("BYBLOCK", {}), 1.0), fcad::io::AddedLinetype::BuiltIn);

    EXPECT_EQ(linetypes.find("CONTINUOUS"), fcad::domain::continuousLinetypeId());
    EXPECT_EQ(std::move(linetypes).takeLinetypes().size(), 1);
}

TEST(DxfLinetypesTest, AddComplexMapsToContinuous)
{
    // Arrange
    fcad::io::DxfLinetypes linetypes;

    // Act
    const fcad::io::AddedLinetype added{ linetypes.add(
        dxfLinetype("GAS", { { .length = 0.5 }, { .length = -0.2, .complexFlags = 2 } }), 1.0
    ) };

    // Assert
    EXPECT_EQ(added, fcad::io::AddedLinetype::Complex);
    EXPECT_EQ(linetypes.find("GAS"), fcad::domain::continuousLinetypeId());
    EXPECT_EQ(std::move(linetypes).takeLinetypes().size(), 1);
}

TEST(DxfLinetypesTest, AddInvalid)
{
    // Arrange
    fcad::io::DxfLinetypes linetypes;
    linetypes.add(dxfLinetype("DASHED", dxfDashed), 1.0);

    // Act & Assert
    EXPECT_EQ(
        linetypes.add(dxfLinetype("BAD<NAME>", dxfDashed), 1.0), fcad::io::AddedLinetype::Invalid
    );
    EXPECT_EQ(linetypes.add(dxfLinetype("", dxfDashed), 1.0), fcad::io::AddedLinetype::Invalid);
    EXPECT_EQ(
        linetypes.add(dxfLinetype("dashed", dxfDashed), 1.0), fcad::io::AddedLinetype::Invalid
    );
    EXPECT_EQ(
        linetypes.add(
            dxfLinetype("INFINITE", { { .length = std::numeric_limits<double>::infinity() } }), 1.0
        ),
        fcad::io::AddedLinetype::Invalid
    );

    EXPECT_EQ(
        linetypes.add(dxfLinetype("GAPS", { { .length = -1.0 } }), 1.0),
        fcad::io::AddedLinetype::Invalid
    );

    EXPECT_EQ(linetypes.find("BAD<NAME>"), std::nullopt);
    EXPECT_EQ(linetypes.find("GAPS"), std::nullopt);
    EXPECT_EQ(linetypes.find("INFINITE"), std::nullopt);
    EXPECT_EQ(std::move(linetypes).takeLinetypes().size(), 2);
}

TEST(DxfLinetypesTest, LinetypeWarningsNone)
{
    EXPECT_TRUE(fcad::io::linetypeWarnings({}).isEmpty());
}

TEST(DxfLinetypesTest, LinetypeWarningsSingular)
{
    const QStringList warnings{ fcad::io::linetypeWarnings(
        { .complexLinetypes = 1, .invalidLinetypes = 1, .unknownLayerLinetypes = 1 }
    ) };

    ASSERT_EQ(warnings.size(), 3);
    EXPECT_TRUE(warnings.at(0).startsWith(u"1 linetype contains texts or shapes"));
    EXPECT_TRUE(warnings.at(1).startsWith(u"1 linetype has an invalid or duplicate name"));
    EXPECT_TRUE(warnings.at(2).startsWith(u"1 layer uses an unknown linetype"));
}

TEST(DxfLinetypesTest, LinetypeWarningsPlural)
{
    const QStringList warnings{ fcad::io::linetypeWarnings(
        { .complexLinetypes = 2, .invalidLinetypes = 3, .unknownLayerLinetypes = 4 }
    ) };

    ASSERT_EQ(warnings.size(), 3);
    EXPECT_TRUE(warnings.at(0).startsWith(u"2 linetypes contain texts or shapes"));
    EXPECT_TRUE(warnings.at(1).startsWith(u"3 linetypes have an invalid or duplicate name"));
    EXPECT_TRUE(warnings.at(2).startsWith(u"4 layers use an unknown linetype"));
}
