#include "DxfUnits.hpp"

#include "domain/LengthUnit.hpp"

#include <opendxf/header_variables.hpp>

#include <gtest/gtest.h>

#include <cmath>
#include <optional>
#include <ostream>

TEST(DxfUnitsTest, ToUnitConversionWithoutUnit)
{
    EXPECT_FALSE(fcad::io::toUnitConversion(odxf::length_unit::unitless).has_value());
    EXPECT_FALSE(fcad::io::toUnitConversion(static_cast<odxf::length_unit>(-1)).has_value());
    EXPECT_FALSE(fcad::io::toUnitConversion(static_cast<odxf::length_unit>(25)).has_value());
}

namespace {

struct UnitConversionParam final
{
    odxf::length_unit dxfUnit;
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
        UnitConversionParam{ odxf::length_unit::inches, fcad::domain::LengthUnit::Inch, 1.0 },
        UnitConversionParam{ odxf::length_unit::feet, fcad::domain::LengthUnit::Foot, 1.0 },
        UnitConversionParam{ odxf::length_unit::miles, fcad::domain::LengthUnit::Mile, 1.0 },
        UnitConversionParam{
            odxf::length_unit::millimeters, fcad::domain::LengthUnit::Millimeter, 1.0 },
        UnitConversionParam{
            odxf::length_unit::centimeters, fcad::domain::LengthUnit::Centimeter, 1.0 },
        UnitConversionParam{ odxf::length_unit::meters, fcad::domain::LengthUnit::Meter, 1.0 },
        UnitConversionParam{
            odxf::length_unit::kilometers, fcad::domain::LengthUnit::Kilometer, 1.0 },
        UnitConversionParam{
            odxf::length_unit::microinches, fcad::domain::LengthUnit::Inch, 1.0e-6 },
        UnitConversionParam{ odxf::length_unit::mils, fcad::domain::LengthUnit::Inch, 1.0e-3 },
        UnitConversionParam{ odxf::length_unit::yards, fcad::domain::LengthUnit::Yard, 1.0 },
        UnitConversionParam{
            odxf::length_unit::angstroms, fcad::domain::LengthUnit::Micrometer, 1.0e-4 },
        UnitConversionParam{
            odxf::length_unit::nanometers, fcad::domain::LengthUnit::Micrometer, 1.0e-3 },
        UnitConversionParam{
            odxf::length_unit::microns, fcad::domain::LengthUnit::Micrometer, 1.0 },
        UnitConversionParam{
            odxf::length_unit::decimeters, fcad::domain::LengthUnit::Decimeter, 1.0 },
        UnitConversionParam{ odxf::length_unit::decameters, fcad::domain::LengthUnit::Meter, 10.0 },
        UnitConversionParam{
            odxf::length_unit::hectometers, fcad::domain::LengthUnit::Meter, 100.0 },
        UnitConversionParam{
            odxf::length_unit::gigameters, fcad::domain::LengthUnit::Kilometer, 1.0e6 },
        UnitConversionParam{ odxf::length_unit::astronomical_units,
                             fcad::domain::LengthUnit::Kilometer,
                             1.495'978'707e8 },
        UnitConversionParam{ odxf::length_unit::light_years,
                             fcad::domain::LengthUnit::Kilometer,
                             9.460'730'472'580'8e12 },
        UnitConversionParam{ odxf::length_unit::parsecs,
                             fcad::domain::LengthUnit::Kilometer,
                             3.085'677'581'491'367e13 },
        UnitConversionParam{
            odxf::length_unit::us_survey_feet, fcad::domain::LengthUnit::Foot, usSurveyFactor },
        UnitConversionParam{
            odxf::length_unit::us_survey_inch, fcad::domain::LengthUnit::Inch, usSurveyFactor },
        UnitConversionParam{
            odxf::length_unit::us_survey_yard, fcad::domain::LengthUnit::Yard, usSurveyFactor },
        UnitConversionParam{
            odxf::length_unit::us_survey_mile, fcad::domain::LengthUnit::Mile, usSurveyFactor }
    )
);
