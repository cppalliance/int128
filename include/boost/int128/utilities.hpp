// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_UTILITIES_HPP
#define BOOST_INT128_UTILITIES_HPP

#include <boost/int128/int128.hpp>
#include <boost/int128/bit.hpp>
#include <boost/int128/detail/config.hpp>

#ifndef BOOST_INT128_BUILD_MODULE

#include <cstdint>

#endif

namespace boost {
namespace int128 {

namespace detail {

// Modular addition for 128-bit operands assuming 0 <= a, b < m
BOOST_INT128_HOST_DEVICE constexpr uint128_t addmod(const uint128_t a, const uint128_t b, const uint128_t m) noexcept
{
    const uint128_t s {a + b};

    if (s < a || s >= m)
    {
        return s - m;
    }

    return s;
}

// Modular multiplication via shift-and-add for the full 128-bit modulus case
BOOST_INT128_HOST_DEVICE constexpr uint128_t mulmod_shift(uint128_t a, uint128_t b, const uint128_t m) noexcept
{
    uint128_t result {0};

    while (b != 0U)
    {
        if (static_cast<bool>(b.low & 1U))
        {
            result = addmod(result, a, m);
        }

        a = addmod(a, a, m);
        b >>= 1;
    }

    return result;
}

// Modular multiplication when the modulus fits in 64 bits
BOOST_INT128_HOST_DEVICE constexpr std::uint64_t mulmod_word(const std::uint64_t a, const std::uint64_t b, const std::uint64_t m) noexcept
{
    return ((uint128_t{a} * uint128_t{b}) % uint128_t{m}).low;
}

} // namespace detail

// Computes (base ^ exp) mod m using fast modular exponentiation with
// optimizations specific to the boost::int128 library types
BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr uint128_t powm(uint128_t base, uint128_t exp, const uint128_t m) noexcept
{
    if (BOOST_INT128_UNLIKELY(m == 0U))
    {
        return uint128_t{0};
    }

    if (m == 1U)
    {
        return uint128_t{0};
    }

    if (exp == 0U)
    {
        return uint128_t{1};
    }

    base %= m;

    if (base == 0U)
    {
        return uint128_t{0};
    }

    // Power-of-two modulus: reduction is just a bitmask.
    if (has_single_bit(m))
    {
        const uint128_t mask {m - 1U};
        uint128_t result {1};

        while (exp != 0U)
        {
            if (static_cast<bool>(exp.low & 1U))
            {
                result = (result * base) & mask;
            }

            base = (base * base) & mask;
            exp >>= 1;
        }

        return result;
    }

    // Modulus fits in 64 bits: stay in 64-bit lanes.
    if (m.high == 0U)
    {
        const auto mm {m.low};
        std::uint64_t result {1};
        auto b {base.low};

        while (exp != 0U)
        {
            if (static_cast<bool>(exp.low & 1U))
            {
                result = detail::mulmod_word(result, b, mm);
            }

            b = detail::mulmod_word(b, b, mm);
            exp >>= 1;
        }

        return uint128_t{result};
    }

    // General 128-bit modulus: shift-and-add for each squaring keeps every
    // intermediate strictly below m without needing a 256-bit product.
    uint128_t result {1};

    while (exp != 0U)
    {
        if (static_cast<bool>(exp.low & 1U))
        {
            result = detail::mulmod_shift(result, base, m);
        }

        base = detail::mulmod_shift(base, base, m);
        exp >>= 1;
    }

    return result;
}

// Signed overload. Returns the non-negative residue in [0, m)
BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr int128_t powm(const int128_t base, const int128_t exp, const int128_t m) noexcept
{
    if (BOOST_INT128_UNLIKELY(m <= 0 || exp < 0))
    {
        return int128_t{0};
    }

    const uint128_t um {static_cast<uint128_t>(m)};

    uint128_t ub {};

    if (base.high < 0)
    {
        const uint128_t magnitude {static_cast<uint128_t>(abs(base))};
        const uint128_t r {magnitude % um};
        ub = r == 0U ? uint128_t{0} : static_cast<uint128_t>(um - r);
    }
    else
    {
        ub = static_cast<uint128_t>(base) % um;
    }

    return static_cast<int128_t>(powm(ub, static_cast<uint128_t>(exp), um));
}

// Computes base^exp using exponentiation by squaring. The result is reduced
// modulo 2^128, mirroring the wrap-around behavior of operator*.
BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr uint128_t ipow(uint128_t base, std::uint64_t exp) noexcept
{
    uint128_t result {1};

    while (exp != 0U)
    {
        if (static_cast<bool>(exp & 1U))
        {
            result *= base;
        }

        exp >>= 1;

        if (exp != 0U)
        {
            base *= base;
        }
    }

    return result;
}

// Signed overload. Wraps modulo 2^128 on overflow, matching operator*.
BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr int128_t ipow(int128_t base, std::uint64_t exp) noexcept
{
    int128_t result {1};

    while (exp != 0U)
    {
        if (static_cast<bool>(exp & 1U))
        {
            result *= base;
        }

        exp >>= 1;

        if (exp != 0U)
        {
            base *= base;
        }
    }

    return result;
}

// Integer square root: returns floor(sqrt(n)).
BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr uint128_t isqrt(const uint128_t n) noexcept
{
    if (n < 2U)
    {
        return n;
    }

    // 2^ceil(bit_width(n)/2) is the smallest power of two whose square exceeds n.
    uint128_t x {uint128_t{1} << ((bit_width(n) + 1) / 2)};

    while (true)
    {
        const uint128_t y {(x + n / x) >> 1};

        if (y >= x)
        {
            return x;
        }

        x = y;
    }
}

// Signed overload. Negative inputs are documented to return 0.
BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr int128_t isqrt(const int128_t n) noexcept
{
    if (BOOST_INT128_UNLIKELY(n < 0))
    {
        return int128_t{0};
    }

    return static_cast<int128_t>(isqrt(static_cast<uint128_t>(n)));
}

} // namespace int128
} // namespace boost

#endif // BOOST_INT128_UTILITIES_HPP
