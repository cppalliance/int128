// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_DETAIL_UINT128_HPP
#define BOOST_INT128_DETAIL_UINT128_HPP

#include <boost/int128/detail/config.hpp>
#include <boost/int128/detail/traits.hpp>
#include <boost/int128/detail/constants.hpp>
#include <boost/int128/detail/clz.hpp>
#include <boost/int128/detail/knuth_mul.hpp>
#include <cstdint>
#include <cstring>
#include <climits>

namespace boost {
namespace int128 {

struct
    #ifdef BOOST_INT128_HAS_INT128
    alignas(alignof(detail::builtin_u128))
    #else
    alignas(16)
    #endif
uint128_t
{
    #if BOOST_INT128_ENDIAN_LITTLE_BYTE
    std::uint64_t low {};
    std::uint64_t high {};
    #else

    #ifdef __GNUC__
    #  pragma GCC diagnostic push
    #  pragma GCC diagnostic ignored "-Wreorder"
    #endif

    std::uint64_t high {};
    std::uint64_t low {};

    #ifdef __GNUC__
    #  pragma GCC diagnostic pop
    #endif

    #endif // BOOST_INT128_ENDIAN_LITTLE_BYTE

    // Defaulted basic construction
    constexpr uint128_t() noexcept = default;
    constexpr uint128_t(const uint128_t&) noexcept = default;
    constexpr uint128_t(uint128_t&&) noexcept = default;
    constexpr uint128_t& operator=(const uint128_t&) noexcept = default;
    constexpr uint128_t& operator=(uint128_t&&) noexcept = default;

    // Requires conversion file to be implemented
    constexpr uint128_t(const int128_t& v) noexcept;

    // Construct from integral types
    #if BOOST_INT128_ENDIAN_LITTLE_BYTE

    constexpr uint128_t(const std::uint64_t hi, const std::uint64_t lo) noexcept : low {lo}, high {hi} {}

    template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
    constexpr uint128_t(const SignedInteger v) noexcept : low {static_cast<std::uint64_t>(v)}, high {v < 0 ? UINT64_MAX : UINT64_C(0)} {}

    template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
    constexpr uint128_t(const UnsignedInteger v) noexcept : low {static_cast<std::uint64_t>(v)}, high {} {}

    #ifdef BOOST_INT128_HAS_INT128

    constexpr uint128_t(const detail::builtin_i128 v) noexcept :
        low {static_cast<std::uint64_t>(v)},
        high {static_cast<std::uint64_t>(static_cast<detail::builtin_u128>(v) >> 64U)} {}

    constexpr uint128_t(const detail::builtin_u128 v) noexcept :
        low {static_cast<std::uint64_t>(v)},
        high {static_cast<std::uint64_t>(v >> 64U)} {}

    #endif // BOOST_INT128_HAS_INT128

    #else // Big endian

    constexpr uint128_t(const std::uint64_t hi, const std::uint64_t lo) noexcept : high {hi}, low {lo} {}

    template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
    constexpr uint128_t(const SignedInteger v) noexcept : high {v < 0 ? UINT64_MAX : UINT64_C(0)}, low {static_cast<std::uint64_t>(v)} {}

    template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
    constexpr uint128_t(const UnsignedInteger v) noexcept : high {}, low {static_cast<std::uint64_t>(v)} {}

    #ifdef BOOST_INT128_HAS_INT128

    constexpr uint128_t(const detail::builtin_i128 v) noexcept :
        high {static_cast<std::uint64_t>(static_cast<detail::builtin_u128>(v) >> 64U)},
        low {static_cast<std::uint64_t>(v)} {}

    constexpr uint128_t(const detail::builtin_u128 v) noexcept :
        high {static_cast<std::uint64_t>(v >> 64U)},
        low {static_cast<std::uint64_t>(v)} {}

    #endif // BOOST_INT128_HAS_INT128

    #endif // BOOST_INT128_ENDIAN_LITTLE_BYTE

    // Integer conversion operators
    constexpr operator bool() const noexcept {return low || high; }

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

    // Compound OR
    template <BOOST_INT128_DEFAULTED_INTEGER_CONCEPT>
    constexpr uint128_t operator|=(Integer rhs) noexcept;

    constexpr uint128_t operator|=(uint128_t rhs) noexcept;

    // Compound AND
    template <BOOST_INT128_DEFAULTED_INTEGER_CONCEPT>
    constexpr uint128_t operator&=(Integer rhs) noexcept;

    constexpr uint128_t operator&=(uint128_t rhs) noexcept;

    // Compound XOR
    template <BOOST_INT128_DEFAULTED_INTEGER_CONCEPT>
    constexpr uint128_t operator^=(Integer rhs) noexcept;

    constexpr uint128_t operator^=(uint128_t rhs) noexcept;

    // Compound Left Shift
    template <BOOST_INT128_DEFAULTED_INTEGER_CONCEPT>
    constexpr uint128_t operator<<=(Integer rhs) noexcept;

    constexpr uint128_t operator<<=(uint128_t rhs) noexcept;

    // Compound Right Shift
    template <BOOST_INT128_DEFAULTED_INTEGER_CONCEPT>
    constexpr uint128_t operator>>=(Integer rhs) noexcept;

    constexpr uint128_t operator>>=(uint128_t rhs) noexcept;

    constexpr uint128_t& operator++() noexcept;
    constexpr uint128_t& operator++(int) noexcept;
    constexpr uint128_t& operator--() noexcept;
    constexpr uint128_t& operator--(int) noexcept;

    // Compound Addition
    template <BOOST_INT128_DEFAULTED_INTEGER_CONCEPT>
    constexpr uint128_t& operator+=(Integer rhs) noexcept;

    constexpr uint128_t& operator+=(uint128_t rhs) noexcept;

    // Compound Subtraction
    template <BOOST_INT128_DEFAULTED_INTEGER_CONCEPT>
    constexpr uint128_t& operator-=(Integer rhs) noexcept;

    constexpr uint128_t& operator-=(uint128_t rhs) noexcept;

    // Compound Multiplication
    template <BOOST_INT128_DEFAULTED_INTEGER_CONCEPT>
    constexpr uint128_t& operator*=(Integer rhs) noexcept;

    constexpr uint128_t& operator*=(uint128_t rhs) noexcept;
};

//=====================================
// Absolute Value function
//=====================================

constexpr uint128_t abs(const uint128_t value) noexcept
{
    return value;
}

//=====================================
// Float Conversion Operators
//=====================================

// The most correct way to do this would be std::ldexp(static_cast<T>(high), 64) + static_cast<T>(low);
// Since std::ldexp is not constexpr until C++23 we can work around this by multiplying the high word
// by 0xFFFFFFFF in order to generally replicate what ldexp is doing in the constexpr context.
// We also avoid pulling in <quadmath.h> for the __float128 case where we would need ldexpq

constexpr uint128_t::operator float() const noexcept
{
    return static_cast<float>(high) * detail::offset_value_v<float> + static_cast<float>(low);
}

constexpr uint128_t::operator double() const noexcept
{
    return static_cast<double>(high) * detail::offset_value_v<double> + static_cast<double>(low);
}

constexpr uint128_t::operator long double() const noexcept
{
    return static_cast<long double>(high) * detail::offset_value_v<long double> + static_cast<long double>(low);
}

//=====================================
// Unary Operators
//=====================================

constexpr uint128_t operator+(const uint128_t value) noexcept
{
    return value;
}

constexpr uint128_t operator-(const uint128_t value) noexcept
{
    return {~value.high + static_cast<std::uint64_t>(value.low == UINT64_C(0)), ~value.low + UINT64_C(1)};
}

//=====================================
// Equality Operators
//=====================================

constexpr bool operator==(const uint128_t lhs, const bool rhs) noexcept
{
    return lhs.high == UINT64_C(0) && lhs.low == static_cast<std::uint64_t>(rhs);
}

constexpr bool operator==(const bool lhs, const uint128_t rhs) noexcept
{
    return rhs.high == UINT64_C(0) && rhs.low == static_cast<std::uint64_t>(lhs);
}

#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wsign-conversion"
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wsign-conversion"
#endif

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr bool operator==(const uint128_t lhs, const SignedInteger rhs) noexcept
{
    return rhs >= 0 && lhs.high == UINT64_C(0) && lhs.low == static_cast<std::uint64_t>(rhs);
}

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr bool operator==(const SignedInteger lhs, const uint128_t rhs) noexcept
{
    return lhs >= 0 && rhs.high == UINT64_C(0) && rhs.low == static_cast<std::uint64_t>(lhs);
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr bool operator==(const uint128_t lhs, const UnsignedInteger rhs) noexcept
{
    return lhs.high == UINT64_C(0) && lhs.low == static_cast<std::uint64_t>(rhs);
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr bool operator==(const UnsignedInteger lhs, const uint128_t rhs) noexcept
{
    return rhs.high == UINT64_C(0) && rhs.low == static_cast<std::uint64_t>(lhs);
}

constexpr bool operator==(const uint128_t lhs, const uint128_t rhs) noexcept
{
    // Intel and ARM like the values in opposite directions

    #if defined(__aarch64__) || defined(_M_ARM64) || defined(_M_AMD64)

    return lhs.low == rhs.low && lhs.high == rhs.high;

    #elif (defined(__i386__) || defined(_M_IX86) || defined(_M_AMD64)) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION) && defined(__SSE2__)

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return lhs.low == rhs.low && lhs.high == rhs.high;
    }
    else
    {
        __m128i a = _mm_load_si128(reinterpret_cast<const __m128i*>(&lhs));
        __m128i b = _mm_load_si128(reinterpret_cast<const __m128i*>(&rhs));
        __m128i cmp = _mm_cmpeq_epi32(a, b);

        return _mm_movemask_ps(_mm_castsi128_ps(cmp)) == 0xF;
    }

    #else

    return lhs.high == rhs.high && lhs.low == rhs.low;

    #endif
}

#ifdef BOOST_INT128_HAS_INT128

constexpr bool operator==(const uint128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs == static_cast<uint128_t>(rhs);
}

constexpr bool operator==(const detail::builtin_i128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) == rhs;
}

constexpr bool operator==(const uint128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return lhs == static_cast<uint128_t>(rhs);
}

constexpr bool operator==(const detail::builtin_u128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) == rhs;
}

#endif // BOOST_INT128_HAS_INT128

//=====================================
// Inequality Operators
//=====================================

constexpr bool operator!=(const uint128_t lhs, const bool rhs) noexcept
{
    return lhs.high != UINT64_C(0) || lhs.low != static_cast<std::uint64_t>(rhs);
}

constexpr bool operator!=(const bool lhs, const uint128_t rhs) noexcept
{
    return rhs.high != UINT64_C(0) || rhs.low != static_cast<std::uint64_t>(lhs);
}

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr bool operator!=(const uint128_t lhs, const SignedInteger rhs) noexcept
{
    return rhs < 0 || lhs.high != UINT64_C(0) || lhs.low != static_cast<std::uint64_t>(rhs);
}

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr bool operator!=(const SignedInteger lhs, const uint128_t rhs) noexcept
{
    return lhs < 0 || rhs.high != UINT64_C(0) || rhs.low != static_cast<std::uint64_t>(lhs);
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr bool operator!=(const uint128_t lhs, const UnsignedInteger rhs) noexcept
{
    return lhs.high != UINT64_C(0) || lhs.low != static_cast<std::uint64_t>(rhs);
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr bool operator!=(const UnsignedInteger lhs, const uint128_t rhs) noexcept
{
    return rhs.high != UINT64_C(0) || rhs.low != static_cast<std::uint64_t>(lhs);
}

constexpr bool operator!=(const uint128_t lhs, const uint128_t rhs) noexcept
{
    #if defined(__aarch64__) || defined(_M_ARM64) || defined(_M_AMD64)

    return lhs.low != rhs.low || lhs.high != rhs.high;

    #elif (defined(__i386__) || defined(_M_IX86)) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION) && defined(__SSE2__)

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return lhs.low == rhs.low && lhs.high == rhs.high;
    }
    else
    {
        __m128i a = _mm_load_si128(reinterpret_cast<const __m128i*>(&lhs));
        __m128i b = _mm_load_si128(reinterpret_cast<const __m128i*>(&rhs));
        __m128i cmp = _mm_cmpeq_epi32(a, b);

        return _mm_movemask_ps(_mm_castsi128_ps(cmp)) != 0xF;
    }

    #else

    return lhs.high != rhs.high || lhs.low != rhs.low;

    #endif
}

#ifdef BOOST_INT128_HAS_INT128

constexpr bool operator!=(const uint128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs != static_cast<uint128_t>(rhs);
}

constexpr bool operator!=(const detail::builtin_i128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) != rhs;
}

constexpr bool operator!=(const uint128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return lhs != static_cast<uint128_t>(rhs);
}

constexpr bool operator!=(const detail::builtin_u128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) != rhs;
}

