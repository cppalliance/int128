// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
//
// Verifies that boost::int128::int128 and boost::int128::uint128 produce
// results identical to the built-in __int128 / unsigned __int128 types under
// the C++ usual arithmetic conversions, for every operator x type-pair.

#include <boost/int128.hpp>
#include <boost/core/lightweight_test.hpp>
#include <cstdint>
#include <cstring>
#include <random>

#ifdef BOOST_INT128_HAS_INT128

#ifdef __GNUC__
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wsign-compare"
#  pragma GCC diagnostic ignored "-Wsign-conversion"
#  pragma GCC diagnostic ignored "-Wconversion"
#endif

using boost::int128::int128;
using boost::int128::uint128;
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
// uint128 vs small signed/unsigned built-in integers
// =========================================================================

template <typename SignedT>
void test_uint128_vs_signed_small()
{
    for (std::size_t i {0}; i < N; ++i)
    {
        const auto raw_lhs {random_value<builtin_u128>()};
        const auto raw_rhs {random_value<SignedT>()};
        const uint128 lib_lhs {raw_lhs};
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
        BOOST_TEST_EQ(lib_lhs + s_rhs, uint128{oracle_lhs + oracle_rhs});
        BOOST_TEST_EQ(lib_lhs - s_rhs, uint128{oracle_lhs - oracle_rhs});
        BOOST_TEST_EQ(lib_lhs * s_rhs, uint128{oracle_lhs * oracle_rhs});
        if (s_rhs != 0)
        {
            BOOST_TEST_EQ(lib_lhs / s_rhs, uint128{oracle_lhs / oracle_rhs});
            BOOST_TEST_EQ(lib_lhs % s_rhs, uint128{oracle_lhs % oracle_rhs});
        }
        if (raw_lhs != 0)
        {
            BOOST_TEST_EQ(s_rhs + lib_lhs, uint128{oracle_rhs + oracle_lhs});
            BOOST_TEST_EQ(s_rhs - lib_lhs, uint128{oracle_rhs - oracle_lhs});
            BOOST_TEST_EQ(s_rhs * lib_lhs, uint128{oracle_rhs * oracle_lhs});
            BOOST_TEST_EQ(s_rhs / lib_lhs, uint128{oracle_rhs / oracle_lhs});
            BOOST_TEST_EQ(s_rhs % lib_lhs, uint128{oracle_rhs % oracle_lhs});
        }

        // Bitwise
        BOOST_TEST_EQ(lib_lhs | s_rhs, uint128{oracle_lhs | oracle_rhs});
        BOOST_TEST_EQ(lib_lhs & s_rhs, uint128{oracle_lhs & oracle_rhs});
        BOOST_TEST_EQ(lib_lhs ^ s_rhs, uint128{oracle_lhs ^ oracle_rhs});
    }
}

// =========================================================================
// int128 vs small signed/unsigned built-in integers
// =========================================================================

