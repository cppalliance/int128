// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_NUMERIC_HPP
#define BOOST_INT128_NUMERIC_HPP

#include <boost/int128/bit.hpp>
#include <boost/int128/detail/traits.hpp>
#include <limits>

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
static constexpr bool is_reduced_integer_v {reduced_integers<IntegerType>::value ||
                                              std::is_same<IntegerType, detail::builtin_i128>::value ||
                                              std::is_same<IntegerType, detail::builtin_u128>::value};

#else

template <typename IntegerType>
static constexpr bool is_reduced_integer_v {reduced_integers<IntegerType>::value};

#endif // 128-bit

} // namespace detail

constexpr uint128_t add_sat(const uint128_t x, const uint128_t y) noexcept
{
    const auto z {x + y};

    if (z < x)
    {
        return std::numeric_limits<uint128_t>::max();
    }

    return z;
}

constexpr uint128_t sub_sat(const uint128_t x, const uint128_t y) noexcept
{
    const auto z {x - y};

    if (z > x)
    {
        return std::numeric_limits<uint128_t>::min();
    }

    return z;
}

constexpr uint128_t mul_sat(const uint128_t x, const uint128_t y) noexcept
{
    const auto x_bits {bit_width(x)};
    const auto y_bits {bit_width(y)};

    if (x_bits + y_bits > std::numeric_limits<uint128_t>::digits)
    {
        return std::numeric_limits<uint128_t>::max();
    }

    return x * y;
}

constexpr uint128_t div_sat(const uint128_t x, const uint128_t y) noexcept
{
    return x / y;
}

template <typename TargetType, std::enable_if_t<detail::is_reduced_integer_v<TargetType>, bool> = true>
constexpr uint128_t saturate_cast(const uint128_t value) noexcept
{
    BOOST_INT128_IF_CONSTEXPR (std::is_same<uint128_t, TargetType>::value)
    {
        return value;
    }
    else
    {
        if (value > static_cast<uint128_t>(std::numeric_limits<TargetType>::max()))
        {
            return std::numeric_limits<TargetType>::max();
        }

        return static_cast<TargetType>(value);
    }
}

template <typename TargetType, std::enable_if_t<detail::is_reduced_integer_v<TargetType>, bool> = true>
constexpr int128_t saturate_cast(const int128_t value) noexcept
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
        if (value > static_cast<int128_t>(std::numeric_limits<TargetType>::max()))
        {
            return std::numeric_limits<TargetType>::max();
        }
        else if (value < static_cast<int128_t>(std::numeric_limits<TargetType>::min()))
        {
            return std::numeric_limits<TargetType>::min();
        }

        return static_cast<TargetType>(value);
    }
}

} // namespace int128
} // namespace boost

#endif // BOOST_INT128_NUMERIC_HPP
