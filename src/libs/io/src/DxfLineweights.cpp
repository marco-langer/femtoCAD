#include "DxfLineweights.hpp"

#include <QStringView>

#include <variant>

namespace fcad::io {

domain::LayerLineweight toLayerLineweight(int value)
{
    if (value < 0) {
        return domain::DefaultLineweight{};
    }

    return domain::Lineweight::clamped(value);
}

bool isValidLayerLineweight(int value) { return value >= 0 || value == defaultLineweightValue; }

domain::EntityLineweight toEntityLineweight(int value)
{
    if (value == byLayerLineweightValue) {
        return domain::ByLayer{};
    }

    return std::visit(
        [](const auto& lineweight) { return domain::EntityLineweight{ lineweight }; },
        toLayerLineweight(value)
    );
}

bool isValidEntityLineweight(int value)
{
    return value >= 0 || value == byLayerLineweightValue || value == byBlockLineweightValue
        || value == defaultLineweightValue;
}

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

    if (counts.byBlockEntities == 1) {
        warnings.push_back(QStringLiteral(
            u"1 entity uses the lineweight ByBlock, which femtoCAD doesn't support yet. It uses "
            u"the default lineweight."
        ));
    } else if (counts.byBlockEntities > 1) {
        warnings.push_back(
            QStringView{ u"%1 entities use the lineweight ByBlock, which femtoCAD doesn't support "
                         u"yet. They use the default lineweight." }
                .arg(QString::number(counts.byBlockEntities))
        );
    }

    if (counts.invalidEntities == 1) {
        warnings.push_back(
            QStringLiteral(u"1 entity has an invalid lineweight. It uses the default lineweight.")
        );
    } else if (counts.invalidEntities > 1) {
        warnings.push_back(
            QStringView{
                u"%1 entities have an invalid lineweight. They use the default lineweight." }
                .arg(QString::number(counts.invalidEntities))
        );
    }

    return warnings;
}

}   // namespace fcad::io
