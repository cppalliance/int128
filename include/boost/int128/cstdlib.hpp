// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_CSTDLIB_HPP
#define BOOST_INT128_CSTDLIB_HPP

#include <boost/int128/int128.hpp>

namespace boost {
namespace int128 {

struct u128div_t
{
    uint128_t quot;
    uint128_t rem;
};

struct i128div_t
{
    int128_t quot;
    int128_t rem;
};

constexpr u128div_t div(const uint128_t x, const uint128_t y) noexcept
{
    if (BOOST_INT128_UNLIKELY(x == 0U || y == 0U))
    {
        return u128div_t{0U, 0U};
    }

    if (x < y)
    {
        return u128div_t{0U, x};
    }
    else if (y.high != 0U)
    {
        u128div_t res {};
        res.quot = detail::knuth_div(x, y, res.rem);
        return res;
    }
    else
    {
        if (x.high == 0U)
        {
            return u128div_t{x.low / y.low, x.low % y.low};
        }
        else
        {
            u128div_t res {};
            detail::one_word_div(x, y.low, res.quot, res.rem);
            return res;
        }
    }
}

constexpr i128div_t div(const int128_t x, const int128_t y) noexcept
{
    if (BOOST_INT128_UNLIKELY(x == 0 || y == 0))
    {
        return i128div_t{0, 0};
    }

    const auto abs_lhs {static_cast<uint128_t>(abs(x))};
    const auto abs_rhs {static_cast<uint128_t>(abs(y))};

    const auto unsigned_res {div(abs_lhs, abs_rhs)};

    #if defined(_MSC_VER) && !defined(__GNUC__)
    const auto is_neg{static_cast<bool>(x < 0)};
    #else
    const auto is_neg {static_cast<bool>((x < 0) != (y < 0))};
    #endif

    i128div_t res {static_cast<int128_t>(unsigned_res.quot), static_cast<int128_t>(unsigned_res.rem)};

    res.quot = is_neg ? -res.quot : res.quot;
    res.rem = (x < 0) ? -res.rem : res.rem;

    return res;
}

} // namespace int128
} // namespace boost

#endif // BOOST_INT128_CSTDLIB_HPP
