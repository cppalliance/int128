// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_CSTDLIB_HPP
#define BOOST_INT128_CSTDLIB_HPP

#include <boost/int128/int128.hpp>

namespace boost {
namespace int128 {

BOOST_INT128_EXPORT struct u128div_t
{
    uint128_t quot;
    uint128_t rem;
};

BOOST_INT128_EXPORT struct i128div_t
{
    int128_t quot;
    int128_t rem;
};

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr u128div_t div(const uint128_t x, const uint128_t y) noexcept
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

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr i128div_t div(const int128_t x, const int128_t y) noexcept
{
    if (BOOST_INT128_UNLIKELY(x == 0 || y == 0))
    {
        return i128div_t{0, 0};
    }

    const auto abs_lhs {static_cast<uint128_t>(abs(x))};
    const auto abs_rhs {static_cast<uint128_t>(abs(y))};

    if (abs_rhs > abs_lhs)
    {
        return {0, x};
    }

    const auto negative_quot {(x.high < 0) != (y.high < 0)};
    const auto negative_rem {x.high < 0};

    // Narrow fast path: when the divisor magnitude fits in 64 bits, divide the magnitudes with
    // the hardware-accelerated one_word_div and reapply the signs (quotient by operand-sign XOR,
    // remainder by the dividend's sign), beating native signed division for this common case.
    if (abs_rhs.high == 0)
    {
        uint128_t quot {};
        uint128_t rem {};

        if (abs_lhs.high == 0)
        {
            quot = {0, abs_lhs.low / abs_rhs.low};
            rem = {0, abs_lhs.low % abs_rhs.low};
        }
        else
        {
            detail::one_word_div(abs_lhs, abs_rhs.low, quot, rem);
        }

        i128div_t res {static_cast<int128_t>(quot), static_cast<int128_t>(rem)};
        res.quot = negative_quot ? -res.quot : res.quot;
        res.rem = negative_rem ? -res.rem : res.rem;
        return res;
    }

    #if defined(BOOST_INT128_HAS_INT128)

    const auto builtin_x {static_cast<detail::builtin_i128>(x)};
    const auto builtin_y {static_cast<detail::builtin_i128>(y)};
    return i128div_t{static_cast<int128_t>(builtin_x / builtin_y),
                     static_cast<int128_t>(builtin_x % builtin_y)};

    #else

    const auto unsigned_res {div(abs_lhs, abs_rhs)};

    i128div_t res {static_cast<int128_t>(unsigned_res.quot), static_cast<int128_t>(unsigned_res.rem)};

    res.quot = negative_quot ? -res.quot : res.quot;
    res.rem = negative_rem ? -res.rem : res.rem;

    return res;

    #endif
}

} // namespace int128
} // namespace boost

#endif // BOOST_INT128_CSTDLIB_HPP
