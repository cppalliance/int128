// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128.hpp>
#include <boost/core/lightweight_test.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/xoshiro.hpp>
#include <random>

#ifdef BOOST_INT128_HAS_INT128

using boost::int128::detail::builtin_i128;
using boost::int128::detail::builtin_u128;

template <typename T>
constexpr T get_max();

template <typename T>
constexpr T get_min();

template <>
constexpr builtin_u128 get_max<builtin_u128>()
{
    return static_cast<builtin_u128>(UINT64_MAX) << 64 | UINT64_MAX;
}

template <>
constexpr builtin_u128 get_min<builtin_u128>()
{
    return 0;
}

template <>
builtin_i128 get_max<builtin_i128>()
{
    return static_cast<builtin_i128>((static_cast<builtin_u128>(1) << 127) - 1);
}

template <>
builtin_i128 get_min<builtin_i128>()
{
    return -get_max<builtin_i128>() - 1;
}

static constexpr std::size_t N {1024};

static boost::random::xoshiro256pp rng;
static boost::random::uniform_int_distribution<builtin_i128> signed_dist(get_min<builtin_i128>(),get_max<builtin_i128>());
static boost::random::uniform_int_distribution<builtin_u128> unsigned_dist(get_min<builtin_u128>(),get_max<builtin_u128>());

void test_construction()
{
    for (std::size_t i = 0; i < N; ++i)
    {
        const auto builtin_signed_val {signed_dist(rng)};
        const auto builtin_unsigned_val {unsigned_dist(rng)};

        const auto builtin_converted_signed_val {static_cast<builtin_i128>(builtin_unsigned_val)};
        const auto builtin_converted_unsigned_val {static_cast<builtin_u128>(builtin_signed_val)};

        const auto emulated_signed_val {static_cast<boost::int128::int128_t>(builtin_signed_val)};
        const auto emulated_unsigned_val {static_cast<boost::int128::uint128_t>(builtin_unsigned_val)};

        const boost::int128::int128_t emulated_converted_signed_val {emulated_unsigned_val};
        const boost::int128::uint128_t emulated_converted_unsigned_val {emulated_signed_val};

        BOOST_TEST(builtin_converted_signed_val == emulated_converted_signed_val);
        BOOST_TEST(builtin_converted_unsigned_val == emulated_converted_unsigned_val);
    }
}

void test_conversion()
{
    for (std::size_t i = 0; i < N; ++i)
    {
        const auto builtin_signed_val {signed_dist(rng)};
        const auto builtin_unsigned_val {unsigned_dist(rng)};

        const auto builtin_converted_signed_val {static_cast<builtin_i128>(builtin_unsigned_val)};
        const auto builtin_converted_unsigned_val {static_cast<builtin_u128>(builtin_signed_val)};

        const auto emulated_signed_val {static_cast<boost::int128::int128_t>(builtin_signed_val)};
        const auto emulated_unsigned_val {static_cast<boost::int128::uint128_t>(builtin_unsigned_val)};

        const auto emulated_converted_signed_val {static_cast<boost::int128::int128_t>(emulated_unsigned_val)};
        const auto emulated_converted_unsigned_val {static_cast<boost::int128::uint128_t>(emulated_signed_val)};

        BOOST_TEST(builtin_converted_signed_val == emulated_converted_signed_val);
        BOOST_TEST(builtin_converted_unsigned_val == emulated_converted_unsigned_val);
    }
}

int main()
{
    test_construction();
    test_conversion();

    return boost::report_errors();
}

#else

#include <iostream>

int main()
{
    std::cerr << "Test not run for lack of builtin 128-bit types" << std::endl;
    return 0;
}

#endif
