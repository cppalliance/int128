// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
//
// Verifies that boost::int128::int128_t and boost::int128::uint128_t produce
// results identical to the built-in __int128 / unsigned __int128 types under
// the C++ usual arithmetic conversions, for every operator x type-pair.

#include <boost/int128.hpp>
#include <boost/core/lightweight_test.hpp>
#include <cstdint>
#include <random>

#ifdef BOOST_INT128_HAS_INT128

#ifdef __GNUC__
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wsign-compare"
#  pragma GCC diagnostic ignored "-Wsign-conversion"
#  pragma GCC diagnostic ignored "-Wconversion"
#endif

using boost::int128::int128_t;
using boost::int128::uint128_t;
using boost::int128::detail::builtin_i128;
using boost::int128::detail::builtin_u128;

static std::mt19937_64 rng{42};
static constexpr std::size_t N {256U};

template <typename T>
static T random_value()
{
    return static_cast<T>(rng());
}

template <>
builtin_u128 random_value<builtin_u128>()
{
    return (static_cast<builtin_u128>(rng()) << 64) | static_cast<builtin_u128>(rng());
}

template <>
builtin_i128 random_value<builtin_i128>()
{
    return static_cast<builtin_i128>(random_value<builtin_u128>());
}

// =========================================================================
// uint128_t vs small signed/unsigned built-in integers
// =========================================================================

template <typename SignedT>
void test_uint128_vs_signed_small()
{
    for (std::size_t i {0}; i < N; ++i)
    {
        const auto raw_lhs {random_value<builtin_u128>()};
        const auto raw_rhs {random_value<SignedT>()};
        const uint128_t lib_lhs {raw_lhs};
        const SignedT s_rhs {raw_rhs};

        // Builtin path: usual arithmetic conversion converts SignedT to unsigned __int128
        const builtin_u128 oracle_lhs {raw_lhs};
        const builtin_u128 oracle_rhs = static_cast<builtin_u128>(static_cast<builtin_i128>(s_rhs));

        // Comparisons
        BOOST_TEST_EQ(lib_lhs == s_rhs, oracle_lhs == oracle_rhs);
        BOOST_TEST_EQ(lib_lhs != s_rhs, oracle_lhs != oracle_rhs);
        BOOST_TEST_EQ(lib_lhs <  s_rhs, oracle_lhs <  oracle_rhs);
        BOOST_TEST_EQ(lib_lhs <= s_rhs, oracle_lhs <= oracle_rhs);
        BOOST_TEST_EQ(lib_lhs >  s_rhs, oracle_lhs >  oracle_rhs);
        BOOST_TEST_EQ(lib_lhs >= s_rhs, oracle_lhs >= oracle_rhs);

        BOOST_TEST_EQ(s_rhs == lib_lhs, oracle_rhs == oracle_lhs);
        BOOST_TEST_EQ(s_rhs != lib_lhs, oracle_rhs != oracle_lhs);
        BOOST_TEST_EQ(s_rhs <  lib_lhs, oracle_rhs <  oracle_lhs);
        BOOST_TEST_EQ(s_rhs <= lib_lhs, oracle_rhs <= oracle_lhs);
        BOOST_TEST_EQ(s_rhs >  lib_lhs, oracle_rhs >  oracle_lhs);
        BOOST_TEST_EQ(s_rhs >= lib_lhs, oracle_rhs >= oracle_lhs);

        // Arithmetic
        BOOST_TEST_EQ(lib_lhs + s_rhs, uint128_t{oracle_lhs + oracle_rhs});
        BOOST_TEST_EQ(lib_lhs - s_rhs, uint128_t{oracle_lhs - oracle_rhs});
        BOOST_TEST_EQ(lib_lhs * s_rhs, uint128_t{oracle_lhs * oracle_rhs});
        if (s_rhs != 0)
        {
            BOOST_TEST_EQ(lib_lhs / s_rhs, uint128_t{oracle_lhs / oracle_rhs});
            BOOST_TEST_EQ(lib_lhs % s_rhs, uint128_t{oracle_lhs % oracle_rhs});
        }
        if (raw_lhs != 0)
        {
            BOOST_TEST_EQ(s_rhs + lib_lhs, uint128_t{oracle_rhs + oracle_lhs});
            BOOST_TEST_EQ(s_rhs - lib_lhs, uint128_t{oracle_rhs - oracle_lhs});
            BOOST_TEST_EQ(s_rhs * lib_lhs, uint128_t{oracle_rhs * oracle_lhs});
            BOOST_TEST_EQ(s_rhs / lib_lhs, uint128_t{oracle_rhs / oracle_lhs});
            BOOST_TEST_EQ(s_rhs % lib_lhs, uint128_t{oracle_rhs % oracle_lhs});
        }

        // Bitwise
        BOOST_TEST_EQ(lib_lhs | s_rhs, uint128_t{oracle_lhs | oracle_rhs});
        BOOST_TEST_EQ(lib_lhs & s_rhs, uint128_t{oracle_lhs & oracle_rhs});
        BOOST_TEST_EQ(lib_lhs ^ s_rhs, uint128_t{oracle_lhs ^ oracle_rhs});
    }
}

