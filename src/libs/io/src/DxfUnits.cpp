#include "DxfUnits.hpp"

#include <numbers>

namespace fcad::io {

namespace {

    constexpr double kilometersPerAstronomicalUnit{ 149'597'870.7 };
    // US survey units are 1200/3937 m per foot instead of 0.3048 m.
    constexpr double usSurveyToInternational{ 1200.0 / 3937.0 / 0.3048 };

}   // namespace

QString toName(odxf::length_unit dxfUnit)
{
    switch (dxfUnit) {
    case odxf::length_unit::unitless: return QStringLiteral(u"unitless");
    case odxf::length_unit::inches: return QStringLiteral(u"inches");
    case odxf::length_unit::feet: return QStringLiteral(u"feet");
    case odxf::length_unit::miles: return QStringLiteral(u"miles");
    case odxf::length_unit::millimeters: return QStringLiteral(u"millimeters");
    case odxf::length_unit::centimeters: return QStringLiteral(u"centimeters");
    case odxf::length_unit::meters: return QStringLiteral(u"meters");
    case odxf::length_unit::kilometers: return QStringLiteral(u"kilometers");
    case odxf::length_unit::microinches: return QStringLiteral(u"microinches");
    case odxf::length_unit::mils: return QStringLiteral(u"mils");
    case odxf::length_unit::yards: return QStringLiteral(u"yards");
    case odxf::length_unit::angstroms: return QStringLiteral(u"angstroms");
    case odxf::length_unit::nanometers: return QStringLiteral(u"nanometers");
    case odxf::length_unit::microns: return QStringLiteral(u"micrometers");
    case odxf::length_unit::decimeters: return QStringLiteral(u"decimeters");
    case odxf::length_unit::decameters: return QStringLiteral(u"decameters");
    case odxf::length_unit::hectometers: return QStringLiteral(u"hectometers");
    case odxf::length_unit::gigameters: return QStringLiteral(u"gigameters");
    case odxf::length_unit::astronomical_units: return QStringLiteral(u"astronomical units");
    case odxf::length_unit::light_years: return QStringLiteral(u"light years");
    case odxf::length_unit::parsecs: return QStringLiteral(u"parsecs");
    case odxf::length_unit::us_survey_feet: return QStringLiteral(u"US survey feet");
    case odxf::length_unit::us_survey_inch: return QStringLiteral(u"US survey inches");
    case odxf::length_unit::us_survey_yard: return QStringLiteral(u"US survey yards");
    case odxf::length_unit::us_survey_mile: return QStringLiteral(u"US survey miles");
    }

    return QStringLiteral(u"unknown units");
}

QString toName(domain::LengthUnit unit)
{
    switch (unit) {
    case domain::LengthUnit::Micrometer: return QStringLiteral(u"micrometers");
    case domain::LengthUnit::Millimeter: return QStringLiteral(u"millimeters");
    case domain::LengthUnit::Centimeter: return QStringLiteral(u"centimeters");
    case domain::LengthUnit::Decimeter: return QStringLiteral(u"decimeters");
    case domain::LengthUnit::Meter: return QStringLiteral(u"meters");
    case domain::LengthUnit::Kilometer: return QStringLiteral(u"kilometers");
    case domain::LengthUnit::Inch: return QStringLiteral(u"inches");
    case domain::LengthUnit::Foot: return QStringLiteral(u"feet");
    case domain::LengthUnit::Yard: return QStringLiteral(u"yards");
    case domain::LengthUnit::Mile: return QStringLiteral(u"miles");
    }

    return QStringLiteral(u"unknown units");
}

std::optional<UnitConversion> toUnitConversion(odxf::length_unit dxfUnit)
{
    using domain::LengthUnit;

    switch (dxfUnit) {
    case odxf::length_unit::unitless: return std::nullopt;
    case odxf::length_unit::inches: return UnitConversion{ .unit = LengthUnit::Inch };
    case odxf::length_unit::feet: return UnitConversion{ .unit = LengthUnit::Foot };
    case odxf::length_unit::miles: return UnitConversion{ .unit = LengthUnit::Mile };
    case odxf::length_unit::millimeters: return UnitConversion{ .unit = LengthUnit::Millimeter };
    case odxf::length_unit::centimeters: return UnitConversion{ .unit = LengthUnit::Centimeter };
    case odxf::length_unit::meters: return UnitConversion{ .unit = LengthUnit::Meter };
    case odxf::length_unit::kilometers: return UnitConversion{ .unit = LengthUnit::Kilometer };
    case odxf::length_unit::microinches:
        return UnitConversion{ .unit = LengthUnit::Inch, .factor = 1e-6 };
    case odxf::length_unit::mils: return UnitConversion{ .unit = LengthUnit::Inch, .factor = 1e-3 };
    case odxf::length_unit::yards: return UnitConversion{ .unit = LengthUnit::Yard };
    case odxf::length_unit::angstroms:
        return UnitConversion{ .unit = LengthUnit::Micrometer, .factor = 1e-4 };
    case odxf::length_unit::nanometers:
        return UnitConversion{ .unit = LengthUnit::Micrometer, .factor = 1e-3 };
    case odxf::length_unit::microns: return UnitConversion{ .unit = LengthUnit::Micrometer };
    case odxf::length_unit::decimeters: return UnitConversion{ .unit = LengthUnit::Decimeter };
    case odxf::length_unit::decameters:
        return UnitConversion{ .unit = LengthUnit::Meter, .factor = 10.0 };
    case odxf::length_unit::hectometers:
        return UnitConversion{ .unit = LengthUnit::Meter, .factor = 100.0 };
    case odxf::length_unit::gigameters:
        return UnitConversion{ .unit = LengthUnit::Kilometer, .factor = 1e6 };
    case odxf::length_unit::astronomical_units:
        return UnitConversion{ .unit = LengthUnit::Kilometer,
                               .factor = kilometersPerAstronomicalUnit };
    case odxf::length_unit::light_years:
        // The IAU light year: 9'460'730'472'580'800 m.
        return UnitConversion{ .unit = LengthUnit::Kilometer, .factor = 9'460'730'472'580.8 };
    case odxf::length_unit::parsecs:
        // 648000 / pi astronomical units.
        return UnitConversion{ .unit = LengthUnit::Kilometer,
                               .factor =
                                   648'000.0 / std::numbers::pi * kilometersPerAstronomicalUnit };
    case odxf::length_unit::us_survey_feet:
        return UnitConversion{ .unit = LengthUnit::Foot, .factor = usSurveyToInternational };
    case odxf::length_unit::us_survey_inch:
        return UnitConversion{ .unit = LengthUnit::Inch, .factor = usSurveyToInternational };
    case odxf::length_unit::us_survey_yard:
        return UnitConversion{ .unit = LengthUnit::Yard, .factor = usSurveyToInternational };
    case odxf::length_unit::us_survey_mile:
        return UnitConversion{ .unit = LengthUnit::Mile, .factor = usSurveyToInternational };
    }

    // Treat codes outside the known range like a missing unit.
    return std::nullopt;
}

}   // namespace fcad::io
