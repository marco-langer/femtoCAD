#include "DxfLineweights.hpp"

#include <QStringView>

namespace fcad::io {

domain::LayerLineweight toLayerLineweight(int value)
{
    if (value < 0) {
        return domain::DefaultLineweight{};
    }

    return domain::Lineweight::clamped(value);
}

bool isValidLayerLineweight(int value) { return value >= 0 || value == defaultLineweightValue; }

QStringList lineweightWarnings(const LineweightCounts& counts)
{
    QStringList warnings;

    if (counts.invalidLayers == 1) {
        warnings.push_back(
            QStringLiteral(u"1 layer has an invalid lineweight. It uses the default lineweight.")
        );
    } else if (counts.invalidLayers > 1) {
        warnings.push_back(
            QStringView{ u"%1 layers have an invalid lineweight. They use the default lineweight." }
                .arg(QString::number(counts.invalidLayers))
        );
    }

    return warnings;
}

}   // namespace fcad::io