// =========================================================================
// int128_t vs small signed/unsigned built-in integers
// =========================================================================

template <typename UnsignedT>
void test_int128_vs_unsigned_small()
{
    for (std::size_t i {0}; i < N; ++i)
    {
        const auto raw_lhs {random_value<builtin_i128>()};
        const auto raw_rhs {random_value<UnsignedT>()};
        const int128_t lib_lhs {raw_lhs};
        const UnsignedT u_rhs {raw_rhs};

        // Builtin path: int128_t has higher rank and can represent UnsignedT,
        // so both promote to __int128 (signed) and the result type is signed.
        const builtin_i128 oracle_lhs {raw_lhs};
        const builtin_i128 oracle_rhs = static_cast<builtin_i128>(u_rhs);

        BOOST_TEST_EQ(lib_lhs == u_rhs, oracle_lhs == oracle_rhs);
        BOOST_TEST_EQ(lib_lhs != u_rhs, oracle_lhs != oracle_rhs);
        BOOST_TEST_EQ(lib_lhs <  u_rhs, oracle_lhs <  oracle_rhs);
        BOOST_TEST_EQ(lib_lhs <= u_rhs, oracle_lhs <= oracle_rhs);
        BOOST_TEST_EQ(lib_lhs >  u_rhs, oracle_lhs >  oracle_rhs);
        BOOST_TEST_EQ(lib_lhs >= u_rhs, oracle_lhs >= oracle_rhs);

        BOOST_TEST_EQ(u_rhs == lib_lhs, oracle_rhs == oracle_lhs);
        BOOST_TEST_EQ(u_rhs != lib_lhs, oracle_rhs != oracle_lhs);
        BOOST_TEST_EQ(u_rhs <  lib_lhs, oracle_rhs <  oracle_lhs);
        BOOST_TEST_EQ(u_rhs <= lib_lhs, oracle_rhs <= oracle_lhs);
        BOOST_TEST_EQ(u_rhs >  lib_lhs, oracle_rhs >  oracle_lhs);
        BOOST_TEST_EQ(u_rhs >= lib_lhs, oracle_rhs >= oracle_lhs);

        BOOST_TEST_EQ(lib_lhs + u_rhs, int128_t{oracle_lhs + oracle_rhs});
        BOOST_TEST_EQ(lib_lhs - u_rhs, int128_t{oracle_lhs - oracle_rhs});
        BOOST_TEST_EQ(lib_lhs * u_rhs, int128_t{oracle_lhs * oracle_rhs});
        if (u_rhs != 0)
        {
            BOOST_TEST_EQ(lib_lhs / u_rhs, int128_t{oracle_lhs / oracle_rhs});
            BOOST_TEST_EQ(lib_lhs % u_rhs, int128_t{oracle_lhs % oracle_rhs});
        }
        if (raw_lhs != 0)
        {
            BOOST_TEST_EQ(u_rhs + lib_lhs, int128_t{oracle_rhs + oracle_lhs});
            BOOST_TEST_EQ(u_rhs - lib_lhs, int128_t{oracle_rhs - oracle_lhs});
            BOOST_TEST_EQ(u_rhs * lib_lhs, int128_t{oracle_rhs * oracle_lhs});
            BOOST_TEST_EQ(u_rhs / lib_lhs, int128_t{oracle_rhs / oracle_lhs});
            BOOST_TEST_EQ(u_rhs % lib_lhs, int128_t{oracle_rhs % oracle_lhs});
        }

        BOOST_TEST_EQ(lib_lhs | u_rhs, int128_t{oracle_lhs | oracle_rhs});
        BOOST_TEST_EQ(lib_lhs & u_rhs, int128_t{oracle_lhs & oracle_rhs});
        BOOST_TEST_EQ(lib_lhs ^ u_rhs, int128_t{oracle_lhs ^ oracle_rhs});
    }
}

