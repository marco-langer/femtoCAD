#include "domain/Linetype.hpp"

#include <QString>

#include <gtest/gtest.h>

#include <optional>
#include <type_traits>

namespace {

fcad::domain::LinetypeName nameOf(const char16_t* name)
{
    return fcad::domain::LinetypeName::fromValue(QString::fromUtf16(name)).value();
}

const fcad::domain::PatternElements dashDotElements{
    fcad::domain::Dash{ fcad::domain::PatternLength{ 0.5 } },
    fcad::domain::Gap{ fcad::domain::PatternLength{ 0.25 } },
    fcad::domain::Dot{},
    fcad::domain::Gap{ fcad::domain::PatternLength{ 0.25 } },
};

const fcad::domain::DashPattern dashDot{
    fcad::domain::DashPattern::fromElements(dashDotElements).value()
};

}   // namespace

TEST(LinetypeNameTest, fromValueKeepsTheSpelling)
{
    const std::optional<fcad::domain::LinetypeName> name{
        fcad::domain::LinetypeName::fromValue(QStringLiteral(u"Dash Dot"))
    };

    ASSERT_TRUE(name.has_value());
    EXPECT_EQ(name.value().value(), u"Dash Dot");
}

TEST(LinetypeNameTest, fromValueRejectsEmptyAndSurroundingSpaces)
{
    EXPECT_FALSE(fcad::domain::LinetypeName::fromValue(QString{}).has_value());
    EXPECT_FALSE(fcad::domain::LinetypeName::fromValue(QStringLiteral(u"   ")).has_value());
    EXPECT_FALSE(fcad::domain::LinetypeName::fromValue(QStringLiteral(u" Dashed")).has_value());
    EXPECT_FALSE(fcad::domain::LinetypeName::fromValue(QStringLiteral(u"Dashed ")).has_value());
}

TEST(LinetypeNameTest, fromValueRejectsReferenceNames)
{
    EXPECT_FALSE(fcad::domain::LinetypeName::fromValue(QStringLiteral(u"ByLayer")).has_value());
    EXPECT_FALSE(fcad::domain::LinetypeName::fromValue(QStringLiteral(u"BYLAYER")).has_value());
    EXPECT_FALSE(fcad::domain::LinetypeName::fromValue(QStringLiteral(u"ByBlock")).has_value());
    EXPECT_FALSE(fcad::domain::LinetypeName::fromValue(QStringLiteral(u"byblock")).has_value());

    EXPECT_TRUE(fcad::domain::LinetypeName::fromValue(QStringLiteral(u"ByLayer2")).has_value());
    EXPECT_TRUE(fcad::domain::LinetypeName::fromValue(QStringLiteral(u"Continuous")).has_value());
}

TEST(LinetypeNameTest, fromValueRejectsForbiddenCharacters)
{
    for (const QChar character : QStringLiteral(u"<>/\\\":;?*|,=`")) {
        const QString name{ QStringLiteral(u"Dash") + character + QStringLiteral(u"Dot") };
        EXPECT_FALSE(fcad::domain::LinetypeName::fromValue(name).has_value()) << name.toStdString();
    }
}

TEST(LinetypeNameTest, fromValueLimitsTheLength)
{
    const qsizetype maximumLength{ fcad::domain::LinetypeName::maximumLength };

    EXPECT_TRUE(fcad::domain::LinetypeName::fromValue(QString{ maximumLength, u'a' }).has_value());
    EXPECT_FALSE(
        fcad::domain::LinetypeName::fromValue(QString{ maximumLength + 1, u'a' }).has_value()
    );
}

TEST(LinetypeNameTest, comparesWithoutRegardToCase)
{
    EXPECT_EQ(nameOf(u"Dashed"), nameOf(u"DASHED"));
    EXPECT_NE(nameOf(u"Dashed"), nameOf(u"Dashed2"));
}

TEST(DashPatternTest, continuousHasNoElements)
{
    const fcad::domain::DashPattern pattern{ fcad::domain::DashPattern::continuous() };

    EXPECT_TRUE(pattern.isContinuous());
    EXPECT_TRUE(pattern.elements().empty());
}

TEST(DashPatternTest, fromElements)
{
    const std::optional<fcad::domain::DashPattern> pattern{
        fcad::domain::DashPattern::fromElements(dashDotElements)
    };

    ASSERT_TRUE(pattern.has_value());
    EXPECT_FALSE(pattern.value().isContinuous());
    EXPECT_EQ(pattern.value().elements(), dashDotElements);
}

TEST(DashPatternTest, fromElementsWithoutElementsIsContinuous)
{
    const std::optional<fcad::domain::DashPattern> pattern{
        fcad::domain::DashPattern::fromElements({})
    };

    ASSERT_TRUE(pattern.has_value());
    EXPECT_TRUE(pattern.value().isContinuous());
}

TEST(DashPatternTest, fromElementsAcceptsDotsWithGaps)
{
    EXPECT_TRUE(
        fcad::domain::DashPattern::fromElements(
            { fcad::domain::Dot{}, fcad::domain::Gap{ fcad::domain::PatternLength{ 1.0 } } }
        ).has_value()
    );
}

TEST(DashPatternTest, fromElementsRejectsOnlyGaps)
{
    EXPECT_FALSE(
        fcad::domain::DashPattern::fromElements(
            { fcad::domain::Gap{ fcad::domain::PatternLength{ 1.0 } } }
        ).has_value()
    );
}

TEST(DashPatternTest, fromElementsRejectsOnlyDots)
{
    EXPECT_FALSE(
        fcad::domain::DashPattern::fromElements(
            { fcad::domain::Dot{}, fcad::domain::Dot{} }
        ).has_value()
    );
}

TEST(LinetypeTest, isMoveOnly)
{
    EXPECT_FALSE(std::is_copy_constructible_v<fcad::domain::Linetype>);
    EXPECT_FALSE(std::is_copy_assignable_v<fcad::domain::Linetype>);
    EXPECT_TRUE(std::is_nothrow_move_constructible_v<fcad::domain::Linetype>);
}

TEST(LinetypeTest, newLinetypesHaveDifferentIds)
{
    const fcad::domain::Linetype first{ fcad::domain::LinetypeDefinition{ nameOf(u"Dashed"),
                                                                          dashDot } };
    const fcad::domain::Linetype second{ fcad::domain::LinetypeDefinition{ nameOf(u"Dashed"),
                                                                           dashDot } };

    EXPECT_NE(first.id(), second.id());
}

TEST(LinetypeTest, keepsItsDefinition)
{
    const fcad::domain::Linetype linetype{ fcad::domain::LinetypeDefinition{
        .name = nameOf(u"Dash dot"), .pattern = dashDot } };

    EXPECT_EQ(linetype.name().value(), u"Dash dot");
    EXPECT_EQ(linetype.pattern().elements(), dashDotElements);
}

TEST(LinetypeTest, continuousIsBuiltIn)
{
    const fcad::domain::Linetype continuous{ fcad::domain::Linetype::continuous() };

    EXPECT_EQ(continuous.id(), fcad::domain::continuousLinetypeId());
    EXPECT_EQ(continuous.id(), fcad::domain::Linetype::continuous().id());
    EXPECT_EQ(continuous.name(), nameOf(u"Continuous"));
    EXPECT_TRUE(continuous.pattern().isContinuous());
}