#endif // BOOST_INT128_HAS_INT128

//=====================================
// Less than Operators
//=====================================

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr bool operator<(const uint128_t lhs, const SignedInteger rhs) noexcept
{
    return rhs > 0 && lhs.high == UINT64_C(0) && lhs.low < static_cast<std::uint64_t>(rhs);
}

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr bool operator<(const SignedInteger lhs, const uint128_t rhs) noexcept
{
    return lhs < 0 || rhs.high > UINT64_C(0) || static_cast<std::uint64_t>(lhs) < rhs.low;
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr bool operator<(const uint128_t lhs, const UnsignedInteger rhs) noexcept
{
    return lhs.high == UINT64_C(0) && lhs.low < static_cast<std::uint64_t>(rhs);
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr bool operator<(const UnsignedInteger lhs, const uint128_t rhs) noexcept
{
    return rhs.high > UINT64_C(0) || static_cast<std::uint64_t>(lhs) < rhs.low;
}

constexpr bool operator<(const uint128_t lhs, const uint128_t rhs) noexcept
{
    // On ARM macs only with the clang compiler is casting to unsigned __int128 uniformly better (and seemingly cost free)
    #if defined(__clang__) && defined(BOOST_INT128_HAS_INT128)

    return static_cast<detail::builtin_u128>(lhs) < static_cast<detail::builtin_u128>(rhs);

    #elif defined(__x86_64__) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION) && defined(__GNUC__) && !defined(__clang__)

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return lhs.high == rhs.high ? lhs.low < rhs.low : lhs.high < rhs.high;
    }
    else
    {
        detail::builtin_u128 builtin_lhs {};
        detail::builtin_u128 builtin_rhs {};

        std::memcpy(&builtin_lhs, &lhs, sizeof(builtin_lhs));
        std::memcpy(&builtin_rhs, &rhs, sizeof(builtin_rhs));

        return builtin_lhs < builtin_rhs;
    }

    #elif (defined(__i386__) || defined(_M_IX86) || defined(__arm__)) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION)

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return lhs.high == rhs.high ? lhs.low < rhs.low : lhs.high < rhs.high;
    }
    else
    {
        const uint32_t* l = reinterpret_cast<const uint32_t*>(&lhs);
        const uint32_t* r = reinterpret_cast<const uint32_t*>(&rhs);

        if (l[3] != r[3])
        {
            return l[3] < r[3];
        }
        else if (l[2] != r[2])
        {
            return l[2] < r[2];
        }
        else if (l[1] != r[1])
        {
            return l[1] < r[1];
        }
        else
        {
            return l[0] < r[0];
        }
    }

    #else

    return lhs.high == rhs.high ? lhs.low < rhs.low : lhs.high < rhs.high;

    #endif
}

