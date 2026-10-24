#pragma once

#include <QString>

#include <system_error>

namespace fcad::core {

struct Error final
{
    std::error_code code;
    QString message;
};

}   // namespace fcad::core
