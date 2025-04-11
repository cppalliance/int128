// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_DETAIL_INT128_HPP
#define BOOST_INT128_DETAIL_INT128_HPP

#include <boost/int128/detail/fwd.hpp>
#include <boost/int128/detail/config.hpp>
#include <boost/int128/detail/traits.hpp>
#include <boost/int128/detail/constants.hpp>
#include <boost/int128/detail/clz.hpp>
#include <cstdint>
#include <cstring>

namespace boost {
namespace int128 {

struct
    #ifdef BOOST_INT128_HAS_INT128
    alignas(alignof(detail::builtin_i128))
    #else
    alignas(16)
    #endif
int128_t
{
    #if BOOST_INT128_ENDIAN_LITTLE_BYTE
    std::uint64_t low {};
    std::int64_t high {};
    #else

    #ifdef __GNUC__
    #  pragma GCC diagnostic push
    #  pragma GCC diagnostic ignored "-Wreorder"
    #endif

    std::int64_t high {};
    std::uint64_t low {};

    #ifdef __GNUC__
    #  pragma GCC diagnostic pop
    #endif

    #endif

    // Defaulted basic construction
    constexpr int128_t() noexcept = default;
    constexpr int128_t(const int128_t&) noexcept = default;
    constexpr int128_t(int128_t&&) noexcept = default;
    constexpr int128_t& operator=(const int128_t&) noexcept = default;
    constexpr int128_t& operator=(int128_t&&) noexcept = default;

    // Requires conversion file to be implemented
    constexpr int128_t(const uint128_t& v) noexcept;

    // Construct from integral types
    #if BOOST_INT128_ENDIAN_LITTLE_BYTE

    constexpr int128_t(const std::int64_t hi, const std::uint64_t lo) noexcept : low{lo}, high{hi} {}

    template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
    constexpr int128_t(const SignedInteger v) noexcept : low {static_cast<std::uint64_t>(v)}, high {v < 0 ? -1 : 0} {}

    template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
    constexpr int128_t(const UnsignedInteger v) noexcept : low {static_cast<std::uint64_t>(v)}, high {} {}

    #ifdef BOOST_INT128_HAS_INT128

    constexpr int128_t(const detail::builtin_i128 v) noexcept : low {static_cast<std::uint64_t>(v & detail::low_word_mask)}, high {static_cast<std::int64_t>(v >> 64U)} {}
    constexpr int128_t(const detail::builtin_u128 v) noexcept : low {static_cast<std::uint64_t>(v & detail::low_word_mask)}, high {static_cast<std::int64_t>(v >> 64U)} {}

    #endif // BOOST_INT128_HAS_INT128

    #else // Big endian

    constexpr int128_t(const std::int64_t hi, const std::uint64_t lo) noexcept : high{hi}, low{lo} {}

    template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
    constexpr int128_t(const SignedInteger v) noexcept : high{v < 0 ? -1 : 0}, low{static_cast<std::uint64_t>(v)} {}

    template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
    constexpr int128_t(const UnsignedInteger v) noexcept : high {}, low {static_cast<std::uint64_t>(v)} {}

    #ifdef BOOST_INT128_HAS_INT128

    constexpr int128_t(const detail::builtin_i128 v) noexcept : high {static_cast<std::int64_t>(v >> 64U)}, low {static_cast<std::uint64_t>(v & detail::low_word_mask)} {}
    constexpr int128_t(const detail::builtin_u128 v) noexcept : high {static_cast<std::int64_t>(v >> 64U)}, low {static_cast<std::uint64_t>(v & detail::low_word_mask)} {}

    #endif // BOOST_INT128_HAS_INT128

    #endif // BOOST_INT128_ENDIAN_LITTLE_BYTE

    // Integer Conversion operators
    constexpr operator bool() const noexcept { return low || high; }

    template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
    explicit constexpr operator SignedInteger() const noexcept { return static_cast<SignedInteger>(low); }

    template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
    explicit constexpr operator UnsignedInteger() const noexcept { return static_cast<UnsignedInteger>(low); }

    #ifdef BOOST_INT128_HAS_INT128

    explicit constexpr operator detail::builtin_i128() const noexcept { return static_cast<detail::builtin_i128>(static_cast<detail::builtin_u128>(high) << 64) | low; }

    explicit constexpr operator detail::builtin_u128() const noexcept { return (static_cast<detail::builtin_u128>(high) << 64) | low; }

    #endif // BOOST_INT128_HAS_INT128

    // Conversion to float
    // This is basically the same as ldexp(static_cast<T>(high), 64) + static_cast<T>(low),
    // but can be constexpr at C++11 instead of C++26
    explicit constexpr operator float() const noexcept;
    explicit constexpr operator double() const noexcept;
    explicit constexpr operator long double() const noexcept;

    // Compound Or
    template <BOOST_INT128_DEFAULTED_INTEGER_CONCEPT>
    constexpr int128_t& operator|=(Integer rhs) noexcept;

    constexpr int128_t& operator|=(int128_t rhs) noexcept;

    // Compound And
    template <BOOST_INT128_DEFAULTED_INTEGER_CONCEPT>
    constexpr int128_t& operator&=(Integer rhs) noexcept;

    constexpr int128_t& operator&=(int128_t rhs) noexcept;

    // Compound XOR
    template <BOOST_INT128_DEFAULTED_INTEGER_CONCEPT>
    constexpr int128_t& operator^=(Integer rhs) noexcept;

    constexpr int128_t& operator^=(int128_t rhs) noexcept;

    // Compound Left Shift
    template <BOOST_INT128_DEFAULTED_INTEGER_CONCEPT>
    constexpr int128_t& operator<<=(Integer rhs) noexcept;

    constexpr int128_t& operator<<=(int128_t rhs) noexcept;

    // Compound Right Shift
    template <BOOST_INT128_DEFAULTED_INTEGER_CONCEPT>
    constexpr int128_t& operator>>=(Integer rhs) noexcept;

    constexpr int128_t& operator>>=(int128_t rhs) noexcept;

    // Prefix and postfix increment
    constexpr int128_t& operator++() noexcept;
    constexpr int128_t& operator++(int) noexcept;

    // Prefix and postfix decrment
    constexpr int128_t& operator--() noexcept;
    constexpr int128_t& operator--(int) noexcept;

    // Compound Addition
    template <BOOST_INT128_DEFAULTED_INTEGER_CONCEPT>
    constexpr int128_t& operator+=(Integer rhs) noexcept;

    constexpr int128_t& operator+=(int128_t rhs) noexcept;

    // Compound Subtraction
    template <BOOST_INT128_DEFAULTED_INTEGER_CONCEPT>
    constexpr int128_t& operator-=(Integer rhs) noexcept;

    constexpr int128_t& operator-=(int128_t rhs) noexcept;

    // Compound Multiplication
    template <BOOST_INT128_DEFAULTED_INTEGER_CONCEPT>
    constexpr int128_t& operator*=(Integer rhs) noexcept;

    constexpr int128_t& operator*=(int128_t rhs) noexcept;

    // Compound Division
    template <BOOST_INT128_DEFAULTED_INTEGER_CONCEPT>
    constexpr int128_t& operator/=(Integer rhs) noexcept;

    constexpr int128_t& operator/=(int128_t rhs) noexcept;
};

//=====================================
// Absolute Value function
//=====================================

constexpr int128_t abs(int128_t value) noexcept
{
    if (value.high < 0)
    {
        value.low = ~value.low + 1;
        value.high = ~value.high + (value.low == 0 ? 1 : 0);
    }
    
    return value;
}

//=====================================
// Float Conversion Operators
//=====================================

// The most correct way to do this would be std::ldexp(static_cast<T>(high), 64) + static_cast<T>(low);
// Since std::ldexp is not constexpr until C++23 we can work around this by multiplying the high word
// by 0xFFFFFFFF in order to generally replicate what ldexp is doing in the constexpr context.
// We also avoid pulling in <quadmath.h> for the __float128 case where we would need ldexpq

constexpr int128_t::operator float() const noexcept
{
    return static_cast<float>(high) * detail::offset_value_v<float> + static_cast<float>(low);
}

constexpr int128_t::operator double() const noexcept
{
    return static_cast<double>(high) * detail::offset_value_v<double> + static_cast<double>(low);
}

constexpr int128_t::operator long double() const noexcept
{
    return static_cast<long double>(high) * detail::offset_value_v<long double> + static_cast<long double>(low);
}

//=====================================
// Unary Operators
//=====================================

constexpr int128_t operator+(const int128_t value) noexcept
{
    return value;
}

constexpr int128_t operator-(const int128_t value) noexcept
{
    return (value.low == 0) ? int128_t{-value.high, 0} :
                              int128_t{~value.high, ~value.low + 1};
}

//=====================================
// Equality Operators
//=====================================

constexpr bool operator==(const int128_t lhs, const bool rhs) noexcept
{
    return lhs.high == 0 && lhs.low == static_cast<std::uint64_t>(rhs);
}

constexpr bool operator==(const bool lhs, const int128_t rhs) noexcept
{
    return rhs.high == 0 && rhs.low == static_cast<std::uint64_t>(lhs);
}

#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wsign-conversion"
#  pragma clang diagnostic ignored "-Wsign-compare"
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wsign-conversion"
#  pragma GCC diagnostic ignored "-Wsign-compare"
#endif

constexpr bool operator==(const int128_t lhs, const int128_t rhs) noexcept
{
    // x64 and ARM64 like the values in opposite directions

    #if defined(__aarch64__) || defined(_M_ARM64) || defined(__x86_64__) || defined(_M_X64) || defined(_M_IX86)

    return lhs.low == rhs.low && lhs.high == rhs.high;

    #else

    return lhs.high == rhs.high && lhs.low == rhs.low;

    #endif
}

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr bool operator==(const int128_t lhs, const SignedInteger rhs) noexcept
{
    return lhs.high == (rhs < 0 ? -1 : 0) && lhs.low == static_cast<std::uint64_t>(rhs);
}

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr bool operator==(const SignedInteger lhs, const int128_t rhs) noexcept
{
    return rhs.high == (lhs < 0 ? -1 : 0) && rhs.low == static_cast<std::uint64_t>(lhs);
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr bool operator==(const int128_t lhs, const UnsignedInteger rhs) noexcept
{
    return lhs.high == 0 && lhs.low == static_cast<std::uint64_t>(rhs);
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr bool operator==(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    return rhs.high == 0 && rhs.low == static_cast<std::uint64_t>(lhs);
}

#ifdef BOOST_INT128_HAS_INT128

constexpr bool operator==(const int128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs.low == static_cast<std::uint64_t>(rhs & detail::low_word_mask) &&
           lhs.high == static_cast<std::int64_t>(rhs >> 64);
}

constexpr bool operator==(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    return rhs.low == static_cast<std::uint64_t>(lhs & detail::low_word_mask) &&
           rhs.high == static_cast<std::int64_t>(lhs >> 64);
}

constexpr bool operator==(const int128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return lhs.high < 0 ? false :
           lhs.low == static_cast<std::uint64_t>(rhs & detail::low_word_mask) &&
           lhs.high == static_cast<std::int64_t>(rhs >> 64);
}

constexpr bool operator==(const detail::builtin_u128 lhs, const int128_t rhs) noexcept
{
    return rhs.high < 0 ? false :
           rhs.low == static_cast<std::uint64_t>(lhs & detail::low_word_mask) &&
           rhs.high == static_cast<std::int64_t>(lhs >> 64);
}

#endif // BOOST_INT128_HAS_INT128

//=====================================
// Inequality Operators
//=====================================

constexpr bool operator!=(const int128_t lhs, const int128_t rhs) noexcept
{
    // x64 and ARM64 like the values in opposite directions

    #if defined(__aarch64__) || defined(_M_ARM64) || defined(_M_X64) || defined(_M_IX86)

    return lhs.low != rhs.low || lhs.high != rhs.high;

    #elif defined(__x86_64__) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION) && defined(__GNUC__) && !defined(__clang__)

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return lhs.high != rhs.high || lhs.low != rhs.low;
    }
    else
    {
        detail::builtin_i128 builtin_lhs {};
        detail::builtin_i128 builtin_rhs {};

        std::memcpy(&builtin_lhs, &lhs, sizeof(builtin_lhs));
        std::memcpy(&builtin_rhs, &rhs, sizeof(builtin_rhs));

        return builtin_lhs != builtin_rhs;
    }

    #else

    return lhs.high != rhs.high || lhs.low != rhs.low;

    #endif
}

constexpr bool operator!=(const int128_t lhs, const bool rhs) noexcept
{
    return lhs.high != 0 || lhs.low != static_cast<std::uint64_t>(rhs);
}

constexpr bool operator!=(const bool lhs, const int128_t rhs) noexcept
{
    return rhs.high != 0 || rhs.low != static_cast<std::uint64_t>(lhs);
}

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr bool operator!=(const int128_t lhs, const SignedInteger rhs) noexcept
{
    return lhs.high != (rhs < 0 ? -1 : 0) || lhs.low != static_cast<std::uint64_t>(rhs);
}

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr bool operator!=(const SignedInteger lhs, const int128_t rhs) noexcept
{
    return rhs.high != (lhs < 0 ? -1 : 0) || rhs.low != static_cast<std::uint64_t>(lhs);
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr bool operator!=(const int128_t lhs, const UnsignedInteger rhs) noexcept
{
    return lhs.high != 0 || lhs.low != static_cast<std::uint64_t>(rhs);
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr bool operator!=(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    return rhs.high != 0 || rhs.low != static_cast<std::uint64_t>(lhs);
}

#ifdef BOOST_INT128_HAS_INT128

constexpr bool operator!=(const int128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs.low != static_cast<std::uint64_t>(rhs & detail::low_word_mask) ||
           lhs.high != static_cast<std::int64_t>(rhs >> 64);
}

constexpr bool operator!=(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    return rhs.low != static_cast<std::uint64_t>(lhs & detail::low_word_mask) ||
           rhs.high != static_cast<std::int64_t>(lhs >> 64);
}

constexpr bool operator!=(const int128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return lhs.high < 0 ? true :
           lhs.low != static_cast<std::uint64_t>(rhs & detail::low_word_mask) ||
           lhs.high != static_cast<std::int64_t>(rhs >> 64);
}

constexpr bool operator!=(const detail::builtin_u128 lhs, const int128_t rhs) noexcept
{
    return rhs.high < 0 ? true :
           rhs.low != static_cast<std::uint64_t>(lhs & detail::low_word_mask) ||
           rhs.high != static_cast<std::int64_t>(lhs >> 64);
}

#endif // BOOST_INT128_HAS_INT128

//=====================================
// Less than Operators
//=====================================

constexpr bool operator<(const int128_t lhs, const int128_t rhs) noexcept
{
    // On ARM macs only with the clang compiler is casting to __int128 uniformly better (and seemingly cost free)
    #if defined(__aarch64__) && defined(__APPLE__) && defined(__clang__)

    return static_cast<detail::builtin_i128>(lhs) < static_cast<detail::builtin_i128>(rhs);

    #elif defined(__x86_64__) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION) && defined(__GNUC__) && !defined(__clang__)

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return lhs.high == rhs.high ? lhs.low < rhs.low : lhs.high < rhs.high;
    }
    else
    {
        detail::builtin_i128 builtin_lhs {};
        detail::builtin_i128 builtin_rhs {};

        std::memcpy(&builtin_lhs, &lhs, sizeof(builtin_lhs));
        std::memcpy(&builtin_rhs, &rhs, sizeof(builtin_rhs));

        return builtin_lhs < builtin_rhs;
    }

    #else

    return lhs.high == rhs.high ? lhs.low < rhs.low : lhs.high < rhs.high;

    #endif
}

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr bool operator<(const int128_t lhs, const SignedInteger rhs) noexcept
{
    return lhs.high < -1 ? true : lhs.low < static_cast<std::uint64_t>(rhs);
}

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr bool operator<(const SignedInteger lhs, const int128_t rhs) noexcept
{
    return rhs.high < -1 ? false : static_cast<std::uint64_t>(lhs) < rhs.low;
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr bool operator<(const int128_t lhs, const UnsignedInteger rhs) noexcept
{
    return lhs.high < 0 ? true : lhs.low < static_cast<std::uint64_t>(rhs);
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr bool operator<(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    return rhs.high < 0 ? false : static_cast<std::uint64_t>(lhs) < rhs.low;
}

#ifdef BOOST_INT128_HAS_INT128

constexpr bool operator<(const int128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs < static_cast<int128_t>(rhs);
}

constexpr bool operator<(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    return static_cast<int128_t>(lhs) < rhs;
}

constexpr bool operator<(const int128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return lhs.high < 0 ? false : lhs < static_cast<int128_t>(rhs);
}

constexpr bool operator<(const detail::builtin_u128 lhs, const int128_t rhs) noexcept
{
    return rhs.high < 0 ? true : static_cast<int128_t>(lhs) < rhs;
}

#endif // BOOST_INT128_HAS_INT128

//=====================================
// Greater than Operators
//=====================================

constexpr bool operator>(const int128_t lhs, const int128_t rhs) noexcept
{
    // On ARM macs only with the clang compiler is casting to __int128 uniformly better (and seemingly cost free)
    #if defined(__aarch64__) && defined(__APPLE__) && defined(__clang__)

    return static_cast<detail::builtin_i128>(lhs) > static_cast<detail::builtin_i128>(rhs);

    #elif defined(__x86_64__) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION) && defined(__GNUC__) && !defined(__clang__)

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return lhs.high == rhs.high ? lhs.low > rhs.low : lhs.high > rhs.high;
    }
    else
    {
        detail::builtin_i128 builtin_lhs {};
        detail::builtin_i128 builtin_rhs {};

        std::memcpy(&builtin_lhs, &lhs, sizeof(builtin_lhs));
        std::memcpy(&builtin_rhs, &rhs, sizeof(builtin_rhs));

        return builtin_lhs > builtin_rhs;
    }

    #else

    return lhs.high == rhs.high ? lhs.low > rhs.low : lhs.high > rhs.high;

    #endif
}

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr bool operator>(const int128_t lhs, const SignedInteger rhs) noexcept
{
    return lhs.high > 0 ? true : lhs.low > static_cast<std::uint64_t>(rhs);
}

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr bool operator>(const SignedInteger lhs, const int128_t rhs) noexcept
{
    return rhs.high < -1 ? true : static_cast<std::uint64_t>(lhs) > rhs.low;
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr bool operator>(const int128_t lhs, const UnsignedInteger rhs) noexcept
{
    return lhs.high > 0 ? true : lhs.low > static_cast<std::uint64_t>(rhs);
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr bool operator>(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    return rhs.high < 0 ? true : static_cast<std::uint64_t>(lhs) > rhs.low;
}

#ifdef BOOST_INT128_HAS_INT128

constexpr bool operator>(const int128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs > static_cast<int128_t>(rhs);
}

constexpr bool operator>(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    return static_cast<int128_t>(lhs) > rhs;
}

constexpr bool operator>(const int128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return lhs.high < 0 ? false : lhs > static_cast<int128_t>(rhs);
}

constexpr bool operator>(const detail::builtin_u128 lhs, const int128_t rhs) noexcept
{
    return rhs.high < 0 ? true : static_cast<int128_t>(lhs) > rhs;
}

#endif // BOOST_INT128_HAS_INT128

//=====================================
// Less Equal Operators
//=====================================

constexpr bool operator<=(const int128_t lhs, const int128_t rhs) noexcept
{
    // On ARM macs only with the clang compiler is casting to __int128 uniformly better (and seemingly cost free)
    #if defined(__aarch64__) && defined(__APPLE__) && defined(__clang__)

    return static_cast<detail::builtin_i128>(lhs) <= static_cast<detail::builtin_i128>(rhs);

    #elif defined(__x86_64__) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION) && defined(__GNUC__) && !defined(__clang__)

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return lhs.high == rhs.high ? lhs.low <= rhs.low : lhs.high <= rhs.high;
    }
    else
    {
        detail::builtin_i128 builtin_lhs {};
        detail::builtin_i128 builtin_rhs {};

        std::memcpy(&builtin_lhs, &lhs, sizeof(builtin_lhs));
        std::memcpy(&builtin_rhs, &rhs, sizeof(builtin_rhs));

        return builtin_lhs <= builtin_rhs;
    }

    #else

    return lhs.high == rhs.high ? lhs.low <= rhs.low : lhs.high <= rhs.high;

    #endif
}

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr bool operator<=(const int128_t lhs, const SignedInteger rhs) noexcept
{
    return lhs.high < -1 ? true : lhs.low <= static_cast<std::uint64_t>(rhs);
}

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr bool operator<=(const SignedInteger lhs, const int128_t rhs) noexcept
{
    return rhs.high < -1 ? false : static_cast<std::uint64_t>(lhs) <= rhs.low;
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr bool operator<=(const int128_t lhs, const UnsignedInteger rhs) noexcept
{
    return lhs.high < 0 ? true : lhs.low <= static_cast<std::uint64_t>(rhs);
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr bool operator<=(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    return rhs.high < 0 ? false : static_cast<std::uint64_t>(lhs) <= rhs.low;
}

#ifdef BOOST_INT128_HAS_INT128

constexpr bool operator<=(const int128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs <= static_cast<int128_t>(rhs);
}

constexpr bool operator<=(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    return static_cast<int128_t>(lhs) <= rhs;
}

constexpr bool operator<=(const int128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return lhs.high < 0 ? true : lhs <= static_cast<int128_t>(rhs);
}

constexpr bool operator<=(const detail::builtin_u128 lhs, const int128_t rhs) noexcept
{
    return rhs.high < 0 ? false : static_cast<int128_t>(lhs) <= rhs;
}

#endif // BOOST_INT128_HAS_INT128

//=====================================
// Greater Equal Operators
//=====================================

constexpr bool operator>=(const int128_t lhs, const int128_t rhs) noexcept
{
    // On ARM macs only with the clang compiler is casting to __int128 uniformly better (and seemingly cost free)
    #if defined(__aarch64__) && defined(__APPLE__) && defined(__clang__)

    return static_cast<detail::builtin_i128>(lhs) >= static_cast<detail::builtin_i128>(rhs);

    #elif defined(__x86_64__) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION) && defined(__GNUC__) && !defined(__clang__)

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return lhs.high == rhs.high ? lhs.low >= rhs.low : lhs.high >= rhs.high;
    }
    else
    {
        detail::builtin_i128 builtin_lhs {};
        detail::builtin_i128 builtin_rhs {};

        std::memcpy(&builtin_lhs, &lhs, sizeof(builtin_lhs));
        std::memcpy(&builtin_rhs, &rhs, sizeof(builtin_rhs));

        return builtin_lhs >= builtin_rhs;
    }

    #else

    return lhs.high == rhs.high ? lhs.low >= rhs.low : lhs.high >= rhs.high;

    #endif
}

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr bool operator>=(const int128_t lhs, const SignedInteger rhs) noexcept
{
    return lhs.high < -1 ? false : lhs.low >= static_cast<std::uint64_t>(rhs);
}

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr bool operator>=(const SignedInteger lhs, const int128_t rhs) noexcept
{
    return rhs.high < -1 ? true : static_cast<std::uint64_t>(lhs) >= rhs.low;
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr bool operator>=(const int128_t lhs, const UnsignedInteger rhs) noexcept
{
    return lhs.high < 0 ? false : lhs.low >= static_cast<std::uint64_t>(rhs);
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr bool operator>=(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    return rhs.high < 0 ? true : static_cast<std::uint64_t>(lhs) >= rhs.low;
}

#ifdef BOOST_INT128_HAS_INT128

constexpr bool operator>=(const int128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs >= static_cast<int128_t>(rhs);
}

constexpr bool operator>=(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    return static_cast<int128_t>(lhs) >= rhs;
}

constexpr bool operator>=(const int128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return lhs.high < 0 ? false : lhs >= static_cast<int128_t>(rhs);
}

constexpr bool operator>=(const detail::builtin_u128 lhs, const int128_t rhs) noexcept
{
    return rhs.high < 0 ? true : static_cast<int128_t>(lhs) >= rhs;
}

#endif // BOOST_INT128_HAS_INT128

//=====================================
// Not Operator
//=====================================

constexpr int128_t operator~(const int128_t rhs) noexcept
{
    return {~rhs.high, ~rhs.low};
}

//=====================================
// Or Operator
//=====================================

constexpr int128_t operator|(const int128_t lhs, const int128_t rhs) noexcept
{
    return {lhs.high | rhs.high, lhs.low | rhs.low};
}

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr int128_t operator|(const int128_t lhs, const SignedInteger rhs) noexcept
{
    return {lhs.high | (rhs < 0 ? -1 : 0), lhs.low | static_cast<std::uint64_t>(rhs)};
}

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr int128_t operator|(const SignedInteger lhs, const int128_t rhs) noexcept
{
    return {rhs.high | (lhs < 0 ? -1 : 0), static_cast<std::uint64_t>(lhs) | rhs.low};
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr int128_t operator|(const int128_t lhs, const UnsignedInteger rhs) noexcept
{
    return {lhs.high, lhs.low | static_cast<std::uint64_t>(rhs)};
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr int128_t operator|(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    return {rhs.high, static_cast<std::uint64_t>(lhs) | rhs.low};
}

#ifdef BOOST_INT128_HAS_INT128

constexpr int128_t operator|(const int128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs | static_cast<int128_t>(rhs);
}

constexpr int128_t operator|(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    return static_cast<int128_t>(lhs) | rhs;
}

constexpr int128_t operator|(const int128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return lhs | static_cast<int128_t>(rhs);
}

constexpr int128_t operator|(const detail::builtin_u128 lhs, const int128_t rhs) noexcept
{
    return static_cast<int128_t>(lhs) | rhs;
}

#endif // BOOST_INT128_HAS_INT128

//=====================================
// Compound OR Operator
//=====================================

template <BOOST_INT128_INTEGER_CONCEPT>
constexpr int128_t& int128_t::operator|=(const Integer rhs) noexcept
{
    *this = *this | rhs;
    return *this;
}

constexpr int128_t& int128_t::operator|=(const int128_t rhs) noexcept
{
    *this = *this | rhs;
    return *this;
}

//=====================================
// And Operator
//=====================================

constexpr int128_t operator&(const int128_t lhs, const int128_t rhs) noexcept
{
    return {lhs.high & rhs.high, lhs.low & rhs.low};
}

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr int128_t operator&(const int128_t lhs, const SignedInteger rhs) noexcept
{
    return {lhs.high & (rhs < 0 ? -1 : 0), lhs.low & static_cast<std::uint64_t>(rhs)};
}

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr int128_t operator&(const SignedInteger lhs, const int128_t rhs) noexcept
{
    return {rhs.high & (lhs < 0 ? -1 : 0), static_cast<std::uint64_t>(lhs) & rhs.low};
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr int128_t operator&(const int128_t lhs, const UnsignedInteger rhs) noexcept
{
    return {lhs.high, lhs.low & static_cast<std::uint64_t>(rhs)};
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr int128_t operator&(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    return {rhs.high, static_cast<std::uint64_t>(lhs) & rhs.low};
}

#ifdef BOOST_INT128_HAS_INT128

constexpr int128_t operator&(const int128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs & static_cast<int128_t>(rhs);
}

constexpr int128_t operator&(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    return static_cast<int128_t>(lhs) & rhs;
}

constexpr int128_t operator&(const int128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return lhs & static_cast<int128_t>(rhs);
}

constexpr int128_t operator&(const detail::builtin_u128 lhs, const int128_t rhs) noexcept
{
    return static_cast<int128_t>(lhs) & rhs;
}

#endif // BOOST_INT128_HAS_INT128

//=====================================
// Compound And Operator
//=====================================

template <BOOST_INT128_INTEGER_CONCEPT>
constexpr int128_t& int128_t::operator&=(const Integer rhs) noexcept
{
    *this = *this & rhs;
    return *this;
}

constexpr int128_t& int128_t::operator&=(const int128_t rhs) noexcept
{
    *this = *this & rhs;
    return *this;
}

//=====================================
// XOR Operator
//=====================================

constexpr int128_t operator^(const int128_t lhs, const int128_t rhs) noexcept
{
    return {lhs.high ^ rhs.high, lhs.low ^ rhs.low};
}

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr int128_t operator^(const int128_t lhs, const SignedInteger rhs) noexcept
{
    return {lhs.high ^ (rhs < 0 ? -1 : 0), lhs.low ^ static_cast<std::uint64_t>(rhs)};
}

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr int128_t operator^(const SignedInteger lhs, const int128_t rhs) noexcept
{
    return {rhs.high ^ (lhs < 0 ? -1 : 0), static_cast<std::uint64_t>(lhs) ^ rhs.low};
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr int128_t operator^(const int128_t lhs, const UnsignedInteger rhs) noexcept
{
    return {lhs.high, lhs.low ^ static_cast<std::uint64_t>(rhs)};
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr int128_t operator^(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    return {rhs.high, static_cast<std::uint64_t>(lhs) ^ rhs.low};
}

#ifdef BOOST_INT128_HAS_INT128

constexpr int128_t operator^(const int128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs ^ static_cast<int128_t>(rhs);
}

constexpr int128_t operator^(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    return static_cast<int128_t>(lhs) ^ rhs;
}

constexpr int128_t operator^(const int128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return lhs ^ static_cast<int128_t>(rhs);
}

constexpr int128_t operator^(const detail::builtin_u128 lhs, const int128_t rhs) noexcept
{
    return static_cast<int128_t>(lhs) ^ rhs;
}

#endif // BOOST_INT128_HAS_INT128

//=====================================
// Compound XOR Operator
//=====================================

template <BOOST_INT128_INTEGER_CONCEPT>
constexpr int128_t& int128_t::operator^=(Integer rhs) noexcept
{
    *this = *this ^ rhs;
    return *this;
}

constexpr int128_t& int128_t::operator^=(int128_t rhs) noexcept
{
    *this = *this ^ rhs;
    return *this;
}

//=====================================
// Left Shift Operator
//=====================================

template <BOOST_INT128_DEFAULTED_INTEGER_CONCEPT>
constexpr int128_t operator<<(const int128_t lhs, const Integer rhs) noexcept
{
    if (rhs < 0 || rhs >= 128)
    {
        return {0, 0};
    }

    if (rhs == 0)
    {
        return lhs;
    }

    if (rhs == 64)
    {
        return {static_cast<std::int64_t>(lhs.low), 0};
    }

    if (rhs > 64)
    {
        return {static_cast<std::int64_t>(lhs.low << (rhs - 64)), 0};
    }

    // For shifts < 64
    std::uint64_t high_part = (static_cast<std::uint64_t>(lhs.high) << rhs) |
                              (lhs.low >> (64 - rhs));

    return {
        static_cast<std::int64_t>(high_part),
        lhs.low << rhs
    };
}

template <typename Integer, std::enable_if_t<detail::is_any_integer_v<Integer> && (sizeof(Integer) * 8 > 16), bool> = true>
constexpr Integer operator<<(const Integer lhs, const int128_t rhs) noexcept
{
    constexpr auto bit_width {sizeof(Integer) * 8};

    if (rhs.high != 0 || rhs.low >= bit_width)
    {
        return 0;
    }

    return lhs << rhs.low;
}

template <typename SignedInteger, std::enable_if_t<detail::is_signed_integer_v<SignedInteger> && (sizeof(SignedInteger) * 8 <= 16), bool> = true>
constexpr int operator<<(const SignedInteger lhs, const int128_t rhs) noexcept
{
    constexpr auto bit_width {sizeof(SignedInteger) * 8};

    if (rhs.high != 0 || rhs.low >= bit_width)
    {
        return 0;
    }

    return static_cast<int>(lhs) << rhs.low;
}

template <typename UnsignedInteger, std::enable_if_t<detail::is_unsigned_integer_v<UnsignedInteger> && (sizeof(UnsignedInteger) * 8 <= 16), bool> = true>
constexpr unsigned operator<<(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    constexpr auto bit_width {sizeof(UnsignedInteger) * 8};

    if (rhs.high != 0 || rhs.low >= bit_width)
    {
        return 0;
    }

    return static_cast<unsigned>(lhs) << rhs.low;
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4804) // Unsafe use of type bool in operation
#endif // _MSC_VER

template <BOOST_INT128_INTEGER_CONCEPT>
constexpr int128_t& int128_t::operator<<=(const Integer rhs) noexcept
{
    *this = *this << rhs;
    return *this;
}

constexpr int128_t& int128_t::operator<<=(const int128_t rhs) noexcept
{
    *this = *this << rhs;
    return *this;
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

//=====================================
// Right Shift Operator
//=====================================

template <BOOST_INT128_DEFAULTED_INTEGER_CONCEPT>
constexpr int128_t operator>>(const int128_t lhs, const Integer rhs) noexcept
{
    if (rhs < 0 || rhs >= 128)
    {
        return lhs.high < 0 ? int128_t{-1, UINT64_MAX} : int128_t{0, 0};
    }

    if (rhs == 0)
    {
        return lhs;
    }

    if (rhs == 64)
    {
        return {lhs.high < 0 ? -1 : 0, static_cast<std::uint64_t>(lhs.high)};
    }

    if (rhs > 64)
    {
        return {lhs.high < 0 ? -1 : 0, static_cast<std::uint64_t>(lhs.high >> (rhs - 64))};
    }

    // For shifts < 64
    const auto high_to_low {static_cast<std::uint64_t>(lhs.high) << (64 - rhs)};
    const auto low_shifted {lhs.low >> rhs};
    const auto low_part {high_to_low | low_shifted};

    return {
        lhs.high >> rhs,
        low_part
    };
}

template <typename Integer, std::enable_if_t<detail::is_any_integer_v<Integer> && (sizeof(Integer) * 8 > 16), bool> = true>
constexpr Integer operator>>(const Integer lhs, const int128_t rhs) noexcept
{
    constexpr auto bit_width {sizeof(Integer) * 8};

    if (rhs.high != 0 || rhs.low >= bit_width)
    {
        return 0;
    }

    return lhs >> rhs.low;
}

template <typename SignedInteger, std::enable_if_t<detail::is_signed_integer_v<SignedInteger> && (sizeof(SignedInteger) * 8 <= 16), bool> = true>
constexpr int operator>>(const SignedInteger lhs, const int128_t rhs) noexcept
{
    constexpr auto bit_width {sizeof(SignedInteger) * 8};

    if (rhs.high != 0 || rhs.low >= bit_width)
    {
        return 0;
    }

    return static_cast<int>(lhs) >> rhs.low;
}

template <typename UnsignedInteger, std::enable_if_t<detail::is_unsigned_integer_v<UnsignedInteger> && (sizeof(UnsignedInteger) * 8 <= 16), bool> = true>
constexpr unsigned operator>>(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    constexpr auto bit_width {sizeof(UnsignedInteger) * 8};

    if (rhs.high != 0 || rhs.low >= bit_width)
    {
        return 0;
    }

    return static_cast<unsigned>(lhs) >> rhs.low;
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4804) // Unsafe use of type bool in operation
#endif // _MSC_VER

template <BOOST_INT128_INTEGER_CONCEPT>
constexpr int128_t& int128_t::operator>>=(const Integer rhs) noexcept
{
    *this = *this >> rhs;
    return *this;
}

constexpr int128_t& int128_t::operator>>=(const int128_t rhs) noexcept
{
    *this = *this >> rhs;
    return *this;
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

//=====================================
// Increment Operators
//=====================================

constexpr int128_t& int128_t::operator++() noexcept
{
    if (++low == UINT64_C(0))
    {
        ++high;
    }

    return *this;
}

constexpr int128_t& int128_t::operator++(int) noexcept
{
    if (++low == UINT64_C(0))
    {
        ++high;
    }

    return *this;
}

//=====================================
// Decrement Operators
//=====================================

constexpr int128_t& int128_t::operator--() noexcept
{
    if (low-- == UINT64_C(0))
    {
        --high;
    }

    return *this;
}

constexpr int128_t& int128_t::operator--(int) noexcept
{
    if (low-- == UINT64_C(0))
    {
        --high;
    }

    return *this;
}

//=====================================
// Addition Operators
//=====================================

namespace detail {

BOOST_INT128_FORCE_INLINE constexpr int128_t library_add(const int128_t lhs, const int128_t rhs) noexcept
{
    const auto new_low {lhs.low + rhs.low};
    const auto new_high {static_cast<std::uint64_t>(lhs.high) +
                                        static_cast<std::uint64_t>(rhs.high) +
                                        static_cast<std::uint64_t>(new_low < lhs.low)};

    return int128_t{static_cast<std::int64_t>(new_high), new_low};
}

BOOST_INT128_FORCE_INLINE constexpr int128_t default_add(const int128_t lhs, const int128_t rhs) noexcept
{
    #ifdef BOOST_INT128_HAS_BUILTIN_ADD_OVERFLOW

    std::uint64_t result_low {};
    std::uint64_t result_high {};

    result_low = lhs.low + rhs.low;
    result_high = lhs.high + rhs.high + __builtin_add_overflow(lhs.low, rhs.low, &result_low);

    return int128_t{static_cast<std::int64_t>(result_high), result_low};

    #elif defined(__x86_64__) && !defined(_WIN32)

    return static_cast<detail::builtin_i128>(lhs) + static_cast<detail::builtin_i128>(rhs);

    #elif defined(__aarch64__) && !defined(__APPLE__) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION) && defined(__GNUC__)

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return library_add(lhs, rhs);
    }
    else
    {
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wclass-memaccess"

        detail::builtin_i128 builtin_lhs {};
        detail::builtin_i128 builtin_rhs {};

        std::memcpy(&builtin_lhs, &lhs, sizeof(builtin_i128));
        std::memcpy(&builtin_rhs, &rhs, sizeof(builtin_i128));

        auto builtin_res {builtin_lhs + builtin_rhs};

        int128_t result {};
        std::memcpy(&result, &builtin_res, sizeof(int128_t));

        return result;

        #pragma GCC diagnostic pop
    }

    #elif defined(_M_AMD64) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION)

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return library_add(lhs, rhs);
    }
    else
    {
        int128_t result {};
        const auto carry {BOOST_INT128_ADD_CARRY(0, lhs.low, rhs.low, &result.low)};
        BOOST_INT128_ADD_CARRY(carry, static_cast<std::uint64_t>(lhs.high), static_cast<std::uint64_t>(rhs.high), reinterpret_cast<std::uint64_t*>(&result.high));

        return result;
    }

    #else

    return library_add(lhs, rhs);

    #endif
}

template <BOOST_INT128_DEFAULTED_INTEGER_CONCEPT>
BOOST_INT128_FORCE_INLINE constexpr int128_t default_add(const int128_t lhs, const Integer rhs) noexcept
{
    const auto new_low {lhs.low + rhs};
    const auto new_high {lhs.high + static_cast<std::int64_t>(new_low < lhs.low)};
    return int128_t{new_high, new_low};
}

BOOST_INT128_FORCE_INLINE constexpr int128_t library_sub(const int128_t lhs, const int128_t rhs) noexcept
{
    const auto new_low {lhs.low - rhs.low};
    const auto new_high {lhs.high - rhs.high - static_cast<std::int64_t>(lhs.low < rhs.low)};

    return int128_t{new_high, new_low};
}

BOOST_INT128_FORCE_INLINE constexpr int128_t default_sub(const int128_t lhs, const int128_t rhs) noexcept
{
    #if defined(BOOST_INT128_HAS_BUILTIN_SUB_OVERFLOW)

    // __builtin_sub_overflow is marked constexpr so we don't need if consteval handling
    std::uint64_t result_low {lhs.low - rhs.low};
    const auto result_high {static_cast<std::uint64_t>(lhs.high - rhs.high - __builtin_sub_overflow(lhs.low, rhs.low, &result_low))};

    return int128_t{static_cast<std::int64_t>(result_high), result_low};

    #elif defined(_M_AMD64) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION)

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return library_sub(lhs, rhs);
    }
    else
    {
        int128_t result {};
        const auto borrow {BOOST_INT128_SUB_BORROW(0, lhs.low, rhs.low, &result.low)};
        BOOST_INT128_SUB_BORROW(borrow, static_cast<std::uint64_t>(lhs.high), static_cast<std::uint64_t>(rhs.high), reinterpret_cast<std::uint64_t*>(&result.high));

        return result;
    }

    #else

    return library_sub(lhs, rhs);

    #endif
}

template <BOOST_INT128_DEFAULTED_INTEGER_CONCEPT>
BOOST_INT128_FORCE_INLINE constexpr int128_t default_sub(const int128_t lhs, const Integer rhs) noexcept
{
    const auto new_low {lhs.low - rhs};
    const auto new_high {lhs.high - static_cast<std::int64_t>(lhs.low < rhs)};
    return int128_t{static_cast<std::int64_t>(new_high), new_low};
}

}

// On s390x with multiple different versions of GCC and language standards
// doing addition via subtraction is >10% faster in the benchmarks
#if defined(__s390__) || defined(__s390x__)

constexpr int128_t operator+(const int128_t lhs, const int128_t rhs) noexcept
{
    return detail::default_sub(lhs, -rhs);
}

#else

constexpr int128_t operator+(const int128_t lhs, const int128_t rhs) noexcept
{
    return detail::default_add(lhs, rhs);
}

#endif

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr int128_t operator+(const int128_t lhs, const UnsignedInteger rhs) noexcept
{
    return detail::default_add(lhs, rhs);
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr int128_t operator+(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    return detail::default_add(rhs, lhs);
}

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr int128_t operator+(const int128_t lhs, const SignedInteger rhs) noexcept
{
    return rhs > 0 ? detail::default_add(lhs, rhs) : detail::default_sub(lhs, -rhs);
}

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr int128_t operator+(const SignedInteger lhs, const int128_t rhs) noexcept
{
    return lhs > 0 ? detail::default_add(rhs, lhs) : detail::default_sub(rhs, -lhs);
}

#ifdef BOOST_INT128_HAS_INT128

constexpr int128_t operator+(const int128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return detail::default_add(lhs, static_cast<int128_t>(rhs));
}

constexpr int128_t operator+(const detail::builtin_u128 lhs, const int128_t rhs) noexcept
{
    return detail::default_add(rhs, static_cast<int128_t>(lhs));
}

constexpr int128_t operator+(const int128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return detail::default_add(lhs, static_cast<int128_t>(rhs));
}

constexpr int128_t operator+(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    return detail::default_add(rhs, static_cast<int128_t>(lhs));
}

#endif // BOOST_INT128_HAS_INT128

template <BOOST_INT128_INTEGER_CONCEPT>
constexpr int128_t& int128_t::operator+=(const Integer rhs) noexcept
{
    *this = *this + rhs;
    return *this;
}

constexpr int128_t& int128_t::operator+=(const int128_t rhs) noexcept
{
    *this = *this + rhs;
    return *this;
}

//=====================================
// Subtraction Operators
//=====================================

constexpr int128_t operator-(const int128_t lhs, const int128_t rhs) noexcept
{
    return detail::default_sub(lhs, rhs);
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr int128_t operator-(const int128_t lhs, const UnsignedInteger rhs) noexcept
{
    return detail::default_sub(lhs, rhs);
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr int128_t operator-(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    return detail::default_add(-rhs, lhs);
}

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr int128_t operator-(const int128_t lhs, const SignedInteger rhs) noexcept
{
    return detail::default_sub(lhs, static_cast<int128_t>(rhs));
}

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr int128_t operator-(const SignedInteger lhs, const int128_t rhs) noexcept
{
    return detail::default_sub(static_cast<int128_t>(lhs), rhs);
}

#ifdef BOOST_INT128_HAS_INT128

constexpr int128_t operator-(const int128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return static_cast<int128_t>(static_cast<detail::builtin_i128>(lhs) - rhs);
}

constexpr int128_t operator-(const detail::builtin_u128 lhs, const int128_t rhs) noexcept
{
    return static_cast<int128_t>(lhs - static_cast<detail::builtin_i128>(rhs));
}

constexpr int128_t operator-(const int128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs - static_cast<int128_t>(rhs);
}

constexpr int128_t operator-(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    return static_cast<int128_t>(lhs) - rhs;
}

#endif

template <BOOST_INT128_INTEGER_CONCEPT>
constexpr int128_t& int128_t::operator-=(const Integer rhs) noexcept
{
    *this = *this - rhs;
    return *this;
}

constexpr int128_t& int128_t::operator-=(const int128_t rhs) noexcept
{
    *this = *this - rhs;
    return *this;
}

//=====================================
// Multiplication Operators
//=====================================

namespace detail {

BOOST_INT128_FORCE_INLINE constexpr int128_t signed_shift_left_32(const std::uint64_t low) noexcept
{
    return {static_cast<std::int64_t>(low >> 32), low << 32};
}

BOOST_INT128_FORCE_INLINE constexpr int128_t library_mul(const int128_t lhs, const int128_t rhs) noexcept
{
    const auto a {lhs.low >> 32U};
    const auto b {lhs.low & UINT32_MAX};
    const auto c {rhs.low >> 32U};
    const auto d {rhs.low & UINT32_MAX};

    int128_t result { static_cast<std::int64_t>(lhs.high * rhs.low + lhs.low * rhs.high + a * c), b * d };
    result += signed_shift_left_32(a * d) + signed_shift_left_32(b * c);

    return result;
}

#ifdef __SSE2__

BOOST_INT128_FORCE_INLINE int128_t sse_mul(const int128_t lhs, const int128_t rhs) noexcept
{
    int128_t result;

    const auto a0 {static_cast<std::uint32_t>(lhs.low & UINT32_MAX)};
    const auto a1 {static_cast<std::uint32_t>(lhs.low >> 32)};
    const auto a2 {static_cast<std::uint32_t>(lhs.high & UINT32_MAX)};
    const auto a3 {static_cast<std::uint32_t>(lhs.high >> 32)};

    const auto b0 {static_cast<std::uint32_t>(rhs.low & UINT32_MAX)};
    const auto b1 {static_cast<std::uint32_t>(rhs.low >> 32)};
    const auto b2 {static_cast<std::uint32_t>(rhs.high & UINT32_MAX)};
    const auto b3 {static_cast<std::uint32_t>(rhs.high >> 32)};

    // Perform multiplications in parallel using SSE2
    __m128i a_vec = _mm_set_epi32(0, a2, a1, a0);
    __m128i b0_vec = _mm_set1_epi32(b0);
    __m128i b1_vec = _mm_set1_epi32(b1);

    // Multiply a_vec by b0 and b1
    __m128i prod0 = _mm_mul_epu32(a_vec, b0_vec);
    __m128i prod1 = _mm_mul_epu32(a_vec, b1_vec);

    // Extract results and handle carries
    std::uint64_t p00 = _mm_cvtsi128_si64(prod0);
    std::uint64_t p10 = _mm_cvtsi128_si64(_mm_srli_si128(prod0, 8));
    std::uint64_t p01 = _mm_cvtsi128_si64(prod1);
    std::uint64_t p11 = _mm_cvtsi128_si64(_mm_srli_si128(prod1, 8));

    // Additional multiplications for higher parts
    const auto p20 {static_cast<std::uint64_t>(a2) * b0};
    const auto p02 {static_cast<std::uint64_t>(a0) * b2};
    const auto p21 {static_cast<std::uint64_t>(a2) * b1};
    const auto p12 {static_cast<std::uint64_t>(a1) * b2};

    const auto high_products {static_cast<std::int64_t>(
                           (a3 * b0 + a2 * b1 + a1 * b2 + a0 * b3) +
                           (a3 * b1 + a2 * b2 + a1 * b3) +
                           (a3 * b2 + a2 * b3) +
                           (a3 * b3))};

    // Combine results with carries
    const auto middle {(p00 >> 32) + (p01 & UINT32_MAX) + (p10 & UINT32_MAX)};
    result.low = (middle << 32) | (p00 & UINT32_MAX);
    result.high = static_cast<std::int64_t>(
                    high_products + p11 + p20 + p02 + p21 + p12 +
                    (p01 >> 32) + (p10 >> 32) + (middle >> 32)
                  );

    return result;
}

#endif

#if defined(_M_AMD64) && !defined(__GNUC__)

BOOST_INT128_FORCE_INLINE int128_t msvc_amd64_mul(const int128_t lhs, const int128_t rhs) noexcept
{
    int128_t result {};
    result.low = _umul128(lhs.low, rhs.low, reinterpret_cast<std::uint64_t*>(&result.high));
    result.high += lhs.low * rhs.high;
    result.high += lhs.high * rhs.low;

    return result;
}

#endif

#if defined(_M_IX86) || defined(_M_ARM) || defined(__arm__)

#ifdef __GNUC__
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wclass-memaccess"
#endif

// See: The Art of Computer Programming Volume 2 (Semi-numerical algorithms) section 4.3.1
// Algorithm M: Multiplication of Non-negative integers
BOOST_INT128_FORCE_INLINE int128_t msvc_32_mul(const int128_t lhs, const int128_t rhs) noexcept
{
    constexpr std::size_t width {4};

    const auto negative {static_cast<bool>((lhs < 0) ^ (rhs < 0))};

    std::uint32_t u_parts[width] {};
    std::uint32_t v_parts[width] {};
    std::memcpy(&u_parts, &lhs, sizeof(int128_t));
    std::memcpy(&v_parts, &rhs, sizeof(int128_t));

    std::uint32_t w[2 * width] {};

    for (std::size_t j {}; j < width; ++j)
    {
        std::uint64_t t {};
        for (std::size_t i {}; i < width; ++i)
        {
            t += static_cast<std::uint64_t>(u_parts[i]) * v_parts[j] + w[i + j];
            w[i + j] = static_cast<std::uint32_t>(t);
            t >>= 32u;
        }
        w[j + width] = static_cast<std::uint32_t>(t);
    }

    int128_t result {};
    std::memcpy(&result, &w, sizeof(int128_t));

    if (negative)
    {
        result.low = ~result.low + 1;
        result.high = ~result.high + (result.low == 0);
    }

    return result;
}

#ifdef __GNUC__
#  pragma GCC diagnostic pop
#endif

#endif

BOOST_INT128_FORCE_INLINE constexpr int128_t default_mul(const int128_t lhs, const int128_t rhs) noexcept
{
    #if (defined(__aarch64__) || defined(__x86_64__) || defined(__PPC__) || defined(__powerpc__)) && defined(__GNUC__) && !defined(__clang__) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION)

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return library_mul(lhs, rhs);
    }
    else
    {
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wclass-memaccess"

        detail::builtin_i128 new_lhs {};
        detail::builtin_i128 new_rhs {};

        std::memcpy(&new_lhs, &lhs, sizeof(int128_t));
        std::memcpy(&new_rhs, &rhs, sizeof(int128_t));

        const auto res {new_lhs * new_rhs};
        int128_t library_res {};

        std::memcpy(&library_res, &res, sizeof(detail::builtin_i128));

        return library_res;

        #pragma GCC diagnostic pop
    }

    #elif defined(__i386__) && defined(__SSE2__) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION)

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return library_mul(lhs, rhs);
    }
    else
    {
        return sse_mul(lhs, rhs);
    }

    #elif defined(_M_AMD64) && !defined(__GNUC__) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION)

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(rhs))
    {
        return library_mul(lhs, rhs);
    }
    else
    {
        return msvc_amd64_mul(lhs, rhs);
    }

    #elif (defined(_M_IX86) || defined(_M_ARM)) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION)

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(rhs))
    {
        return library_mul(lhs, rhs);
    }
    else
    {
        return msvc_32_mul(lhs, rhs);
    }

    #else

    return library_mul(lhs, rhs);

    #endif
}

BOOST_INT128_FORCE_INLINE constexpr int128_t default_mul(const int128_t lhs, const std::uint64_t rhs) noexcept
{
    const auto low_res {lhs.low * rhs};

    const auto a_lo {lhs.low & UINT32_MAX};
    const auto a_high {lhs.low >> 32U};
    const auto b_lo {rhs & UINT32_MAX};
    const auto b_high {rhs >> 32U};

    const auto lo_lo {a_lo * b_lo};
    const auto lo_hi {a_lo * b_high};
    const auto hi_lo {a_high * b_lo};
    const auto hi_hi {a_high * b_high};

    const auto mid {(lo_lo >> 32U) + (lo_hi & UINT32_MAX) + (hi_lo & UINT32_MAX)};

    const auto carry {hi_hi + (lo_hi >> 32) + (hi_lo >> 32) + (mid >> 32)};

    const auto high_res {lhs.high * static_cast<std::int64_t>(rhs) + static_cast<std::int64_t>(carry)};

    return {high_res, low_res};
}

BOOST_INT128_FORCE_INLINE constexpr int128_t default_mul(const int128_t lhs, const std::uint32_t rhs) noexcept
{
    const auto low_res {lhs.low * rhs};

    const auto a_hi {lhs.low >> 32U};
    const auto hi_lo {a_hi * rhs};

    const auto high_res {lhs.high * static_cast<std::int64_t>(rhs) + static_cast<std::int64_t>(hi_lo)};

    return {high_res, low_res};
}

} // namespace detail

constexpr int128_t operator*(const int128_t lhs, const int128_t rhs) noexcept
{
    return detail::default_mul(lhs, rhs);
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr int128_t operator*(const int128_t lhs, const UnsignedInteger rhs) noexcept
{
    using local_eval_type = detail::evaluation_type_t<UnsignedInteger>;

    return detail::default_mul(lhs, static_cast<local_eval_type>(rhs));
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr int128_t operator*(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    using local_eval_type = detail::evaluation_type_t<UnsignedInteger>;

    return detail::default_mul(rhs, static_cast<local_eval_type>(lhs));
}

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr int128_t operator*(const int128_t lhs, const SignedInteger rhs) noexcept
{
    return rhs < 0 ? -detail::default_mul(lhs, -static_cast<std::uint64_t>(rhs)) :
                      detail::default_mul(lhs, static_cast<std::uint64_t>(rhs));
}

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr int128_t operator*(const SignedInteger lhs, const int128_t rhs) noexcept
{
    return lhs < 0 ? -detail::default_mul(rhs, -static_cast<std::uint64_t>(lhs)) :
                      detail::default_mul(rhs, static_cast<std::uint64_t>(lhs));
}

#ifdef BOOST_INT128_HAS_INT128

constexpr int128_t operator*(const int128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return static_cast<int128_t>(static_cast<detail::builtin_i128>(lhs) * rhs);
}

constexpr int128_t operator*(const detail::builtin_u128 lhs, const int128_t rhs) noexcept
{
    return static_cast<int128_t>(static_cast<detail::builtin_i128>(rhs) * lhs);
}

constexpr int128_t operator*(const int128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return detail::default_mul(lhs, static_cast<int128_t>(rhs));
}

constexpr int128_t operator*(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    return detail::default_mul(rhs, static_cast<int128_t>(lhs));
}

#endif // BOOST_INT128_HAS_INT128

template <BOOST_INT128_INTEGER_CONCEPT>
constexpr int128_t& int128_t::operator*=(const Integer rhs) noexcept
{
    *this = *this * rhs;
    return *this;
}

constexpr int128_t& int128_t::operator*=(const int128_t rhs) noexcept
{
    *this = *this * rhs;
    return *this;
}

//=====================================
// Division Operator
//=====================================

#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wassume"
#endif

namespace detail {

// See: The Art of Computer Programming Volume 2 (Semi-numerical algorithms) section 4.3.1
// Algorithm D: Division of Non-negative integers
template <std::size_t u_size, std::size_t v_size, std::size_t q_size>
constexpr void knuth_div(std::uint32_t (&u)[u_size],
                         const std::uint32_t (&v)[v_size],
                         std::uint32_t (&q)[q_size]) noexcept
{
    static_assert(u_size > v_size, "Dividend must be larger that the divisior");
    static_assert(q_size >= (u_size - v_size), "Quotient array must be at least u_size - v_size");

    constexpr auto n {static_cast<int>(v_size)};
    constexpr auto m {static_cast<int>(u_size - v_size - 1)};

    // D.2 and 7
    for (int j {m}; j >= 0; --j)
    {
        const auto next_two_digits {(static_cast<std::uint64_t>(u[j + n]) << 32) | u[j + n - 1]};

        std::uint64_t qhat {next_two_digits / v[n - 1]};
        std::uint64_t rhat {next_two_digits % v[n - 1]};

        // D.3
        // Test quotient and correct if needed
        while ((qhat >> 32) != 0 ||
               qhat * static_cast<std::uint64_t>(v[n - 2]) > ((rhat << 32) | u[j + n - 2]))
        {
            --qhat;
            rhat += v[n - 1];
            if ((rhat >> 32) != 0)
            {
                break;
            }
        }

        std::int64_t k {};

        // D.4
        for (int i {}; i < n; ++i)
        {
            const auto prod {qhat * static_cast<std::uint64_t>(v[i])};
            const auto prod_low {static_cast<std::uint32_t>(prod)};
            const auto prod_high {static_cast<std::uint32_t>(prod >> 32)};

            const auto t {static_cast<std::int64_t>(u[i + j]) - k - static_cast<std::int64_t>(prod_low)};
            k = static_cast<std::int64_t>(prod_high) - (t < 0 ? 1 : 0);
            u[i + j] = static_cast<std::uint32_t>(t);
        }

        // D.5
        const auto t {static_cast<std::int64_t>(u[j + n]) - k};
        u[j + n] = static_cast<std::uint32_t>(t);

        // D.6
        q[j] = static_cast<std::uint32_t>(qhat);
        if (t < 0)
        {
            --q[j];
            std::uint32_t k_prime {};
            for (int i {}; i < n; ++i)
            {
                const auto sum {static_cast<std::uint64_t>(u[i + j]) + v[i] + k_prime};
                u[i + j] = static_cast<std::uint32_t>(sum);
                k = static_cast<std::uint32_t>(sum >> 32U);
            }

            u[j + n] += k_prime;
        }
    }
}

BOOST_INT128_FORCE_INLINE constexpr int128_t single_word_div(const int128_t& lhs, const std::uint32_t rhs, int128_t& remainder) noexcept
{
    BOOST_INT128_ASSUME(rhs != 0);

    int128_t quotient {};

    const auto rhs32 = static_cast<std::uint32_t>(rhs);
    auto abs_lhs {abs(lhs)};

    auto current = static_cast<std::uint64_t>(abs_lhs.high >> 32U);
    quotient.high = static_cast<std::uint64_t>(static_cast<std::uint64_t>(static_cast<std::uint32_t>(current / rhs32)) << 32U);
    remainder.low = static_cast<std::uint64_t>(current % rhs32);

    current = static_cast<std::uint64_t>(remainder.low << 32U) | static_cast<std::uint32_t>(abs_lhs.high);
    quotient.high |= static_cast<std::uint32_t>(current / rhs32);
    remainder.low = static_cast<std::uint64_t>(current % rhs32);

    current = static_cast<std::uint64_t>(remainder.low << 32U) | static_cast<std::uint32_t>(abs_lhs.low >> 32U);
    quotient.low = static_cast<std::uint64_t>(static_cast<std::uint64_t>(static_cast<std::uint32_t>(current / rhs32)) << 32U);
    remainder.low = static_cast<std::uint64_t>(current % rhs32);

    current = remainder.low << 32U | static_cast<std::uint32_t>(abs_lhs.low);
    quotient.low |= static_cast<std::uint32_t>(current / rhs32);
    remainder.low = static_cast<std::uint32_t>(current % rhs32);

    return quotient;
}

BOOST_INT128_FORCE_INLINE constexpr int128_t single_word_div(const int128_t& lhs, const std::uint64_t rhs, int128_t& remainder) noexcept
{
    BOOST_INT128_ASSUME(rhs != 0);

    int128_t quotient {};

    // If rhs is greater than 2^32, then the result is trivial to find.
    if (rhs >= UINT32_MAX)
    {
        #if !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION) && defined(_M_AMD64) && !defined(__GNUC__) && !defined(__clang__)
        if (!BOOST_INT128_IS_CONSTANT_EVALUATED(rhs))
        {
            quotient.low = _udiv128(static_cast<std::uint64_t>(lhs.high), lhs.low, rhs, &remainder.low);
        }
        else
        #endif
        {
            if (lhs < rhs)
            {
                remainder = lhs;
            }
            else
            {
                quotient.low = lhs.high / rhs;
                remainder.low = lhs.low;
                remainder.high = lhs.high % rhs;
            }

            const auto n_hi {static_cast<std::uint64_t>(remainder.high)};
            const auto n_lo {remainder.low};

            const auto temp_quo {((n_hi << 32) | (n_lo >> 32)) / rhs};
            const auto temp_rem {((n_hi << 32) | (n_lo >> 32)) % rhs};

            quotient.low = (quotient.low << 32) | temp_quo;

            const auto final_dividend {(temp_rem << 32) | (n_lo & UINT32_MAX)};
            const auto final_quo {final_dividend / rhs};
            const auto final_rem {final_dividend % rhs};

            quotient.low = (quotient.low << 32) | final_quo;
            remainder.low = final_rem;
            remainder.high = 0;
        }
    }
    else
    {
        quotient = single_word_div(lhs, static_cast<std::uint32_t>(rhs), remainder);
    }

    return quotient;
}

template <bool need_mod>
constexpr int128_t default_div(const int128_t& lhs, const int128_t& rhs, int128_t& remainder) noexcept
{
    BOOST_INT128_ASSUME(rhs > 0);

    const auto negative_res {static_cast<bool>((lhs.high < 0) ^ (rhs.high < 0))};

    const auto abs_lhs {abs(lhs)};
    const auto abs_rhs {abs(rhs)};

    std::uint32_t numerator[5] = {
        static_cast<std::uint32_t>(abs_lhs.low),
        static_cast<std::uint32_t>(abs_lhs.low >> 32U),
        static_cast<std::uint32_t>(static_cast<std::uint64_t>(abs_lhs.high)),
        static_cast<std::uint32_t>(static_cast<std::uint64_t>(abs_lhs.high) >> 32U),
        0
    };

    std::uint32_t denominator[4] = {
        static_cast<std::uint32_t>(abs_rhs.low),
        static_cast<std::uint32_t>(abs_rhs.low >> 32U),
        static_cast<std::uint32_t>(static_cast<std::uint64_t>(abs_rhs.high)),
        static_cast<std::uint32_t>(static_cast<std::uint64_t>(abs_rhs.high) >> 32U)
    };

    std::size_t num_size {4};
    while (num_size > 0 && numerator[num_size - 1] == 0)
    {
        --num_size;
    }

    if (BOOST_INT128_UNLIKELY(num_size == 0))
    {
        return int128_t{0, 0};
    }

    std::size_t den_size {4};
    while (den_size > 0 && denominator[den_size - 1] == 0)
    {
        --den_size;
    }

    // Trivial Cases
    if (den_size > num_size)
    {
        // Divisor > dividend
        remainder = rhs;
        return int128_t{0, 0};
    }
    else if (den_size == 1)
    {
        // Division by a single word
        int128_t quotient {single_word_div(lhs, static_cast<std::uint64_t>(rhs), remainder)};

        return negative_res ? -quotient : quotient;
    }

    // General Case: We need to apply Knuth's normalization in preparation to use 4.3.1.D above
    int d {};
    if (denominator[den_size - 1] != 0)
    {
        d = countl_zero(denominator[den_size - 1]);
    }

    std::uint32_t u_norm[5] {};
    std::uint32_t v_norm[4] {};

    // Apply shift
    if (d > 0)
    {
        std::uint32_t carry {};
        for (std::size_t i {}; i < den_size; ++i)
        {
            v_norm[i] = (denominator[i] << d) | (carry >> (32 - d));
            carry = denominator[i];
        }

        carry = 0;
        for (std::size_t i {}; i < num_size; ++i)
        {
            u_norm[i] = (numerator[i] << d) | (carry >> (32 - d));
            carry = numerator[i];
        }

        u_norm[num_size] = carry >> (32 - d);
    }
    else
    {
        for (std::size_t i {}; i < den_size; ++i)
        {
            v_norm[i] = denominator[i];
        }
        for (std::size_t i {}; i < num_size; ++i)
        {
            u_norm[i] = numerator[i];
        }
    }

    std::uint32_t q_result[4] {};
    const auto actual_den_size {den_size > 0 ? den_size : 1};
    const auto actual_num_size {num_size + 1};

    if (actual_num_size > actual_den_size)
    {
        std::uint32_t q_temp[4] {};
        knuth_div(u_norm, v_norm, q_temp);

        for (std::size_t i {}; i < 4 && i < (actual_num_size - actual_den_size + 1); ++i)
        {
            q_result[i] = q_temp[i];
        }
    }

    const auto q_low {(static_cast<std::uint64_t>(q_result[1]) << 32) | q_result[0]};
    const auto q_high {(static_cast<std::int64_t>(q_result[3]) << 32) | q_result[2]};

    int128_t quotient {q_high, q_low};

    if (negative_res)
    {
        quotient = -quotient;
    }

    // De-normalize the remainder if we need it
    // In the pure division case we can skip over this entirely (at run time or hopefully compile time)
    BOOST_INT128_IF_CONSTEXPR (need_mod)
    {
        std::uint32_t r_result[4] {};
        if (d > 0)
        {
            std::uint32_t carry {};
            for (int i {static_cast<int>(actual_den_size - 1)}; i >= 0; --i)
            {
                const auto temp {static_cast<std::uint64_t>(u_norm[i]) | (static_cast<std::uint64_t>(carry) << 32)};
                r_result[i] = static_cast<std::uint32_t>(temp >> d);
                carry = static_cast<std::uint32_t>(temp) & ((1U << d) - 1);
            }
        }
        else
        {
            for (std::size_t i {}; i < actual_den_size; ++i)
            {
                r_result[i] = u_norm[i];
            }
        }

        const auto r_low {(static_cast<std::uint64_t>(r_result[1]) << 32) | r_result[0]};
        const auto r_high {(static_cast<std::int64_t>(r_result[3]) << 32) | r_result[2]};
        remainder = {r_high, r_low};

        if (lhs.high < 0 && (r_high != 0 || r_low != 0))
        {
            remainder = -remainder;
        }
    }

    return quotient;
}

} // namespace detail

constexpr int128_t operator/(const int128_t lhs, const int128_t rhs) noexcept
{
    BOOST_INT128_ASSUME(rhs != 0);
    int128_t remainder {};
    return detail::default_div<false>(lhs, rhs, remainder);
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr int128_t operator/(const int128_t lhs, const UnsignedInteger rhs) noexcept
{
    BOOST_INT128_ASSUME(rhs != 0);
    using eval_type = detail::evaluation_type_t<UnsignedInteger>;

    int128_t remainder {};

    return detail::single_word_div(lhs, static_cast<eval_type>(rhs), remainder);
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr int128_t operator/(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    if (rhs.high != 0 && rhs.high != -1)
    {
        return {0,0};
    }
    else
    {
        auto abs_rhs {abs(rhs)};
        const auto res {static_cast<std::uint64_t>(lhs) / abs_rhs.low};
        return int128_t{rhs.high, res};
    }
}

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr int128_t operator/(const int128_t lhs, const SignedInteger rhs) noexcept
{
    BOOST_INT128_ASSUME(rhs != 0);
    using eval_type = detail::evaluation_type_t<SignedInteger>;

    int128_t remainder {};

    const auto negative_res {static_cast<bool>((lhs.high < 0) ^ (rhs < 0))};
    const auto abs_rhs {rhs < 0 ? -rhs : rhs};
    const auto quo {detail::single_word_div(lhs, static_cast<eval_type>(abs_rhs), remainder)};

    return negative_res ? -quo : quo;
}

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr int128_t operator/(const SignedInteger lhs, const int128_t rhs) noexcept
{
    if (rhs.high != 0 && rhs.high != -1)
    {
        return {0,0};
    }
    else
    {
        const auto negative_res {static_cast<bool>((rhs.high < 0) ^ (lhs < 0))};
        const auto abs_rhs {abs(rhs)};
        const auto abs_lhs {lhs < 0 ? -lhs : lhs};
        const int128_t res {0, static_cast<std::uint64_t>(abs_lhs) / abs_rhs.low};

        return negative_res ? -res : res;
    }
}

#ifdef BOOST_INT128_HAS_INT128

constexpr int128_t operator/(const int128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return static_cast<int128_t>(static_cast<detail::builtin_i128>(lhs) / rhs);
}

constexpr int128_t operator/(const detail::builtin_u128 lhs, const int128_t rhs) noexcept
{
    return static_cast<int128_t>(lhs / static_cast<detail::builtin_i128>(rhs));
}

constexpr int128_t operator/(const int128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return static_cast<int128_t>(static_cast<detail::builtin_i128>(lhs) / rhs);
}

constexpr int128_t operator/(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    return static_cast<int128_t>(lhs / static_cast<detail::builtin_i128>(rhs));
}

#endif // BOOST_INT128_HAS_INT128

template <BOOST_INT128_INTEGER_CONCEPT>
constexpr int128_t& int128_t::operator/=(const Integer rhs) noexcept
{
    *this = *this / rhs;
    return *this;
}

constexpr int128_t& int128_t::operator/=(const int128_t rhs) noexcept
{
    *this = *this / rhs;
    return *this;
}

#if defined(__clang__)
#  pragma clang diagnostic pop
#elif defined(__GNUC__)
#  pragma GCC diagnostic pop
#endif

} // namespace int128
} // namespace boost

#endif // BOOST_INT128_DETAIL_INT128_HPP