#ifdef BOOST_INT128_HAS_INT128

constexpr bool operator<(const uint128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs < static_cast<uint128_t>(rhs);
}

constexpr bool operator<(const detail::builtin_i128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) < rhs;
}

constexpr bool operator<(const uint128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return lhs < static_cast<uint128_t>(rhs);
}

constexpr bool operator<(const detail::builtin_u128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) < rhs;
}

#endif // BOOST_INT128_HAS_INT128

//=====================================
// Less Equal Operators
//=====================================

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr bool operator<=(const uint128_t lhs, const SignedInteger rhs) noexcept
{
    return rhs >= 0 && lhs.high == UINT64_C(0) && lhs.low <= static_cast<std::uint64_t>(rhs);
}

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr bool operator<=(const SignedInteger lhs, const uint128_t rhs) noexcept
{
    return lhs < 0 || rhs.high > UINT64_C(0) || static_cast<std::uint64_t>(lhs) <= rhs.low;
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr bool operator<=(const uint128_t lhs, const UnsignedInteger rhs) noexcept
{
    return lhs.high == UINT64_C(0) && lhs.low <= static_cast<std::uint64_t>(rhs);
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr bool operator<=(const UnsignedInteger lhs, const uint128_t rhs) noexcept
{
    return rhs.high > UINT64_C(0) || static_cast<std::uint64_t>(lhs) <= rhs.low;
}

constexpr bool operator<=(const uint128_t lhs, const uint128_t rhs) noexcept
{
    #if defined(__clang__) && defined(BOOST_INT128_HAS_INT128)

    return static_cast<detail::builtin_u128>(lhs) < static_cast<detail::builtin_u128>(rhs);

    #elif defined(__x86_64__) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION) && defined(__GNUC__) && !defined(__clang__)

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return lhs.high == rhs.high ? lhs.low <= rhs.low : lhs.high <= rhs.high;
    }
    else
    {
        detail::builtin_u128 builtin_lhs {};
        detail::builtin_u128 builtin_rhs {};

        std::memcpy(&builtin_lhs, &lhs, sizeof(builtin_lhs));
        std::memcpy(&builtin_rhs, &rhs, sizeof(builtin_rhs));

        return builtin_lhs < builtin_rhs;
    }

    #elif (defined(__i386__) || defined(_M_IX86) || defined(__arm__)) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION)

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return lhs.high == rhs.high ? lhs.low < rhs.low : lhs.high < rhs.high;
    }
    else
    {
        const uint32_t* l = reinterpret_cast<const uint32_t*>(&lhs);
        const uint32_t* r = reinterpret_cast<const uint32_t*>(&rhs);

        if (l[3] != r[3])
        {
            return l[3] < r[3];
        }
        else if (l[2] != r[2])
        {
            return l[2] < r[2];
        }
        else if (l[1] != r[1])
        {
            return l[1] < r[1];
        }
        else
        {
            return l[0] <= r[0];
        }
    }

    #else

    return lhs.high == rhs.high ? lhs.low <= rhs.low : lhs.high <= rhs.high;

    #endif
}

#ifdef BOOST_INT128_HAS_INT128

constexpr bool operator<=(const uint128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs <= static_cast<uint128_t>(rhs);
}

