#include "features/io/dxf/LengthUnitDialog.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QVBoxLayout>

#include <gsl/narrow>

#include <algorithm>

namespace fcad::app {

namespace {

    // The unit's name for the UI, e.g. "Millimeter (mm)".
    QString displayName(domain::LengthUnit unit)
    {
        switch (unit) {
        case domain::LengthUnit::Micrometer: return QStringLiteral(u"Micrometer (µm)");
        case domain::LengthUnit::Millimeter: return QStringLiteral(u"Millimeter (mm)");
        case domain::LengthUnit::Centimeter: return QStringLiteral(u"Centimeter (cm)");
        case domain::LengthUnit::Decimeter: return QStringLiteral(u"Decimeter (dm)");
        case domain::LengthUnit::Meter: return QStringLiteral(u"Meter (m)");
        case domain::LengthUnit::Kilometer: return QStringLiteral(u"Kilometer (km)");
        case domain::LengthUnit::Inch: return QStringLiteral(u"Inch (in)");
        case domain::LengthUnit::Foot: return QStringLiteral(u"Foot (ft)");
        case domain::LengthUnit::Yard: return QStringLiteral(u"Yard (yd)");
        case domain::LengthUnit::Mile: return QStringLiteral(u"Mile (mi)");
        }

        return QString{};
    }

}   // namespace

LengthUnitDialog::LengthUnitDialog(domain::LengthUnit preselectedUnit, QWidget* parent)
    : QDialog{ parent }
    , m_unitBox{ new QComboBox{} }
    , m_dontAskAgainBox{ new QCheckBox{ "Don't ask again" } }
{
    setWindowTitle("Length unit");

    // The items follow domain::lengthUnits, so an item's index is the unit's index there.
    for (const domain::LengthUnit unit : domain::lengthUnits) {
        m_unitBox->addItem(displayName(unit));
    }
    const auto preselected{ std::ranges::find(domain::lengthUnits, preselectedUnit) };
    m_unitBox->setCurrentIndex(gsl::narrow_cast<int>(preselected - domain::lengthUnits.begin()));

    auto* buttons{ new QDialogButtonBox{ QDialogButtonBox::Ok | QDialogButtonBox::Cancel } };
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto* layout{ new QVBoxLayout{ this } };
    layout->addWidget(new QLabel{ "The file doesn't specify a length unit. Which unit is it in?" });
    layout->addWidget(m_unitBox);
    layout->addWidget(m_dontAskAgainBox);
    layout->addWidget(buttons);
}

domain::LengthUnit LengthUnitDialog::unit() const
{
    return domain::lengthUnits.at(gsl::narrow_cast<std::size_t>(m_unitBox->currentIndex()));
}

bool LengthUnitDialog::dontAskAgain() const { return m_dontAskAgainBox->isChecked(); }

std::optional<domain::LengthUnit> askForDxfUnit(DxfImportSettings& settings, QWidget* parent)
{
    if (settings.useFallbackUnit) {
        return settings.fallbackUnit;
    }

    LengthUnitDialog dialog{ settings.fallbackUnit, parent };
    if (dialog.exec() != QDialog::Accepted) {
        return std::nullopt;
    }

    if (dialog.dontAskAgain()) {
        settings.fallbackUnit = dialog.unit();
        settings.useFallbackUnit = true;
    }

    return dialog.unit();
}

}   // namespace fcad::app
