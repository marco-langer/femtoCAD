#include "DxfUnits.hpp"

#include "domain/LengthUnit.hpp"

#include <opendxf/headervariables.hpp>

#include <gtest/gtest.h>

#include <cmath>
#include <optional>
#include <ostream>

TEST(DxfUnitsTest, ToUnitConversionWithoutUnit)
{
    EXPECT_FALSE(fcad::io::toUnitConversion(odxf::LengthUnit::Unitless).has_value());
    EXPECT_FALSE(fcad::io::toUnitConversion(static_cast<odxf::LengthUnit>(-1)).has_value());
    EXPECT_FALSE(fcad::io::toUnitConversion(static_cast<odxf::LengthUnit>(25)).has_value());
}

namespace {

struct UnitConversionParam final
{
    odxf::LengthUnit dxfUnit;
    fcad::domain::LengthUnit expectedUnit;
    double expectedFactor;
};

// Names the test cases in the output by the DXF code.
void PrintTo(const UnitConversionParam& param, std::ostream* os)
{
    *os << "$INSUNITS " << static_cast<int>(param.dxfUnit);
}

constexpr double usSurveyFactor{ 1.000'002'000'004 };

}   // namespace

class DxfUnitConversionTest : public testing::TestWithParam<UnitConversionParam>
{
};

TEST_P(DxfUnitConversionTest, ToUnitConversion)
{
    // Arrange
    const UnitConversionParam& param{ GetParam() };

    // Act
    const std::optional<fcad::io::UnitConversion> conversion{
        fcad::io::toUnitConversion(param.dxfUnit)
    };

    // Assert
    ASSERT_TRUE(conversion.has_value());
    EXPECT_EQ(conversion.value().unit, param.expectedUnit);
    EXPECT_NEAR(
        conversion.value().factor, param.expectedFactor, std::abs(param.expectedFactor) * 1.0e-12
    );
}

INSTANTIATE_TEST_SUITE_P(
    AllDxfUnits,
    DxfUnitConversionTest,
    testing::Values(
        UnitConversionParam{ odxf::LengthUnit::Inches, fcad::domain::LengthUnit::Inch, 1.0 },
        UnitConversionParam{ odxf::LengthUnit::Feet, fcad::domain::LengthUnit::Foot, 1.0 },
        UnitConversionParam{ odxf::LengthUnit::Miles, fcad::domain::LengthUnit::Mile, 1.0 },
        UnitConversionParam{
            odxf::LengthUnit::Millimeters, fcad::domain::LengthUnit::Millimeter, 1.0 },
        UnitConversionParam{
            odxf::LengthUnit::Centimeters, fcad::domain::LengthUnit::Centimeter, 1.0 },
        UnitConversionParam{ odxf::LengthUnit::Meters, fcad::domain::LengthUnit::Meter, 1.0 },
        UnitConversionParam{
            odxf::LengthUnit::Kilometers, fcad::domain::LengthUnit::Kilometer, 1.0 },
        UnitConversionParam{
            odxf::LengthUnit::Microinches, fcad::domain::LengthUnit::Inch, 1.0e-6 },
        UnitConversionParam{ odxf::LengthUnit::Mils, fcad::domain::LengthUnit::Inch, 1.0e-3 },
        UnitConversionParam{ odxf::LengthUnit::Yards, fcad::domain::LengthUnit::Yard, 1.0 },
        UnitConversionParam{
            odxf::LengthUnit::Angstroms, fcad::domain::LengthUnit::Micrometer, 1.0e-4 },
        UnitConversionParam{
            odxf::LengthUnit::Nanometers, fcad::domain::LengthUnit::Micrometer, 1.0e-3 },
        UnitConversionParam{ odxf::LengthUnit::Microns, fcad::domain::LengthUnit::Micrometer, 1.0 },
        UnitConversionParam{
            odxf::LengthUnit::Decimeters, fcad::domain::LengthUnit::Decimeter, 1.0 },
        UnitConversionParam{ odxf::LengthUnit::Decameters, fcad::domain::LengthUnit::Meter, 10.0 },
        UnitConversionParam{
            odxf::LengthUnit::Hectometers, fcad::domain::LengthUnit::Meter, 100.0 },
        UnitConversionParam{
            odxf::LengthUnit::Gigameters, fcad::domain::LengthUnit::Kilometer, 1.0e6 },
        UnitConversionParam{ odxf::LengthUnit::AstronomicalUnits,
                             fcad::domain::LengthUnit::Kilometer,
                             1.495'978'707e8 },
        UnitConversionParam{ odxf::LengthUnit::LightYears,
                             fcad::domain::LengthUnit::Kilometer,
                             9.460'730'472'580'8e12 },
        UnitConversionParam{ odxf::LengthUnit::Parsecs,
                             fcad::domain::LengthUnit::Kilometer,
                             3.085'677'581'491'367e13 },
        UnitConversionParam{
            odxf::LengthUnit::USSurveyFeet, fcad::domain::LengthUnit::Foot, usSurveyFactor },
        UnitConversionParam{
            odxf::LengthUnit::USSurveyInch, fcad::domain::LengthUnit::Inch, usSurveyFactor },
        UnitConversionParam{
            odxf::LengthUnit::USSurveyYard, fcad::domain::LengthUnit::Yard, usSurveyFactor },
        UnitConversionParam{
            odxf::LengthUnit::USSurveyMile, fcad::domain::LengthUnit::Mile, usSurveyFactor }
    )
);