constexpr bool operator<=(const detail::builtin_i128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) <= rhs;
}

constexpr bool operator<=(const uint128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return lhs <= static_cast<uint128_t>(rhs);
}

constexpr bool operator<=(const detail::builtin_u128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) <= rhs;
}

#endif // BOOST_INT128_HAS_INT128

//=====================================
// Greater Than Operators
//=====================================

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr bool operator>(const uint128_t lhs, const SignedInteger rhs) noexcept
{
    return rhs < 0 || lhs.high > UINT64_C(0) || lhs.low > static_cast<std::uint64_t>(rhs);
}

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr bool operator>(const SignedInteger lhs, const uint128_t rhs) noexcept
{
    return lhs > 0 && rhs.high == UINT64_C(0) && static_cast<std::uint64_t>(lhs) > rhs.low;
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr bool operator>(const uint128_t lhs, const UnsignedInteger rhs) noexcept
{
    return lhs.high > UINT64_C(0) || lhs.low > static_cast<std::uint64_t>(rhs);
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr bool operator>(const UnsignedInteger lhs, const uint128_t rhs) noexcept
{
    return rhs.high == UINT64_C(0) && static_cast<std::uint64_t>(lhs) > rhs.low;
}

constexpr bool operator>(const uint128_t lhs, const uint128_t rhs) noexcept
{
    #if defined(__clang__) && defined(BOOST_INT128_HAS_INT128)

    return static_cast<detail::builtin_u128>(lhs) > static_cast<detail::builtin_u128>(rhs);

    #elif defined(__x86_64__) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION) && defined(__GNUC__) && !defined(__clang__)

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return lhs.high == rhs.high ? rhs.low < lhs.low : rhs.high < lhs.high;
    }
    else
    {
        detail::builtin_u128 builtin_lhs {};
        detail::builtin_u128 builtin_rhs {};

        std::memcpy(&builtin_lhs, &lhs, sizeof(builtin_lhs));
        std::memcpy(&builtin_rhs, &rhs, sizeof(builtin_rhs));

        return builtin_lhs > builtin_rhs;
    }

    #elif (defined(__i386__) || defined(_M_IX86) || defined(__arm__)) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION)

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return lhs.high == rhs.high ? lhs.low < rhs.low : lhs.high < rhs.high;
    }
    else
    {
        const uint32_t* l = reinterpret_cast<const uint32_t*>(&lhs);
        const uint32_t* r = reinterpret_cast<const uint32_t*>(&rhs);

        if (l[3] != r[3])
        {
            return l[3] > r[3];
        }
        else if (l[2] != r[2])
        {
            return l[2] > r[2];
        }
        else if (l[1] != r[1])
        {
            return l[1] > r[1];
        }
        else
        {
            return l[0] > r[0];
        }
    }

    #else

    return lhs.high == rhs.high ? rhs.low < lhs.low : rhs.high < lhs.high;

    #endif
}

#ifdef BOOST_INT128_HAS_INT128

constexpr bool operator>(const uint128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs > static_cast<uint128_t>(rhs);
}

constexpr bool operator>(const detail::builtin_i128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) > rhs;
}

constexpr bool operator>(const uint128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return lhs > static_cast<uint128_t>(rhs);
}

constexpr bool operator>(const detail::builtin_u128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) > rhs;
}

#endif // BOOST_INT128_HAS_INT128

//=====================================
// Greater-equal Operators
//=====================================

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr bool operator>=(const uint128_t lhs, const SignedInteger rhs) noexcept
{
    return rhs < 0 || lhs.high > UINT64_C(0) || lhs.low >= static_cast<std::uint64_t>(rhs);
}

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr bool operator>=(const SignedInteger lhs, const uint128_t rhs) noexcept
{
    return lhs > 0 && rhs.high == UINT64_C(0) && static_cast<std::uint64_t>(lhs) >= rhs.low;
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr bool operator>=(const uint128_t lhs, const UnsignedInteger rhs) noexcept
{
    return lhs.high > UINT64_C(0) || lhs.low >= static_cast<std::uint64_t>(rhs);
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr bool operator>=(const UnsignedInteger lhs, const uint128_t rhs) noexcept
{
    return rhs.high == UINT64_C(0) && static_cast<std::uint64_t>(lhs) >= rhs.low;
}

constexpr bool operator>=(const uint128_t lhs, const uint128_t rhs) noexcept
{
    #if defined(__clang__) && defined(BOOST_INT128_HAS_INT128)

    return static_cast<detail::builtin_u128>(lhs) >= static_cast<detail::builtin_u128>(rhs);

    #elif defined(__x86_64__) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION) && defined(__GNUC__) && !defined(__clang__)

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return lhs.high == rhs.high ? rhs.low <= lhs.low : rhs.high <= lhs.high;
    }
    else
    {
        detail::builtin_u128 builtin_lhs {};
        detail::builtin_u128 builtin_rhs {};

        std::memcpy(&builtin_lhs, &lhs, sizeof(builtin_lhs));
        std::memcpy(&builtin_rhs, &rhs, sizeof(builtin_rhs));

        return builtin_lhs > builtin_rhs;
    }

    #elif (defined(__i386__) || defined(_M_IX86) || defined(__arm__)) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION)

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return lhs.high == rhs.high ? lhs.low < rhs.low : lhs.high < rhs.high;
    }
    else
    {
        const uint32_t* l = reinterpret_cast<const uint32_t*>(&lhs);
        const uint32_t* r = reinterpret_cast<const uint32_t*>(&rhs);

        if (l[3] != r[3])
        {
            return l[3] > r[3];
        }
        else if (l[2] != r[2])
        {
            return l[2] > r[2];
        }
        else if (l[1] != r[1])
        {
            return l[1] > r[1];
        }
        else
        {
            return l[0] >= r[0];
        }
    }

    #else

    return lhs.high == rhs.high ? rhs.low <= lhs.low : rhs.high <= lhs.high;

    #endif
}

#ifdef BOOST_INT128_HAS_INT128

constexpr bool operator>=(const uint128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs >= static_cast<uint128_t>(rhs);
}

constexpr bool operator>=(const detail::builtin_i128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) >= rhs;
}

constexpr bool operator>=(const uint128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return lhs >= static_cast<uint128_t>(rhs);
}

