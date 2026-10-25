#pragma once

#include "Settings.hpp"
#include "domain/LengthUnit.hpp"

#include <QDialog>

#include <gsl/pointers>

#include <optional>

class QCheckBox;
class QComboBox;

namespace fcad::app {

// Asks for the unit of a file that doesn't specify one.
class LengthUnitDialog final : public QDialog
{
public:
    explicit LengthUnitDialog(domain::LengthUnit preselectedUnit, QWidget* parent = nullptr);

    domain::LengthUnit unit() const;
    bool dontAskAgain() const;

private:
    const gsl::not_null<QComboBox*> m_unitBox;
    const gsl::not_null<QCheckBox*> m_dontAskAgainBox;
};

// The unit for a DXF file that doesn't specify one: the fallback unit if the user chose "Don't ask
// again", otherwise the unit chosen in a LengthUnitDialog. Empty if the user cancels.
std::optional<domain::LengthUnit> askForDxfUnit(DxfImportSettings& settings, QWidget* parent);

}   // namespace fcad::app