template <typename UnsignedT>
void test_int128_vs_unsigned_small()
{
    for (std::size_t i {0}; i < N; ++i)
    {
        const auto raw_lhs {random_value<builtin_i128>()};
        const auto raw_rhs {random_value<UnsignedT>()};
        const int128 lib_lhs {raw_lhs};
        const UnsignedT u_rhs {raw_rhs};

        // Builtin path: int128 has higher rank and can represent UnsignedT,
        // so both promote to __int128 (signed) and the result type is signed.
        // Comparisons use the signed oracle directly. Arithmetic and bitwise
        // are performed in the unsigned domain (well-defined wrap-around) and
        // reinterpreted as signed for the result type, matching the library's
        // wrap-around semantics and avoiding UB-on-overflow that UBSan flags.
        const builtin_i128 oracle_lhs {raw_lhs};
        const builtin_i128 oracle_rhs = static_cast<builtin_i128>(u_rhs);
        const builtin_u128 oracle_lhs_u = static_cast<builtin_u128>(oracle_lhs);
        const builtin_u128 oracle_rhs_u = static_cast<builtin_u128>(oracle_rhs);

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

        BOOST_TEST_EQ(lib_lhs + u_rhs, int128{static_cast<builtin_i128>(oracle_lhs_u + oracle_rhs_u)});
        BOOST_TEST_EQ(lib_lhs - u_rhs, int128{static_cast<builtin_i128>(oracle_lhs_u - oracle_rhs_u)});
        BOOST_TEST_EQ(lib_lhs * u_rhs, int128{static_cast<builtin_i128>(oracle_lhs_u * oracle_rhs_u)});
        if (u_rhs != 0)
        {
            BOOST_TEST_EQ(lib_lhs / u_rhs, int128{oracle_lhs / oracle_rhs});
            BOOST_TEST_EQ(lib_lhs % u_rhs, int128{oracle_lhs % oracle_rhs});
        }
        if (raw_lhs != 0)
        {
            BOOST_TEST_EQ(u_rhs + lib_lhs, int128{static_cast<builtin_i128>(oracle_rhs_u + oracle_lhs_u)});
            BOOST_TEST_EQ(u_rhs - lib_lhs, int128{static_cast<builtin_i128>(oracle_rhs_u - oracle_lhs_u)});
            BOOST_TEST_EQ(u_rhs * lib_lhs, int128{static_cast<builtin_i128>(oracle_rhs_u * oracle_lhs_u)});
            BOOST_TEST_EQ(u_rhs / lib_lhs, int128{oracle_rhs / oracle_lhs});
            BOOST_TEST_EQ(u_rhs % lib_lhs, int128{oracle_rhs % oracle_lhs});
        }

        BOOST_TEST_EQ(lib_lhs | u_rhs, int128{static_cast<builtin_i128>(oracle_lhs_u | oracle_rhs_u)});
        BOOST_TEST_EQ(lib_lhs & u_rhs, int128{static_cast<builtin_i128>(oracle_lhs_u & oracle_rhs_u)});
        BOOST_TEST_EQ(lib_lhs ^ u_rhs, int128{static_cast<builtin_i128>(oracle_lhs_u ^ oracle_rhs_u)});
    }
}

// =========================================================================
// uint128 vs int128 (cross-type)
// =========================================================================

void test_cross_type()
{
    for (std::size_t i {0}; i < N; ++i)
    {
        const auto raw_u {random_value<builtin_u128>()};
        const auto raw_i {random_value<builtin_i128>()};
        const uint128 lib_u {raw_u};
        const int128 lib_i {raw_i};

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

        BOOST_TEST_EQ(lib_u + lib_i, uint128{oracle_u + oracle_i});
        BOOST_TEST_EQ(lib_u - lib_i, uint128{oracle_u - oracle_i});
        BOOST_TEST_EQ(lib_u * lib_i, uint128{oracle_u * oracle_i});
        if (oracle_i != 0)
        {
            BOOST_TEST_EQ(lib_u / lib_i, uint128{oracle_u / oracle_i});
            BOOST_TEST_EQ(lib_u % lib_i, uint128{oracle_u % oracle_i});
        }
        if (oracle_u != 0)
        {
            BOOST_TEST_EQ(lib_i / lib_u, uint128{oracle_i / oracle_u});
            BOOST_TEST_EQ(lib_i % lib_u, uint128{oracle_i % oracle_u});
        }

        // Bitwise: same-rank, signed -> unsigned, result uint128
        BOOST_TEST_EQ(lib_u | lib_i, uint128{oracle_u | oracle_i});
        BOOST_TEST_EQ(lib_u & lib_i, uint128{oracle_u & oracle_i});
        BOOST_TEST_EQ(lib_u ^ lib_i, uint128{oracle_u ^ oracle_i});
        BOOST_TEST_EQ(lib_i | lib_u, uint128{oracle_i | oracle_u});
        BOOST_TEST_EQ(lib_i & lib_u, uint128{oracle_i & oracle_u});
        BOOST_TEST_EQ(lib_i ^ lib_u, uint128{oracle_i ^ oracle_u});

        // Shifts: result type follows LHS. Compute the int128 left-shift via
        // the unsigned domain (well-defined wrap-around) and reinterpret as
        // signed to avoid UB when `raw_i` is negative or the result overflows.
        const std::uint64_t shift_amount {static_cast<std::uint64_t>(rng()) % 128};
        const uint128 lib_u_shift {shift_amount};
        const int128 lib_i_shift {static_cast<std::int64_t>(shift_amount)};
        const builtin_u128 raw_i_u = static_cast<builtin_u128>(raw_i);
        BOOST_TEST_EQ(lib_i << lib_u_shift, int128{static_cast<builtin_i128>(raw_i_u << shift_amount)});
        BOOST_TEST_EQ(lib_u << lib_i_shift, uint128{raw_u << shift_amount});
        BOOST_TEST_EQ(lib_i >> lib_u_shift, int128{raw_i >> shift_amount});
        BOOST_TEST_EQ(lib_u >> lib_i_shift, uint128{raw_u >> shift_amount});
    }
}