constexpr bool operator>=(const detail::builtin_u128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) >= rhs;
}

#endif // BOOST_INT128_HAS_INT128

//=====================================
// Not Operator
//=====================================

constexpr uint128_t operator~(const uint128_t rhs) noexcept
{
    return {~rhs.high, ~rhs.low};
}

//=====================================
// OR Operator
//=====================================

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr uint128_t operator|(const uint128_t lhs, const SignedInteger rhs) noexcept
{
    return {lhs.high | (rhs < 0 ? ~UINT64_C(0) : UINT64_C(0)), lhs.low | static_cast<std::uint64_t>(rhs)};
}

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr uint128_t operator|(const SignedInteger lhs, const uint128_t rhs) noexcept
{
    return {rhs.high | (lhs < 0 ? ~UINT64_C(0) : UINT64_C(0)), rhs.low | static_cast<std::uint64_t>(lhs)};
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr uint128_t operator|(const uint128_t lhs, const UnsignedInteger rhs) noexcept
{
    return {lhs.high, lhs.low | static_cast<std::uint64_t>(rhs)};
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr uint128_t operator|(const UnsignedInteger lhs, const uint128_t rhs) noexcept
{
    return {rhs.high, rhs.low | static_cast<std::uint64_t>(lhs)};
}

constexpr uint128_t operator|(const uint128_t lhs, const uint128_t rhs) noexcept
{
    return {lhs.high | rhs.high, lhs.low | rhs.low};
}

#ifdef BOOST_INT128_HAS_INT128

constexpr uint128_t operator|(const uint128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs | static_cast<uint128_t>(rhs);
}

constexpr uint128_t operator|(const detail::builtin_i128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) | rhs;
}

constexpr uint128_t operator|(const uint128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return lhs | static_cast<uint128_t>(rhs);
}

constexpr uint128_t operator|(const detail::builtin_u128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) | rhs;
}

#endif // BOOST_INT128_HAS_INT128

template <BOOST_INT128_INTEGER_CONCEPT>
constexpr uint128_t uint128_t::operator|=(const Integer rhs) noexcept
{
    *this = *this | rhs;
    return *this;
}

constexpr uint128_t uint128_t::operator|=(const uint128_t rhs) noexcept
{
    *this = *this | rhs;
    return *this;
}

//=====================================
// AND Operator
//=====================================

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr uint128_t operator&(const uint128_t lhs, const SignedInteger rhs) noexcept
{
    return {lhs.high & (rhs < 0 ? ~UINT64_C(0) : UINT64_C(0)), lhs.low & static_cast<std::uint64_t>(rhs)};
}

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr uint128_t operator&(const SignedInteger lhs, const uint128_t rhs) noexcept
{
    return {rhs.high & (lhs < 0 ? ~UINT64_C(0) : UINT64_C(0)), rhs.low & static_cast<std::uint64_t>(lhs)};
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr uint128_t operator&(const uint128_t lhs, const UnsignedInteger rhs) noexcept
{
    return {lhs.high, lhs.low & static_cast<std::uint64_t>(rhs)};
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr uint128_t operator&(const UnsignedInteger lhs, const uint128_t rhs) noexcept
{
    return {rhs.high, rhs.low & static_cast<std::uint64_t>(lhs)};
}

constexpr uint128_t operator&(const uint128_t lhs, const uint128_t rhs) noexcept
{
    return {lhs.high & rhs.high, lhs.low & rhs.low};
}

#ifdef BOOST_INT128_HAS_INT128

constexpr uint128_t operator&(const uint128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs & static_cast<uint128_t>(rhs);
}

constexpr uint128_t operator&(const detail::builtin_i128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) & rhs;
}

constexpr uint128_t operator&(const uint128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return lhs & static_cast<uint128_t>(rhs);
}

constexpr uint128_t operator&(const detail::builtin_u128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) & rhs;
}

#endif // BOOST_INT128_HAS_INT128

template <BOOST_INT128_INTEGER_CONCEPT>
constexpr uint128_t uint128_t::operator&=(const Integer rhs) noexcept
{
    *this = *this & rhs;
    return *this;
}

constexpr uint128_t uint128_t::operator&=(const uint128_t rhs) noexcept
{
    *this = *this & rhs;
    return *this;
}

//=====================================
// XOR Operator
//=====================================

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr uint128_t operator^(const uint128_t lhs, const SignedInteger rhs) noexcept
{
    return {lhs.high ^ (rhs < 0 ? ~UINT64_C(0) : UINT64_C(0)), lhs.low ^ static_cast<std::uint64_t>(rhs)};
}

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr uint128_t operator^(const SignedInteger lhs, const uint128_t rhs) noexcept
{
    return {rhs.high ^ (lhs < 0 ? ~UINT64_C(0) : UINT64_C(0)), rhs.low ^ static_cast<std::uint64_t>(lhs)};
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr uint128_t operator^(const uint128_t lhs, const UnsignedInteger rhs) noexcept
{
    return {lhs.high, lhs.low ^ static_cast<std::uint64_t>(rhs)};
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr uint128_t operator^(const UnsignedInteger lhs, const uint128_t rhs) noexcept
{
    return {rhs.high, rhs.low ^ static_cast<std::uint64_t>(lhs)};
}

constexpr uint128_t operator^(const uint128_t lhs, const uint128_t rhs) noexcept
{
    return {lhs.high ^ rhs.high, lhs.low ^ rhs.low};
}

#ifdef BOOST_INT128_HAS_INT128

constexpr uint128_t operator^(const uint128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs ^ static_cast<uint128_t>(rhs);
}

constexpr uint128_t operator^(const detail::builtin_i128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) ^ rhs;
}

constexpr uint128_t operator^(const uint128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return lhs ^ static_cast<uint128_t>(rhs);
}

constexpr uint128_t operator^(const detail::builtin_u128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) ^ rhs;
}

#endif // BOOST_INT128_HAS_INT128

template <BOOST_INT128_INTEGER_CONCEPT>
constexpr uint128_t uint128_t::operator^=(const Integer rhs) noexcept
{
    *this = *this ^ rhs;
    return *this;
}

constexpr uint128_t uint128_t::operator^=(const uint128_t rhs) noexcept
{
    *this = *this ^ rhs;
    return *this;
}

//=====================================
// Left Shift Operator
//=====================================

