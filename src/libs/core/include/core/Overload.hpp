#pragma once

namespace fcad::core {

template <typename... Ts>
struct Overload final : Ts...
{
    using Ts::operator()...;
};

template <typename... Ts>
Overload(Ts...) -> Overload<Ts...>;

}   // namespace fcad::core