// =========================================================================
// uint128 vs builtin_i128
// =========================================================================

void test_uint128_vs_builtin_i128()
{
    for (std::size_t i {0}; i < N; ++i)
    {
        const auto raw_u {random_value<builtin_u128>()};
        const auto raw_i {random_value<builtin_i128>()};
        const uint128 lib_u {raw_u};

        const builtin_u128 oracle_u {raw_u};
        const builtin_u128 oracle_i = static_cast<builtin_u128>(raw_i);

        BOOST_TEST_EQ(lib_u == raw_i, oracle_u == oracle_i);
        BOOST_TEST_EQ(lib_u != raw_i, oracle_u != oracle_i);
        BOOST_TEST_EQ(lib_u <  raw_i, oracle_u <  oracle_i);
        BOOST_TEST_EQ(lib_u <= raw_i, oracle_u <= oracle_i);
        BOOST_TEST_EQ(lib_u >  raw_i, oracle_u >  oracle_i);
        BOOST_TEST_EQ(lib_u >= raw_i, oracle_u >= oracle_i);

        BOOST_TEST_EQ(lib_u + raw_i, uint128{oracle_u + oracle_i});
        BOOST_TEST_EQ(lib_u - raw_i, uint128{oracle_u - oracle_i});
        BOOST_TEST_EQ(lib_u * raw_i, uint128{oracle_u * oracle_i});
        if (oracle_i != 0)
        {
            BOOST_TEST_EQ(lib_u / raw_i, uint128{oracle_u / oracle_i});
            BOOST_TEST_EQ(lib_u % raw_i, uint128{oracle_u % oracle_i});
        }

        BOOST_TEST_EQ(lib_u | raw_i, uint128{oracle_u | oracle_i});
        BOOST_TEST_EQ(lib_u & raw_i, uint128{oracle_u & oracle_i});
        BOOST_TEST_EQ(lib_u ^ raw_i, uint128{oracle_u ^ oracle_i});

        const unsigned shift_amount {static_cast<unsigned>(rng() % 128)};
        BOOST_TEST_EQ(lib_u << shift_amount, uint128{oracle_u << shift_amount});
        BOOST_TEST_EQ(lib_u >> shift_amount, uint128{oracle_u >> shift_amount});
    }
}

// =========================================================================
// int128 vs builtin_u128
// =========================================================================

void test_int128_vs_builtin_u128()
{
    for (std::size_t i {0}; i < N; ++i)
    {
        const auto raw_i {random_value<builtin_i128>()};
        const auto raw_u {random_value<builtin_u128>()};
        const int128 lib_i {raw_i};

        // Both promote to unsigned __int128 (same rank, signed -> unsigned).
        // Result of arithmetic is unsigned __int128 (uint128 in library form).
        const builtin_u128 oracle_i = static_cast<builtin_u128>(raw_i);
        const builtin_u128 oracle_u {raw_u};

        BOOST_TEST_EQ(lib_i == raw_u, oracle_i == oracle_u);
        BOOST_TEST_EQ(lib_i != raw_u, oracle_i != oracle_u);
        BOOST_TEST_EQ(lib_i <  raw_u, oracle_i <  oracle_u);
        BOOST_TEST_EQ(lib_i <= raw_u, oracle_i <= oracle_u);
        BOOST_TEST_EQ(lib_i >  raw_u, oracle_i >  oracle_u);
        BOOST_TEST_EQ(lib_i >= raw_u, oracle_i >= oracle_u);

        BOOST_TEST_EQ(lib_i + raw_u, uint128{oracle_i + oracle_u});
        BOOST_TEST_EQ(lib_i - raw_u, uint128{oracle_i - oracle_u});
        BOOST_TEST_EQ(lib_i * raw_u, uint128{oracle_i * oracle_u});
        if (oracle_u != 0)
        {
            BOOST_TEST_EQ(lib_i / raw_u, uint128{oracle_i / oracle_u});
            BOOST_TEST_EQ(lib_i % raw_u, uint128{oracle_i % oracle_u});
        }

        BOOST_TEST_EQ(lib_i | raw_u, uint128{oracle_i | oracle_u});
        BOOST_TEST_EQ(lib_i & raw_u, uint128{oracle_i & oracle_u});
        BOOST_TEST_EQ(lib_i ^ raw_u, uint128{oracle_i ^ oracle_u});

        // Shifts: result type follows LHS (int128 for `lib_i << count`).
        // Left shift: compute via unsigned (well-defined wrap-around) and
        // reinterpret as signed, since `signed << count` overflowing or
        // shifting a negative value is UB pre-C++20 (UBSan flags it).
        // Right shift: keep signed for arithmetic-shift semantics.
        const unsigned shift_amount {static_cast<unsigned>(rng() % 128)};
        const builtin_u128 raw_i_u = static_cast<builtin_u128>(raw_i);
        BOOST_TEST_EQ(lib_i << shift_amount, int128{static_cast<builtin_i128>(raw_i_u << shift_amount)});
        BOOST_TEST_EQ(lib_i >> shift_amount, int128{raw_i >> shift_amount});
    }
}