// =========================================================================
// uint128_t vs int128_t (cross-type)
// =========================================================================

void test_cross_type()
{
    for (std::size_t i {0}; i < N; ++i)
    {
        const auto raw_u {random_value<builtin_u128>()};
        const auto raw_i {random_value<builtin_i128>()};
        const uint128_t lib_u {raw_u};
        const int128_t lib_i {raw_i};

        // Both promote to unsigned __int128 (same rank, signed -> unsigned)
        const builtin_u128 oracle_u {raw_u};
        const builtin_u128 oracle_i = static_cast<builtin_u128>(raw_i);

        BOOST_TEST_EQ(lib_u == lib_i, oracle_u == oracle_i);
        BOOST_TEST_EQ(lib_u != lib_i, oracle_u != oracle_i);
        BOOST_TEST_EQ(lib_u <  lib_i, oracle_u <  oracle_i);
        BOOST_TEST_EQ(lib_u <= lib_i, oracle_u <= oracle_i);
        BOOST_TEST_EQ(lib_u >  lib_i, oracle_u >  oracle_i);
        BOOST_TEST_EQ(lib_u >= lib_i, oracle_u >= oracle_i);

        BOOST_TEST_EQ(lib_i == lib_u, oracle_i == oracle_u);
        BOOST_TEST_EQ(lib_i != lib_u, oracle_i != oracle_u);
        BOOST_TEST_EQ(lib_i <  lib_u, oracle_i <  oracle_u);
        BOOST_TEST_EQ(lib_i <= lib_u, oracle_i <= oracle_u);
        BOOST_TEST_EQ(lib_i >  lib_u, oracle_i >  oracle_u);
        BOOST_TEST_EQ(lib_i >= lib_u, oracle_i >= oracle_u);

        BOOST_TEST_EQ(lib_u + lib_i, uint128_t{oracle_u + oracle_i});
        BOOST_TEST_EQ(lib_u - lib_i, uint128_t{oracle_u - oracle_i});
        BOOST_TEST_EQ(lib_u * lib_i, uint128_t{oracle_u * oracle_i});
        if (oracle_i != 0)
        {
            BOOST_TEST_EQ(lib_u / lib_i, uint128_t{oracle_u / oracle_i});
            BOOST_TEST_EQ(lib_u % lib_i, uint128_t{oracle_u % oracle_i});
        }
        if (oracle_u != 0)
        {
            BOOST_TEST_EQ(lib_i / lib_u, uint128_t{oracle_i / oracle_u});
            BOOST_TEST_EQ(lib_i % lib_u, uint128_t{oracle_i % oracle_u});
        }
    }
}

// =========================================================================
// uint128_t vs builtin_i128
// =========================================================================