template <BOOST_INT128_DEFAULTED_INTEGER_CONCEPT>
constexpr uint128_t operator<<(const uint128_t lhs, const Integer rhs) noexcept
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
        return {lhs.low, 0};
    }

    if (rhs > 64)
    {
        return {lhs.low << (rhs - 64), 0};
    }

    return {
        (lhs.high << rhs) | (lhs.low >> (64 - rhs)),
        lhs.low << rhs
    };
}

// A number of different overloads to ensure that we return the same type as the builtins would

template <typename Integer, std::enable_if_t<std::is_integral<Integer>::value && (sizeof(Integer) * 8 > 16), bool> = true>
constexpr Integer operator<<(const Integer lhs, const uint128_t rhs) noexcept
{
    constexpr auto bit_width {sizeof(Integer) * CHAR_BIT};

    if (rhs.high > UINT64_C(0) || rhs.low >= bit_width)
    {
        return 0;
    }

    return lhs << rhs.low;
}

template <typename SignedInteger, std::enable_if_t<detail::is_signed_integer_v<SignedInteger> && (sizeof(SignedInteger) * 8 <= 16), bool> = true>
constexpr int operator<<(const SignedInteger lhs, const uint128_t rhs) noexcept
{
    constexpr auto bit_width {sizeof(SignedInteger) * CHAR_BIT};

    if (rhs.high > UINT64_C(0) || rhs.low >= bit_width)
    {
        return 0;
    }

    return static_cast<int>(lhs) << rhs.low;
}

template <typename UnsignedInteger, std::enable_if_t<detail::is_unsigned_integer_v<UnsignedInteger> && (sizeof(UnsignedInteger) * 8 <= 16), bool> = true>
constexpr unsigned int operator<<(const UnsignedInteger lhs, const uint128_t rhs) noexcept
{
    constexpr auto bit_width {sizeof(UnsignedInteger) * CHAR_BIT};

    if (rhs.high > UINT64_C(0) || rhs.low >= bit_width)
    {
        return 0;
    }

    return static_cast<unsigned int>(lhs) << rhs.low;
}

constexpr uint128_t operator<<(const uint128_t lhs, const uint128_t rhs) noexcept
{
    if (rhs >= 128)
    {
        return {0, 0};
    }

    if (rhs.low == 0)
    {
        return lhs;
    }

    if (rhs.low == 64)
    {
        return {lhs.low, 0};
    }

    if (rhs.low > 64)
    {
        return {lhs.low << (rhs.low - 64), 0};
    }

    return {
        (lhs.high << rhs.low) | (lhs.low >> (64 - rhs.low)),
        lhs.low << rhs.low
    };
}

template <BOOST_INT128_INTEGER_CONCEPT>
constexpr uint128_t uint128_t::operator<<=(const Integer rhs) noexcept
{
    *this = *this << rhs;
    return *this;
}

constexpr uint128_t uint128_t::operator<<=(const uint128_t rhs) noexcept
{
    *this = *this << rhs;
    return *this;
}

//=====================================
// Right Shift Operator
//=====================================

template <typename Integer, std::enable_if_t<std::is_integral<Integer>::value, bool> = true>
constexpr uint128_t operator>>(const uint128_t lhs, const Integer rhs) noexcept
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
        return {0, lhs.high};
    }

    if (rhs > 64)
    {
        return {0, lhs.high >> (rhs - 64)};
    }

    return {
        lhs.high >> rhs,
        (lhs.low >> rhs) | (lhs.high << (64 - rhs))
    };
}

template <typename Integer, std::enable_if_t<std::is_integral<Integer>::value && (sizeof(Integer) * 8 > 16), bool> = true>
constexpr Integer operator>>(const Integer lhs, const uint128_t rhs) noexcept
{
    constexpr auto bit_width = sizeof(Integer) * 8;

    if (rhs.high > UINT64_C(0) || rhs.low >= bit_width)
    {
        return 0;
    }

    return lhs >> rhs.low;
}

template <typename SignedInteger, std::enable_if_t<detail::is_signed_integer_v<SignedInteger> && (sizeof(SignedInteger) * 8 <= 16), bool> = true>
constexpr int operator>>(const SignedInteger lhs, const uint128_t rhs) noexcept
{
    constexpr auto bit_width = sizeof(SignedInteger) * 8;

    if (rhs.high > UINT64_C(0) || rhs.low >= bit_width)
    {
        return 0;
    }

    return static_cast<int>(lhs) >> rhs.low;
}

template <typename UnsignedInteger, std::enable_if_t<detail::is_unsigned_integer_v<UnsignedInteger> && (sizeof(UnsignedInteger) * 8 <= 16), bool> = true>
constexpr unsigned operator>>(UnsignedInteger lhs, const uint128_t rhs) noexcept
{
    constexpr auto bit_width = sizeof(UnsignedInteger) * 8;

    if (rhs.high > UINT64_C(0) || rhs.low >= bit_width)
    {
        return 0;
    }

    return static_cast<unsigned>(lhs) >> rhs.low;
}

constexpr uint128_t operator>>(const uint128_t lhs, const uint128_t rhs) noexcept
{
    if (rhs >= 128)
    {
        return {0, 0};
    }

    if (rhs.low == 0)
    {
        return lhs;
    }

    if (rhs.low == 64)
    {
        return {0, lhs.high};
    }

    if (rhs.low > 64)
    {
        return {0, lhs.high >> (rhs.low - UINT64_C(64))};
    }

    return {
        lhs.high >> rhs.low,
        (lhs.low >> rhs.low) | (lhs.high << (UINT64_C(64) - rhs.low))
    };
}

template <BOOST_INT128_INTEGER_CONCEPT>
constexpr uint128_t uint128_t::operator>>=(const Integer rhs) noexcept
{
    *this = *this >> rhs;
    return *this;
}

constexpr uint128_t uint128_t::operator>>=(const uint128_t rhs) noexcept
{
    *this = *this >> rhs;
    return *this;
}

//=====================================
// Increment Operator
//=====================================

constexpr uint128_t& uint128_t::operator++() noexcept
{
    if (++low == UINT64_C(0))
    {
        ++high;
    }

    return *this;
}

constexpr uint128_t& uint128_t::operator++(int) noexcept
{
    return ++(*this);
}

//=====================================
// Decrement Operator
//=====================================