// =========================================================================
// Signed boundary values
//
// The random sweeps above rarely land on the values where sign handling is
// easiest to get wrong. Every sign-dependent codepath (ordering, arithmetic
// right shift, division, abs, saturation, float conversion) is checked here
// against the builtin over an exhaustive cross product of boundary values.
// =========================================================================

template <typename Float>
static bool same_bits(const Float lhs, const Float rhs) noexcept
{
    unsigned char l[sizeof(Float)] {};
    unsigned char r[sizeof(Float)] {};
    std::memcpy(l, &lhs, sizeof(Float));
    std::memcpy(r, &rhs, sizeof(Float));
    return std::memcmp(l, r, sizeof(Float)) == 0;
}

void test_signed_boundaries()
{
    const builtin_i128 one {1};
    const builtin_u128 top_bit {static_cast<builtin_u128>(1) << 127};

    const builtin_i128 values[] {
        0,
        1,
        -1,
        2,
        -2,
        static_cast<builtin_i128>(top_bit),                      // INT128_MIN
        static_cast<builtin_i128>(top_bit) + 1,                  // INT128_MIN + 1
        static_cast<builtin_i128>(~top_bit),                     // INT128_MAX
        static_cast<builtin_i128>(~top_bit) - 1,
        static_cast<builtin_i128>(static_cast<builtin_u128>(UINT64_MAX)),
        -static_cast<builtin_i128>(static_cast<builtin_u128>(UINT64_MAX)),
        static_cast<builtin_i128>(one << 64),
        -static_cast<builtin_i128>(one << 64),
        static_cast<builtin_i128>(one << 63),
        -static_cast<builtin_i128>(one << 63),
        static_cast<builtin_i128>(static_cast<builtin_u128>(INT64_MIN) << 64)
    };

    constexpr std::size_t count {sizeof(values) / sizeof(values[0])};

    for (std::size_t i {0}; i < count; ++i)
    {
        const auto raw_a {values[i]};
        const int128 a {raw_a};

        // Unary and conversion.
        BOOST_TEST_EQ(-a, int128{static_cast<builtin_i128>(0U - static_cast<builtin_u128>(raw_a))});
        BOOST_TEST_EQ(~a, int128{static_cast<builtin_i128>(~static_cast<builtin_u128>(raw_a))});
        BOOST_TEST_EQ(int128{static_cast<builtin_i128>(a)}, int128{raw_a});
        BOOST_TEST_EQ(a.signed_high(), static_cast<std::int64_t>(static_cast<builtin_u128>(raw_a) >> 64));

        // abs(INT128_MIN) has no representable result; it wraps, like the builtin.
        BOOST_TEST_EQ(boost::int128::abs(a),
                      int128{static_cast<builtin_i128>(raw_a < 0 ? 0U - static_cast<builtin_u128>(raw_a)
                                                                   : static_cast<builtin_u128>(raw_a))});

        // Float conversion is the other place the sign is applied by hand. Compared
        // as bit patterns: both sides round identically, and this test builds with
        // -Wfloat-equal, which a pragma here could not suppress inside BOOST_TEST_EQ.
        BOOST_TEST(same_bits(static_cast<double>(a), static_cast<double>(raw_a)));
        BOOST_TEST(same_bits(static_cast<float>(a), static_cast<float>(raw_a)));

        // Arithmetic right shift fills with the sign bit at every distance.
        for (unsigned shift {0}; shift < 128U; ++shift)
        {
            BOOST_TEST_EQ(a >> shift, int128{raw_a >> shift});
            BOOST_TEST_EQ(a << shift,
                          int128{static_cast<builtin_i128>(static_cast<builtin_u128>(raw_a) << shift)});
        }

        for (std::size_t j {0}; j < count; ++j)
        {
            const auto raw_b {values[j]};
            const int128 b {raw_b};

            BOOST_TEST_EQ(a == b, raw_a == raw_b);
            BOOST_TEST_EQ(a != b, raw_a != raw_b);
            BOOST_TEST_EQ(a <  b, raw_a <  raw_b);
            BOOST_TEST_EQ(a <= b, raw_a <= raw_b);
            BOOST_TEST_EQ(a >  b, raw_a >  raw_b);
            BOOST_TEST_EQ(a >= b, raw_a >= raw_b);

            // Computed through the unsigned domain: signed overflow is UB for the oracle.
            const builtin_u128 ua {static_cast<builtin_u128>(raw_a)};
            const builtin_u128 ub {static_cast<builtin_u128>(raw_b)};
            BOOST_TEST_EQ(a + b, int128{static_cast<builtin_i128>(ua + ub)});
            BOOST_TEST_EQ(a - b, int128{static_cast<builtin_i128>(ua - ub)});
            BOOST_TEST_EQ(a * b, int128{static_cast<builtin_i128>(ua * ub)});

            // INT128_MIN / -1 overflows; the builtin traps on it, so skip that pair.
            const bool overflowing_div {raw_b == -1 && raw_a == static_cast<builtin_i128>(top_bit)};
            if (raw_b != 0 && !overflowing_div)
            {
                BOOST_TEST_EQ(a / b, int128{raw_a / raw_b});
                BOOST_TEST_EQ(a % b, int128{raw_a % raw_b});

                const auto qr {boost::int128::div(a, b)};
                BOOST_TEST_EQ(qr.quot, int128{raw_a / raw_b});
                BOOST_TEST_EQ(qr.rem, int128{raw_a % raw_b});
            }

            BOOST_TEST_EQ(a | b, int128{static_cast<builtin_i128>(ua | ub)});
            BOOST_TEST_EQ(a & b, int128{static_cast<builtin_i128>(ua & ub)});
            BOOST_TEST_EQ(a ^ b, int128{static_cast<builtin_i128>(ua ^ ub)});

            // Saturating arithmetic clamps instead of wrapping.
            const builtin_i128 sum {static_cast<builtin_i128>(ua + ub)};
            const bool add_overflowed {(raw_a < 0) == (raw_b < 0) && (sum < 0) != (raw_a < 0)};
            const builtin_i128 expected_add {add_overflowed
                ? (raw_a < 0 ? static_cast<builtin_i128>(top_bit) : static_cast<builtin_i128>(~top_bit))
                : sum};
            BOOST_TEST_EQ(boost::int128::saturating_add(a, b), int128{expected_add});

            const builtin_i128 diff {static_cast<builtin_i128>(ua - ub)};
            const bool sub_overflowed {(raw_a < 0) != (raw_b < 0) && (diff < 0) != (raw_a < 0)};
            const builtin_i128 expected_sub {sub_overflowed
                ? (raw_a < 0 ? static_cast<builtin_i128>(top_bit) : static_cast<builtin_i128>(~top_bit))
                : diff};
            BOOST_TEST_EQ(boost::int128::saturating_sub(a, b), int128{expected_sub});

            // midpoint rounds toward the first argument. Neither expression below can
            // overflow: b - a is representable when the signs agree, and a + b is
            // representable when they differ.
            builtin_i128 mid {};
            if ((raw_a < 0) == (raw_b < 0))
            {
                mid = raw_a + (raw_b - raw_a) / 2;
            }
            else
            {
                const builtin_i128 straddling_sum {raw_a + raw_b};
                mid = (straddling_sum >> 1) + ((straddling_sum & 1) != 0 && raw_a > raw_b ? 1 : 0);
            }
            BOOST_TEST_EQ(boost::int128::midpoint(a, b), int128{mid});
        }
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
    test_signed_boundaries();

    #endif

    return boost::report_errors();
}