void test_uint128_vs_builtin_i128()
{
    for (std::size_t i {0}; i < N; ++i)
    {
        const auto raw_u {random_value<builtin_u128>()};
        const auto raw_i {random_value<builtin_i128>()};
        const uint128_t lib_u {raw_u};

        const builtin_u128 oracle_u {raw_u};
        const builtin_u128 oracle_i = static_cast<builtin_u128>(raw_i);

        BOOST_TEST_EQ(lib_u == raw_i, oracle_u == oracle_i);
        BOOST_TEST_EQ(lib_u != raw_i, oracle_u != oracle_i);
        BOOST_TEST_EQ(lib_u <  raw_i, oracle_u <  oracle_i);
        BOOST_TEST_EQ(lib_u <= raw_i, oracle_u <= oracle_i);
        BOOST_TEST_EQ(lib_u >  raw_i, oracle_u >  oracle_i);
        BOOST_TEST_EQ(lib_u >= raw_i, oracle_u >= oracle_i);

        BOOST_TEST_EQ(lib_u + raw_i, uint128_t{oracle_u + oracle_i});
        BOOST_TEST_EQ(lib_u - raw_i, uint128_t{oracle_u - oracle_i});
        BOOST_TEST_EQ(lib_u * raw_i, uint128_t{oracle_u * oracle_i});
        if (oracle_i != 0)
        {
            BOOST_TEST_EQ(lib_u / raw_i, uint128_t{oracle_u / oracle_i});
            BOOST_TEST_EQ(lib_u % raw_i, uint128_t{oracle_u % oracle_i});
        }
    }
}

// =========================================================================
// int128_t vs builtin_u128
// =========================================================================

void test_int128_vs_builtin_u128()
{
    for (std::size_t i {0}; i < N; ++i)
    {
        const auto raw_i {random_value<builtin_i128>()};
        const auto raw_u {random_value<builtin_u128>()};
        const int128_t lib_i {raw_i};

        // Both promote to unsigned __int128 (same rank, signed -> unsigned).
        // Result of arithmetic is unsigned __int128 (uint128_t in library form).
        const builtin_u128 oracle_i = static_cast<builtin_u128>(raw_i);
        const builtin_u128 oracle_u {raw_u};

        BOOST_TEST_EQ(lib_i == raw_u, oracle_i == oracle_u);
        BOOST_TEST_EQ(lib_i != raw_u, oracle_i != oracle_u);
        BOOST_TEST_EQ(lib_i <  raw_u, oracle_i <  oracle_u);
        BOOST_TEST_EQ(lib_i <= raw_u, oracle_i <= oracle_u);
        BOOST_TEST_EQ(lib_i >  raw_u, oracle_i >  oracle_u);
        BOOST_TEST_EQ(lib_i >= raw_u, oracle_i >= oracle_u);

        BOOST_TEST_EQ(lib_i + raw_u, uint128_t{oracle_i + oracle_u});
        BOOST_TEST_EQ(lib_i - raw_u, uint128_t{oracle_i - oracle_u});
        BOOST_TEST_EQ(lib_i * raw_u, uint128_t{oracle_i * oracle_u});
        if (oracle_u != 0)
        {
            BOOST_TEST_EQ(lib_i / raw_u, uint128_t{oracle_i / oracle_u});
            BOOST_TEST_EQ(lib_i % raw_u, uint128_t{oracle_i % oracle_u});
        }

        BOOST_TEST_EQ(lib_i | raw_u, uint128_t{oracle_i | oracle_u});
        BOOST_TEST_EQ(lib_i & raw_u, uint128_t{oracle_i & oracle_u});
        BOOST_TEST_EQ(lib_i ^ raw_u, uint128_t{oracle_i ^ oracle_u});
    }
}

#endif // BOOST_INT128_HAS_INT128

int main()
{
    #ifdef BOOST_INT128_HAS_INT128

    test_uint128_vs_signed_small<std::int8_t>();
    test_uint128_vs_signed_small<std::int16_t>();
    test_uint128_vs_signed_small<std::int32_t>();
    test_uint128_vs_signed_small<std::int64_t>();

    test_int128_vs_unsigned_small<std::uint8_t>();
    test_int128_vs_unsigned_small<std::uint16_t>();
    test_int128_vs_unsigned_small<std::uint32_t>();
    test_int128_vs_unsigned_small<std::uint64_t>();

    test_cross_type();
    test_uint128_vs_builtin_i128();
    test_int128_vs_builtin_u128();

    #endif

    return boost::report_errors();
}
