#pragma once

#include <QUuid>

namespace fcad::core {

template <typename Tag>
class StrongUuid final
{
public:
    explicit StrongUuid(QUuid uuid) noexcept
        : m_uuid{ uuid }
    {}

    // A new random (version 4) UUID.
    [[nodiscard]] static StrongUuid generate() { return StrongUuid{ QUuid::createUuid() }; }

    bool operator==(const StrongUuid&) const = default;

    QUuid value() const noexcept { return m_uuid; }

private:
    QUuid m_uuid;
};

}   // namespace fcad::core
