// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128/detail/int128_imp.hpp>
#include <boost/int128/detail/conversions.hpp>
#include <boost/core/lightweight_test.hpp>
#include <cstring>
#include <cstdint>
#include <random>
#include <limits>
#include <cmath>
#include <sstream>
#include <iostream>
#include <iomanip>

#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wold-style-cast"
#  pragma clang diagnostic ignored "-Wundef"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wsign-conversion"
#  pragma clang diagnostic ignored "-Wfloat-equal"
#  pragma clang diagnostic ignored "-Wsign-compare"
#  pragma clang diagnostic ignored "-Woverflow"

#  if (__clang_major__ >= 10 && !defined(__APPLE__)) || __clang_major__ >= 13
#    pragma clang diagnostic ignored "-Wdeprecated-copy"
#  endif

#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wundef"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wsign-conversion"
#  pragma GCC diagnostic ignored "-Wsign-compare"
#  pragma GCC diagnostic ignored "-Wfloat-equal"
#  pragma GCC diagnostic ignored "-Woverflow"

#elif defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 4389)
#  pragma warning(disable : 4127)
#endif

template <typename IntType>
constexpr IntType get_max()
{
    return std::numeric_limits<IntType>::max();
}

template <typename IntType>
constexpr IntType get_min()
{
    return std::numeric_limits<IntType>::min();
}

#include <boost/random/uniform_int_distribution.hpp>

// Used defined seed for repeatability
static std::mt19937_64 rng(42);

constexpr std::size_t N = 1024;

#ifdef BOOST_INT128_HAS_INT128

// We reduce the max end of the 128 bit types as they can cause a stack overflow in boost.random

template <>
constexpr unsigned __int128 get_max<unsigned __int128>()
{
    return static_cast<unsigned __int128>(UINT64_MAX) << 64 | UINT64_MAX / 32;
}

template <>
constexpr unsigned __int128 get_min<unsigned __int128>()
{
    return 0;
}

template <>
constexpr __int128 get_max<__int128>()
{
    return static_cast<__int128>((static_cast<unsigned __int128>(1) << 127) - 1) / 32;
}

template <>
constexpr __int128 get_min<__int128>()
{
    return -get_max<__int128>() - 1;
}

template <typename IntType>
void test_arithmetic_constructor()
{
    boost::random::uniform_int_distribution<IntType> dist(get_min<IntType>(),
                                                          get_max<IntType>());

    for (std::size_t i {}; i < N; ++i)
    {
        const IntType value {dist(rng)};
        __int128 builtin_value = static_cast<__int128>(value);
        boost::int128::int128_t emulated_value {value};

        __int128 emulated_bits;
        std::memcpy(&emulated_bits, &emulated_value, sizeof(__int128));

        BOOST_TEST(emulated_bits == builtin_value);
    }
}

int main()
{
    return 0;
}

#else

int main()
{
    return 0;
}

#endif
