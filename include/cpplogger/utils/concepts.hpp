#pragma once

#include <iostream>

namespace cpplogger
{

template <typename T>
concept Printable = requires(T t)
{
    {
        std::cout << t
        } -> std::same_as<std::ostream &>;
};

template <typename T>
concept HasIterator = std::ranges::range<T> && requires(const T a)
{
    {
        a.size()
        } -> std::convertible_to<std::size_t>;
};

template <typename T>
concept PrintableIterator =
    HasIterator<T> && !Printable<T> && Printable<typename std::iterator_traits<typename T::const_iterator>::value_type>;

}    // namespace cpplogger