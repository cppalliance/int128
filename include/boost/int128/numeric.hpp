// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_NUMERIC_HPP
#define BOOST_INT128_NUMERIC_HPP

#include <boost/int128/bit.hpp>
#include <boost/int128/detail/traits.hpp>

#ifndef BOOST_INT128_BUILD_MODULE

#include <limits>
#include <iostream>
#include <limits>

#endif

namespace boost {
namespace int128 {

namespace detail {

template <typename IntegerType>
struct reduced_integers
{
    static constexpr bool value {std::is_same<IntegerType, signed char>::value ||
                                 std::is_same<IntegerType, unsigned char>::value ||
                                 std::is_same<IntegerType, signed short>::value ||
                                 std::is_same<IntegerType, unsigned short>::value ||
                                 std::is_same<IntegerType, signed int>::value ||
                                 std::is_same<IntegerType, unsigned int>::value ||
                                 std::is_same<IntegerType, signed long>::value ||
                                 std::is_same<IntegerType, unsigned long>::value ||
                                 std::is_same<IntegerType, signed long long>::value ||
                                 std::is_same<IntegerType, unsigned long long>::value ||
                                 std::is_same<IntegerType, int128_t>::value ||
                                 std::is_same<IntegerType, uint128_t>::value};
};

#if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)

template <typename IntegerType>
BOOST_INT128_INLINE_CONSTEXPR bool is_reduced_integer_v {reduced_integers<IntegerType>::value ||
                                              std::is_same<IntegerType, detail::builtin_i128>::value ||
                                              std::is_same<IntegerType, detail::builtin_u128>::value};

#else

template <typename IntegerType>
BOOST_INT128_INLINE_CONSTEXPR bool is_reduced_integer_v {reduced_integers<IntegerType>::value};

#endif // 128-bit

} // namespace detail

BOOST_INT128_EXPORT constexpr uint128_t add_sat(const uint128_t x, const uint128_t y) noexcept
{
    const auto z {x + y};

    if (z < x)
    {
        return (std::numeric_limits<uint128_t>::max)();
    }

    return z;
}

BOOST_INT128_EXPORT constexpr uint128_t sub_sat(const uint128_t x, const uint128_t y) noexcept
{
    const auto z {x - y};

    if (z > x)
    {
        return (std::numeric_limits<uint128_t>::min)();
    }

    return z;
}

BOOST_INT128_EXPORT constexpr int128_t add_sat(int128_t x, int128_t y) noexcept;
BOOST_INT128_EXPORT constexpr int128_t sub_sat(int128_t x, int128_t y) noexcept;

#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable : 4307) // Addition Overflow
#  pragma warning(disable : 4146) // Unary minus applied to unsigned type
#endif

constexpr int128_t add_sat(const int128_t x, const int128_t y) noexcept
{
    if (x >= 0 && y >= 0)
    {
        constexpr auto max_value {static_cast<uint128_t>((std::numeric_limits<int128_t>::max)())};
        const auto big_x {static_cast<uint128_t>(x)};
        const auto big_y {static_cast<uint128_t>(y)};
        const auto big_res {big_x + big_y};

        return big_res > max_value ? (std::numeric_limits<int128_t>::max)() : static_cast<int128_t>(big_res);
    }
    else if ((x < 0 && y > 0) || (x > 0 && y < 0))
    {
        return x + y;
    }
    else
    {
        // x < 0 and y < 0
        // Nearly the same technique as the positive values case
        constexpr auto max_value {-static_cast<uint128_t>((std::numeric_limits<int128_t>::min)())};
        const auto big_x {static_cast<uint128_t>(abs(x))};
        const auto big_y {static_cast<uint128_t>(abs(y))};
        const auto big_res {big_x + big_y};

        return big_res > max_value ? (std::numeric_limits<int128_t>::min)() : -static_cast<int128_t>(big_res);
    }
}

constexpr int128_t sub_sat(const int128_t x, const int128_t y) noexcept
{
    if (x <= 0 && y >= 0)
    {
        // Underflow case
        const auto res {x - y};
        return res > x ? (std::numeric_limits<int128_t>::min)() : res;
    }
    else if (x > 0 && y < 0)
    {
        // Overflow Case
        constexpr auto max_val {static_cast<uint128_t>((std::numeric_limits<int128_t>::max)())};
        const auto big_x {static_cast<uint128_t>(x)};
        const auto big_y {-static_cast<uint128_t>(y)};
        const auto res {big_x + big_y};

        return (res > max_val || res < big_x) ? (std::numeric_limits<int128_t>::max)() : static_cast<int128_t>(res);
    }
    else
    {
        return x - y;
    }
}

#ifdef _MSC_VER
#  pragma warning(pop)
#endif

BOOST_INT128_EXPORT constexpr uint128_t mul_sat(const uint128_t x, const uint128_t y) noexcept
{
    const auto x_bits {bit_width(x)};
    const auto y_bits {bit_width(y)};

    if ((x_bits + y_bits) > std::numeric_limits<uint128_t>::digits)
    {
        return (std::numeric_limits<uint128_t>::max)();
    }

    return x * y;
}

BOOST_INT128_EXPORT constexpr int128_t mul_sat(const int128_t& x, const int128_t& y) noexcept
{
    const auto x_bits {bit_width(static_cast<uint128_t>(abs(x)))};
    const auto y_bits {bit_width(static_cast<uint128_t>(abs(y)))};

    if ((x_bits + y_bits) > std::numeric_limits<int128_t>::digits)
    {
        if ((x < 0) != (y < 0))
        {
            return (std::numeric_limits<int128_t>::min)();
        }
        else
        {
            return (std::numeric_limits<int128_t>::max)();
        }
    }

    const int128_t res {x * y};
    return res;
}

BOOST_INT128_EXPORT constexpr uint128_t div_sat(const uint128_t x, const uint128_t y) noexcept
{
    return x / y;
}

BOOST_INT128_EXPORT constexpr int128_t div_sat(const int128_t x, const int128_t y) noexcept
{
    if (BOOST_INT128_UNLIKELY(x == (std::numeric_limits<int128_t>::min)() && y == -1))
    {
        // This is the only possible case of overflow
        return (std::numeric_limits<int128_t>::max)();
    }

    return x / y;
}

#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable: 4267)
#endif

BOOST_INT128_EXPORT template <typename TargetType, std::enable_if_t<detail::is_reduced_integer_v<TargetType>, bool> = true>
constexpr TargetType saturate_cast(const uint128_t value) noexcept
{
    BOOST_INT128_IF_CONSTEXPR (std::is_same<uint128_t, TargetType>::value)
    {
        return value;
    }
    else
    {
        if (value > static_cast<uint128_t>((std::numeric_limits<TargetType>::max)()))
        {
            return (std::numeric_limits<TargetType>::max)();
        }

        return static_cast<TargetType>(value);
    }
}

#ifdef _MSC_VER
#  pragma warning(pop)
#endif

BOOST_INT128_EXPORT template <typename TargetType, std::enable_if_t<detail::is_reduced_integer_v<TargetType>, bool> = true>
constexpr TargetType saturate_cast(const int128_t value) noexcept
{
    BOOST_INT128_IF_CONSTEXPR (std::is_same<int128_t, TargetType>::value)
    {
        return value;
    }
    #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)
    else BOOST_INT128_IF_CONSTEXPR (std::is_same<uint128_t, TargetType>::value || std::is_same<detail::builtin_u128, TargetType>::value)
    #else
    else BOOST_INT128_IF_CONSTEXPR (std::is_same<uint128_t, TargetType>::value)
    #endif
    {
        // We can't possibly have overflow in this case
        return value < 0 ? static_cast<TargetType>(0) : static_cast<TargetType>(value);
    }
    else
    {
        if (value > static_cast<int128_t>((std::numeric_limits<TargetType>::max)()))
        {
            return (std::numeric_limits<TargetType>::max)();
        }
        else if (value < static_cast<int128_t>((std::numeric_limits<TargetType>::min)()))
        {
            return (std::numeric_limits<TargetType>::min)();
        }

        return static_cast<TargetType>(value);
    }
}

namespace detail {

constexpr std::uint64_t gcd64(std::uint64_t x, std::uint64_t y) noexcept
{
    if (x == 0)
    {
        return y;
    }
    if (y == 0)
    {
        return x;
    }

    const auto s {impl::countr_impl(x | y)};
    x >>= impl::countr_impl(x);

    do
    {
        y >>= impl::countr_impl(y);
        if (x > y)
        {
            const auto temp {x};
            x = y;
            y = temp;
        }

        y -= x;
    } while (y);

    return x << s;
}

} // namespace detail

constexpr uint128_t gcd(uint128_t a, uint128_t b) noexcept
{
    // Base case
    if (a == 0U)
    {
        return b;
    }
    if (b == 0U)
    {
        return a;
    }

    const auto a_zero {countr_zero(a)};
    const auto b_zero {countr_zero(b)};
    const auto shift {b_zero < a_zero ? b_zero : a_zero};
    a >>= shift;
    b >>= shift;

    do
    {
        b >>= countr_zero(b);

        if (a > b)
        {
            const uint128_t temp {a};
            a = b;
            b = temp;
        }

        b -= a;

        // Stop doing 128-bit math as soon as we can
        if (a.high == 0U && b.high == 0U)
        {
            const auto g {detail::gcd64(a.low, b.low)};
            return uint128_t{0, g} << shift;
        }

    } while (b != 0U);

    return a << shift;
}

constexpr int128_t gcd(const int128_t a, const int128_t b) noexcept
{
    return static_cast<int128_t>(gcd(static_cast<uint128_t>(abs(a)), static_cast<uint128_t>(abs(b))));
}

// For unknown reasons this implementation fails for MSVC x86 only in release mode
// Directly calculating leads to the same failures, so unfortunately we have a viable,
// but very slow impl that we know works.
#if !(defined(_M_IX86) && !defined(_NDEBUG))

constexpr uint128_t lcm(const uint128_t a, const uint128_t b) noexcept
{
    if (a == 0U || b == 0U)
    {
        return static_cast<uint128_t>(0);
    }

    // Calculate GCD first
    const auto g {gcd(a, b)};

    // Compute LCM avoiding overflow: (a/gcd) * b
    return (a / g) * b;
}

#else

constexpr uint128_t lcm(uint128_t a, uint128_t b) noexcept
{
    if (a == 0U || b == 0U)
    {
        return 0;
    }


    unsigned shift{};
    while ((a & 1U) == 0U && (b & 1U) == 0U) 
    {
        a >>= 1U;
        b >>= 1U;
        shift++;
    }

    // Ensure a >= b
    if (a < b)
    {
        std::swap(a, b);
    }

    uint128_t lcm{a};

    while (lcm % b != 0U) 
    {
        lcm += a;
    }

    return lcm << shift;
}

#endif

constexpr int128_t lcm(const int128_t a, const int128_t b) noexcept
{
    return static_cast<int128_t>(lcm(static_cast<uint128_t>(abs(a)), static_cast<uint128_t>(abs(b))));
}

namespace detail {

template <typename T>
constexpr T midpoint_impl(const T a, const T b) noexcept
{
    const auto unsigned_a {static_cast<uint128_t>(a)};
    const auto unsigned_b {static_cast<uint128_t>(b)};

    auto mid {(unsigned_a & unsigned_b) + ((unsigned_a ^ unsigned_b) >> 1)};

    // std::midpoint rounds towards the first parameter
    if ((unsigned_a ^ unsigned_b) & 1U && a > b)
    {
        ++mid;
    }

    return static_cast<T>(mid);
}

} // namespace detail

constexpr uint128_t midpoint(const uint128_t a, const uint128_t b) noexcept
{
    return detail::midpoint_impl(a, b);
}

constexpr int128_t midpoint(const int128_t a, const int128_t b) noexcept
{
    return detail::midpoint_impl(a, b);
}

} // namespace int128
} // namespace boost

#endif // BOOST_INT128_NUMERIC_HPP
