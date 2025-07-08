#pragma once

#include <iterator>
#include <vector>

// TODO remove in C++23
template <typename T, typename R>
void appendRange(std::vector<T>& vec, R&& range)
{
    vec.insert(
        vec.end(),
        std::make_move_iterator(std::ranges::begin(range)),
        std::make_move_iterator(std::ranges::end(range))
    );
}
