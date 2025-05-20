// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_IOSTREAM_HPP
#define BOOST_INT128_IOSTREAM_HPP

#include <boost/int128/int128.hpp>
#include <boost/int128/detail/mini_from_chars.hpp>
#include <boost/int128/detail/mini_to_chars.hpp>
#include <type_traits>
#include <iostream>

namespace boost {
namespace int128 {

namespace detail {

template <typename T>
struct streamable_overload
{
    static constexpr bool value = std::is_same<T, uint128_t>::value || std::is_same<T, int128_t>::value;
};

template <typename T>
static constexpr bool is_streamable_overload_v = streamable_overload<T>::value;

} // namespace detail

template <typename charT, typename traits, typename LibIntegerType>
auto operator<<(std::basic_ostream<charT, traits>& os, const LibIntegerType& v)
    -> std::enable_if_t<detail::is_streamable_overload_v<LibIntegerType>, std::basic_ostream<charT, traits>&>
{
    char buffer[64U] {};
    const auto last {detail::mini_to_chars(v, buffer)};
    *last = '\0';

    BOOST_INT128_IF_CONSTEXPR (!std::is_same<charT, char>::value)
    {
        charT t_buffer[64U] {};

        auto first {buffer};
        auto t_first {t_buffer};
        while (first != last)
        {
            *t_first++ = static_cast<charT>(*first++);
        }

        os << t_buffer;
    }
    else
    {
        os << buffer;
    }

    return os;
}

} // namespace int128
} // namespace boost

#endif // BOOST_INT128_IOSTREAM_HPP
