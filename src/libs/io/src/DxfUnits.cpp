#include "DxfUnits.hpp"

#include <numbers>

namespace fcad::io {

namespace {

    constexpr double kilometersPerAstronomicalUnit{ 149'597'870.7 };
    // US survey units are 1200/3937 m per foot instead of 0.3048 m.
    constexpr double usSurveyToInternational{ 1200.0 / 3937.0 / 0.3048 };

}   // namespace

QString toName(odxf::LengthUnit dxfUnit)
{
    switch (dxfUnit) {
    case odxf::LengthUnit::Unitless: return QStringLiteral(u"unitless");
    case odxf::LengthUnit::Inches: return QStringLiteral(u"inches");
    case odxf::LengthUnit::Feet: return QStringLiteral(u"feet");
    case odxf::LengthUnit::Miles: return QStringLiteral(u"miles");
    case odxf::LengthUnit::Millimeters: return QStringLiteral(u"millimeters");
    case odxf::LengthUnit::Centimeters: return QStringLiteral(u"centimeters");
    case odxf::LengthUnit::Meters: return QStringLiteral(u"meters");
    case odxf::LengthUnit::Kilometers: return QStringLiteral(u"kilometers");
    case odxf::LengthUnit::Microinches: return QStringLiteral(u"microinches");
    case odxf::LengthUnit::Mils: return QStringLiteral(u"mils");
    case odxf::LengthUnit::Yards: return QStringLiteral(u"yards");
    case odxf::LengthUnit::Angstroms: return QStringLiteral(u"angstroms");
    case odxf::LengthUnit::Nanometers: return QStringLiteral(u"nanometers");
    case odxf::LengthUnit::Microns: return QStringLiteral(u"micrometers");
    case odxf::LengthUnit::Decimeters: return QStringLiteral(u"decimeters");
    case odxf::LengthUnit::Decameters: return QStringLiteral(u"decameters");
    case odxf::LengthUnit::Hectometers: return QStringLiteral(u"hectometers");
    case odxf::LengthUnit::Gigameters: return QStringLiteral(u"gigameters");
    case odxf::LengthUnit::AstronomicalUnits: return QStringLiteral(u"astronomical units");
    case odxf::LengthUnit::LightYears: return QStringLiteral(u"light years");
    case odxf::LengthUnit::Parsecs: return QStringLiteral(u"parsecs");
    case odxf::LengthUnit::USSurveyFeet: return QStringLiteral(u"US survey feet");
    case odxf::LengthUnit::USSurveyInch: return QStringLiteral(u"US survey inches");
    case odxf::LengthUnit::USSurveyYard: return QStringLiteral(u"US survey yards");
    case odxf::LengthUnit::USSurveyMile: return QStringLiteral(u"US survey miles");
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

std::optional<UnitConversion> toUnitConversion(odxf::LengthUnit dxfUnit)
{
    using domain::LengthUnit;

    switch (dxfUnit) {
    case odxf::LengthUnit::Unitless: return std::nullopt;
    case odxf::LengthUnit::Inches: return UnitConversion{ .unit = LengthUnit::Inch };
    case odxf::LengthUnit::Feet: return UnitConversion{ .unit = LengthUnit::Foot };
    case odxf::LengthUnit::Miles: return UnitConversion{ .unit = LengthUnit::Mile };
    case odxf::LengthUnit::Millimeters: return UnitConversion{ .unit = LengthUnit::Millimeter };
    case odxf::LengthUnit::Centimeters: return UnitConversion{ .unit = LengthUnit::Centimeter };
    case odxf::LengthUnit::Meters: return UnitConversion{ .unit = LengthUnit::Meter };
    case odxf::LengthUnit::Kilometers: return UnitConversion{ .unit = LengthUnit::Kilometer };
    case odxf::LengthUnit::Microinches:
        return UnitConversion{ .unit = LengthUnit::Inch, .factor = 1e-6 };
    case odxf::LengthUnit::Mils: return UnitConversion{ .unit = LengthUnit::Inch, .factor = 1e-3 };
    case odxf::LengthUnit::Yards: return UnitConversion{ .unit = LengthUnit::Yard };
    case odxf::LengthUnit::Angstroms:
        return UnitConversion{ .unit = LengthUnit::Micrometer, .factor = 1e-4 };
    case odxf::LengthUnit::Nanometers:
        return UnitConversion{ .unit = LengthUnit::Micrometer, .factor = 1e-3 };
    case odxf::LengthUnit::Microns: return UnitConversion{ .unit = LengthUnit::Micrometer };
    case odxf::LengthUnit::Decimeters: return UnitConversion{ .unit = LengthUnit::Decimeter };
    case odxf::LengthUnit::Decameters:
        return UnitConversion{ .unit = LengthUnit::Meter, .factor = 10.0 };
    case odxf::LengthUnit::Hectometers:
        return UnitConversion{ .unit = LengthUnit::Meter, .factor = 100.0 };
    case odxf::LengthUnit::Gigameters:
        return UnitConversion{ .unit = LengthUnit::Kilometer, .factor = 1e6 };
    case odxf::LengthUnit::AstronomicalUnits:
        return UnitConversion{ .unit = LengthUnit::Kilometer,
                               .factor = kilometersPerAstronomicalUnit };
    case odxf::LengthUnit::LightYears:
        // The IAU light year: 9'460'730'472'580'800 m.
        return UnitConversion{ .unit = LengthUnit::Kilometer, .factor = 9'460'730'472'580.8 };
    case odxf::LengthUnit::Parsecs:
        // 648000 / pi astronomical units.
        return UnitConversion{ .unit = LengthUnit::Kilometer,
                               .factor =
                                   648'000.0 / std::numbers::pi * kilometersPerAstronomicalUnit };
    case odxf::LengthUnit::USSurveyFeet:
        return UnitConversion{ .unit = LengthUnit::Foot, .factor = usSurveyToInternational };
    case odxf::LengthUnit::USSurveyInch:
        return UnitConversion{ .unit = LengthUnit::Inch, .factor = usSurveyToInternational };
    case odxf::LengthUnit::USSurveyYard:
        return UnitConversion{ .unit = LengthUnit::Yard, .factor = usSurveyToInternational };
    case odxf::LengthUnit::USSurveyMile:
        return UnitConversion{ .unit = LengthUnit::Mile, .factor = usSurveyToInternational };
    }

    // Treat codes outside the known range like a missing unit.
    return std::nullopt;
}

}   // namespace fcad::io