constexpr uint128_t& uint128_t::operator--() noexcept
{
    if (--low == UINT64_MAX)
    {
        --high;
    }

    return *this;
}

constexpr uint128_t& uint128_t::operator--(int) noexcept
{
    return --(*this);
}

//=====================================
// Addition Operator
//=====================================

namespace impl {

BOOST_INT128_FORCE_INLINE constexpr uint128_t default_add(const uint128_t lhs, const uint128_t rhs) noexcept
{
    #if defined(BOOST_INT128_HAS_BUILTIN_ADD_OVERFLOW) && (defined(__i386__) || (defined(__aarch64__) && !defined(__APPLE__)) || defined(__arm__) || (defined(__s390__) || defined(__s390x__)))

    uint128_t res {};
    res.high = lhs.high + rhs.high + __builtin_add_overflow(lhs.low, rhs.low, &res.low);

    return res;

    #else

    uint128_t temp {lhs.high + rhs.high, lhs.low + rhs.low};

    if (temp.low < lhs.low)
    {
        ++temp.high;
    }

    return temp;

    #endif
}

BOOST_INT128_FORCE_INLINE constexpr uint128_t default_add(const uint128_t lhs, const std::uint64_t rhs) noexcept
{
    #if defined(BOOST_INT128_HAS_BUILTIN_ADD_OVERFLOW) && (defined(__i386__) || (defined(__aarch64__) && !defined(__APPLE__)) || defined(__arm__) || (defined(__s390__) || defined(__s390x__)))

    uint128_t res {};
    res.high = lhs.high + __builtin_add_overflow(lhs.low, rhs, &res.low);

    return res;

    #else

    uint128_t temp {lhs.high, lhs.low + rhs};

    if (temp.low < lhs.low)
    {
        ++temp.high;
    }

    return temp;

    #endif
}

BOOST_INT128_FORCE_INLINE constexpr uint128_t default_sub(const uint128_t lhs, const uint128_t rhs) noexcept
{
    #if defined(BOOST_INT128_HAS_BUILTIN_SUB_OVERFLOW) && (defined(__i386__) || defined(__arm__) || (defined(__s390__) || defined(__s390x__)))

    uint128_t res {};
    res.high = lhs.high - rhs.high - __builtin_sub_overflow(lhs.low, rhs.low, &res.low);

    return res;

    #elif (defined(__x86_64__) || (defined(__aarch64__) && !defined(__APPLE__))) && !defined(_MSC_VER)

    return static_cast<uint128_t>(static_cast<detail::builtin_u128>(lhs) - static_cast<detail::builtin_u128>(rhs));

    #else

    uint128_t temp {lhs.high - rhs.high, lhs.low - rhs.low};

    // Check for carry
    if (lhs.low < rhs.low)
    {
        --temp.high;
    }

    return temp;

    #endif
}

BOOST_INT128_FORCE_INLINE constexpr uint128_t default_sub(const uint128_t lhs, const std::uint64_t rhs) noexcept
{
    #if defined(BOOST_INT128_HAS_BUILTIN_SUB_OVERFLOW) && (defined(__i386__) || (defined(__aarch64__) && !defined(__APPLE__)) || defined(__arm__) || (defined(__s390__) || defined(__s390x__)))

    uint128_t res {};
    res.high = lhs.high - __builtin_sub_overflow(lhs.low, rhs, &res.low);

    return res;

    #else

    uint128_t temp {lhs.high, lhs.low - rhs};

    // Check for carry
    if (lhs.low < rhs)
    {
        --temp.high;
    }

    return temp;

    #endif
}

} // namespace impl

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr uint128_t operator+(const uint128_t lhs, const SignedInteger rhs) noexcept
{
    return rhs < 0 ? impl::default_sub(lhs, -static_cast<std::uint64_t>(rhs)) :
                     impl::default_add(lhs, static_cast<std::uint64_t>(rhs));
}

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr uint128_t operator+(const SignedInteger lhs, const uint128_t rhs) noexcept
{
    return lhs < 0 ? impl::default_sub(rhs, -static_cast<std::uint64_t>(lhs)) :
                     impl::default_add(rhs, static_cast<std::uint64_t>(lhs));
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr uint128_t operator+(const uint128_t lhs, const UnsignedInteger rhs) noexcept
{
    return impl::default_add(lhs, static_cast<std::uint64_t>(rhs));
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr uint128_t operator+(const UnsignedInteger lhs, const uint128_t rhs) noexcept
{
    return impl::default_add(rhs, static_cast<std::uint64_t>(lhs));
}

constexpr uint128_t operator+(const uint128_t lhs, const uint128_t rhs) noexcept
{
    #if defined(__s390__) || defined(__s390x__)

    return impl::default_sub(lhs, {rhs.high + static_cast<std::uint64_t>(rhs.low == UINT64_C(0)), ~rhs.low + UINT64_C(1)});

    #else

    return impl::default_add(lhs, rhs);

    #endif
}

#ifdef BOOST_INT128_HAS_INT128

constexpr uint128_t operator+(const uint128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return impl::default_add(lhs, static_cast<uint128_t>(rhs));
}

constexpr uint128_t operator+(const detail::builtin_i128 lhs, const uint128_t rhs) noexcept
{
    return impl::default_add(static_cast<uint128_t>(lhs), rhs);
}

constexpr uint128_t operator+(const uint128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return impl::default_add(lhs, static_cast<uint128_t>(rhs));
}

constexpr uint128_t operator+(const detail::builtin_u128 lhs, const uint128_t rhs) noexcept
{
    return impl::default_add(static_cast<int128_t>(lhs), rhs);
}

#endif // BOOST_INT128_HAS_INT128

template <BOOST_INT128_INTEGER_CONCEPT>
constexpr uint128_t& uint128_t::operator+=(const Integer rhs) noexcept
{
    *this = *this + rhs;
    return *this;
}

constexpr uint128_t& uint128_t::operator+=(const uint128_t rhs) noexcept
{
    *this = *this + rhs;
    return *this;
}

//=====================================
// Subtraction Operator
//=====================================

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr uint128_t operator-(const uint128_t lhs, const SignedInteger rhs) noexcept
{
    return rhs < 0 ? impl::default_add(lhs, -static_cast<std::uint64_t>(rhs)) :
                     impl::default_sub(lhs, static_cast<std::uint64_t>(rhs));
}

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr uint128_t operator-(const SignedInteger lhs, const uint128_t rhs) noexcept
{
    return lhs < 0 ? impl::default_sub(-rhs, -static_cast<std::uint64_t>(lhs)) :
                     impl::default_add(-rhs, static_cast<std::uint64_t>(lhs));
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr uint128_t operator-(const uint128_t lhs, const UnsignedInteger rhs) noexcept
{
    return impl::default_sub(lhs, static_cast<std::uint64_t>(rhs));
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr uint128_t operator-(const UnsignedInteger lhs, const uint128_t rhs) noexcept
{
    return impl::default_add(-rhs, static_cast<std::uint64_t>(lhs));
}

constexpr uint128_t operator-(const uint128_t lhs, const uint128_t rhs) noexcept
{
    return impl::default_sub(lhs, rhs);
}

#ifdef BOOST_INT128_HAS_INT128

constexpr uint128_t operator-(const uint128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs - static_cast<uint128_t>(rhs);
}

constexpr uint128_t operator-(const detail::builtin_i128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) - rhs;
}

constexpr uint128_t operator-(const uint128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return lhs - static_cast<uint128_t>(rhs);
}

constexpr uint128_t operator-(const detail::builtin_u128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) - rhs;
}

#endif // BOOST_INT128_HAS_INT128

template <BOOST_INT128_INTEGER_CONCEPT>
constexpr uint128_t& uint128_t::operator-=(const Integer rhs) noexcept
{
    *this = *this - rhs;
    return *this;
}

constexpr uint128_t& uint128_t::operator-=(const uint128_t rhs) noexcept
{
    *this = *this - rhs;
    return *this;
}

//=====================================
// Multiplication Operator
//=====================================

#if defined(__GNUC__) && __GNUC__ >= 8
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wclass-memaccess"
#endif

namespace detail {

BOOST_INT128_FORCE_INLINE constexpr void to_words(const uint128_t& x, std::uint32_t (&words)[4]) noexcept
{
    #ifndef BOOST_INT128_NO_CONSTEVAL_DETECTION

    if (!BOOST_INT128_IS_CONSTANT_EVALUATED(x))
    {
        std::memcpy(&words, &x, sizeof(uint128_t));
    }

    #endif

    words[0] = static_cast<std::uint32_t>(x.low & UINT32_MAX);
    words[1] = static_cast<std::uint32_t>(x.low >> 32);
    words[2] = static_cast<std::uint32_t>(x.high & UINT32_MAX);
    words[3] = static_cast<std::uint32_t>(x.high >> 32);
}

template <typename UnsignedInteger>
BOOST_INT128_FORCE_INLINE constexpr uint128_t default_mul(const uint128_t lhs, const UnsignedInteger rhs) noexcept
{
    static_assert(std::is_same<UnsignedInteger, std::uint32_t>::value ||
                  std::is_same<UnsignedInteger, std::uint64_t>::value ||
                  std::is_same<UnsignedInteger, uint128_t>::value,
                  "Must be 32, 64 or 128 bit unsigned integer");

    constexpr std::size_t rhs_words_needed {sizeof(UnsignedInteger) / sizeof(std::uint32_t)};

    std::uint32_t lhs_words[4] {};
    std::uint32_t rhs_words[rhs_words_needed] {};
    to_words(lhs, lhs_words);
    to_words(rhs, rhs_words);

    return knuth_multiply<uint128_t>(lhs_words, rhs_words);
}

} // namespace detail

#if defined(__GNUC__) && __GNUC__ >= 8
#  pragma GCC diagnostic pop
#endif

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr uint128_t operator*(const uint128_t lhs, const SignedInteger rhs) noexcept
{
    using eval_type = detail::evaluation_type_t<SignedInteger>;

    const auto abs_rhs {rhs < 0 ? -static_cast<eval_type>(rhs) : static_cast<eval_type>(rhs)};
    const auto res {detail::default_mul(lhs, abs_rhs)};

    return rhs < 0 ? -res : res;
}

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
constexpr uint128_t operator*(const SignedInteger lhs, const uint128_t rhs) noexcept
{
    using eval_type = detail::evaluation_type_t<SignedInteger>;

    const auto abs_lhs {lhs < 0 ? -static_cast<eval_type>(lhs) : static_cast<eval_type>(lhs)};
    const auto res {detail::default_mul(rhs, abs_lhs)};

    return lhs < 0 ? -res : res;
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr uint128_t operator*(const uint128_t lhs, const UnsignedInteger rhs) noexcept
{
    return detail::default_mul(lhs, static_cast<std::uint64_t>(rhs));
}

template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
constexpr uint128_t operator*(const UnsignedInteger lhs, const uint128_t rhs) noexcept
{
    return detail::default_mul(rhs, static_cast<std::uint64_t>(lhs));
}

constexpr uint128_t operator*(const uint128_t lhs, const uint128_t rhs) noexcept
{
    return detail::default_mul(lhs, rhs);
}

#ifdef BOOST_INT128_HAS_INT128

constexpr uint128_t operator*(const uint128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    const auto abs_rhs {rhs < 0 ? -static_cast<uint128_t>(rhs) : static_cast<uint128_t>(rhs)};
    const auto res {lhs * abs_rhs};

    return rhs < 0 ? -res : res;
}

constexpr uint128_t operator*(const detail::builtin_i128 lhs, const uint128_t rhs) noexcept
{
    const auto abs_lhs {lhs < 0 ? -static_cast<uint128_t>(lhs) : static_cast<uint128_t>(lhs)};
    const auto res {abs_lhs * rhs};

    return lhs < 0 ? -res : res;
}

constexpr uint128_t operator*(const uint128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return lhs * static_cast<uint128_t>(rhs);
}

constexpr uint128_t operator*(const detail::builtin_u128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) * rhs;
}

#endif // BOOST_INT128_HAS_INT128

template <BOOST_INT128_INTEGER_CONCEPT>
constexpr uint128_t& uint128_t::operator*=(const Integer rhs) noexcept
{
    *this = *this * rhs;
    return *this;
}

constexpr uint128_t& uint128_t::operator*=(const uint128_t rhs) noexcept
{
    *this = *this * rhs;
    return *this;
}

} // namespace int128
} // namespace boost

#endif //BOOST_INT128_DETAIL_UINT128_HPP
