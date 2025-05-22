// Copyright 2022 Peter Dimov
// Copyright 2023 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_DETAIL_MINI_TO_CHARS_HPP
#define BOOST_INT128_DETAIL_MINI_TO_CHARS_HPP

#include <boost/int128/detail/uint128_imp.hpp>
#include <boost/int128/detail/int128_imp.hpp>

namespace boost {
namespace int128 {
namespace detail {

constexpr char* mini_to_chars(char (&buffer)[64], uint128_t v) noexcept
{
    char* p {buffer + 64};
    *--p = '\0';

    do
    {
        *--p = "0123456789"[static_cast<std::size_t>(v % 10U)];
        v /= 10U;
    } while (v != 0U);

    return p;
}

constexpr char* mini_to_chars(char (&buffer)[64], const int128_t v) noexcept
{
    char* p {nullptr};

    if (v < 0)
    {
        // We cant negate the min value inside the signed type, but we know what the result will be
        if (v == std::numeric_limits<int128_t>::min())
        {
            p = mini_to_chars(buffer, uint128_t{UINT64_C(0x8000000000000000), 0});
        }
        else
        {
            const auto neg_v {-v};
            p = mini_to_chars(buffer, static_cast<uint128_t>(neg_v));
        }

        *--p = '-';
    }
    else
    {
        p = mini_to_chars(buffer, static_cast<uint128_t>(v));
    }

    return p;
}

} // namespace detail
} // namespace int128
} // namespace boost

#endif // BOOST_INT128_DETAIL_MINI_TO_CHARS_HPP
