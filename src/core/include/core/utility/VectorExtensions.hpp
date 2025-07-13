#pragma once

#include <algorithm>
#include <iterator>
#include <optional>
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

template <typename T>
std::optional<std::size_t> indexOf(const std::vector<T>& vec, const T& element)
{
    const auto pos{ std::ranges::find(vec, element) };

    return pos == vec.cend() ? std::optional<std::size_t>{}
                             : std::ranges::distance(vec.cbegin(), pos);
}