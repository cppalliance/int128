// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128.hpp>
#include <boost/core/lightweight_test.hpp>
#include <type_traits>
#include <utility>
#include <limits>
#include <cmath>

using namespace boost::int128;

void test_implicit_conversion_traits()
{
    static_assert(std::is_convertible<int128, uint128>::value, "int128 -> uint128 should be implicit");
    static_assert(std::is_convertible<uint128, int128>::value, "uint128 -> int128 should be implicit");
    static_assert(std::is_assignable<int128&, uint128>::value, "uint128 should be assignable to int128");
    static_assert(std::is_assignable<uint128&, int128>::value, "int128 should be assignable to uint128");

    // Implicit conversions to builtin integer types (matches __int128 behavior)
    static_assert(std::is_convertible<int128, int>::value, "int128 -> int should be implicit");
    static_assert(std::is_convertible<int128, unsigned int>::value, "int128 -> unsigned int should be implicit");
    static_assert(std::is_convertible<int128, std::int64_t>::value, "int128 -> int64_t should be implicit");
    static_assert(std::is_convertible<int128, std::uint64_t>::value, "int128 -> uint64_t should be implicit");
    static_assert(std::is_convertible<uint128, int>::value, "uint128 -> int should be implicit");
    static_assert(std::is_convertible<uint128, unsigned int>::value, "uint128 -> unsigned int should be implicit");

    // Implicit conversions to floating-point types
    static_assert(std::is_convertible<int128, float>::value, "int128 -> float should be implicit");
    static_assert(std::is_convertible<int128, double>::value, "int128 -> double should be implicit");
    static_assert(std::is_convertible<uint128, float>::value, "uint128 -> float should be implicit");
    static_assert(std::is_convertible<uint128, double>::value, "uint128 -> double should be implicit");

#if defined(BOOST_INT128_HAS_INT128)
    // Implicit conversions to builtin __int128
    static_assert(std::is_convertible<int128, detail::builtin_i128>::value, "int128 -> __int128 should be implicit");
    static_assert(std::is_convertible<int128, detail::builtin_u128>::value, "int128 -> unsigned __int128 should be implicit");
    static_assert(std::is_convertible<uint128, detail::builtin_i128>::value, "uint128 -> __int128 should be implicit");
    static_assert(std::is_convertible<uint128, detail::builtin_u128>::value, "uint128 -> unsigned __int128 should be implicit");
#endif
}

void test_implicit_conversions_runtime()
{
    const int128 i {0, 42U};

    const int as_int = i;
    BOOST_TEST_EQ(as_int, 42);

    const std::uint64_t as_u64 = i;
    BOOST_TEST_EQ(as_u64, 42U);

    const double as_double = i;
    BOOST_TEST_EQ(static_cast<int>(as_double), 42);

    const uint128 u {0U, 100U};
    const unsigned int as_uint = u;
    BOOST_TEST_EQ(as_uint, 100U);

    const float as_float = u;
    BOOST_TEST_EQ(static_cast<int>(as_float), 100);

#if defined(BOOST_INT128_HAS_INT128)
    const detail::builtin_i128 as_native_i = int128{1, 2U};
    BOOST_TEST(as_native_i == ((static_cast<detail::builtin_i128>(1) << 64) | 2));

    const detail::builtin_u128 as_native_u = uint128{3U, 4U};
    BOOST_TEST(as_native_u == ((static_cast<detail::builtin_u128>(3) << 64) | 4));
#endif
}

void test_uint_to_int_construction()
{
    const uint128 u {1U, 42U};

    // Copy construction with braces
    const int128 a {u};
    BOOST_TEST_EQ(a.low, u.low);
    BOOST_TEST_EQ(a.high, u.high);

    // Copy-initialization (implicit conversion)
    const int128 b = u;
    BOOST_TEST_EQ(b.low, u.low);
    BOOST_TEST_EQ(b.high, u.high);

    // Move construction
    uint128 u_movable {1U, 42U};
    const int128 c {std::move(u_movable)};
    BOOST_TEST_EQ(c.low, 42U);
    BOOST_TEST_EQ(c.high, UINT64_C(1));
}

void test_int_to_uint_construction()
{
    const int128 i {-1, 0xFFFFFFFFFFFFFFFFULL};

    const uint128 a {i};
    BOOST_TEST_EQ(a.low, i.low);
    BOOST_TEST_EQ(a.high, i.high);

    const uint128 b = i;
    BOOST_TEST_EQ(b.low, i.low);
    BOOST_TEST_EQ(b.high, i.high);

    int128 i_movable {-1, 0xFFFFFFFFFFFFFFFFULL};
    const uint128 c {std::move(i_movable)};
    BOOST_TEST_EQ(c.high, 0xFFFFFFFFFFFFFFFFULL);
    BOOST_TEST_EQ(c.low, 0xFFFFFFFFFFFFFFFFULL);
}

void test_uint_to_int_assignment()
{
    const uint128 u {7U, 99U};

    // Copy assignment via implicit conversion
    int128 a {};
    a = u;
    BOOST_TEST_EQ(a.low, 99U);
    BOOST_TEST_EQ(a.high, 7);

    // Move assignment via implicit conversion
    int128 b {};
    b = uint128{7U, 99U};
    BOOST_TEST_EQ(b.low, 99U);
    BOOST_TEST_EQ(b.high, 7);
}

void test_int_to_uint_assignment()
{
    const int128 i {-2, 0x1234U};

    uint128 a {};
    a = i;
    BOOST_TEST_EQ(a.low, 0x1234U);
    BOOST_TEST_EQ(a.high, static_cast<std::uint64_t>(-2));

    uint128 b {};
    b = int128{-2, 0x1234U};
    BOOST_TEST_EQ(b.low, 0x1234U);
    BOOST_TEST_EQ(b.high, static_cast<std::uint64_t>(-2));
}

void test_constexpr_cross_type()
{
    constexpr uint128 u {1U, 42U};
    constexpr int128 a {u};
    static_assert(a.low == 42U, "constexpr cross-type construction");

    constexpr int128 i {-1, 7U};
    constexpr uint128 b {i};
    static_assert(b.low == 7U, "constexpr cross-type construction");
}

template <typename Float>
void test_uint_from_float()
{
    // Basic positive values
    BOOST_TEST_EQ(uint128{Float{0}}.low, 0U);
    BOOST_TEST_EQ(uint128{Float{0}}.high, 0U);
    BOOST_TEST_EQ(uint128{Float{42}}.low, 42U);
    BOOST_TEST_EQ(uint128{Float{42}}.high, 0U);

    // Truncation toward zero. Use Float{N}/Float{D} rather than a double literal
    // so the test compiles cleanly for float and long double without precision warnings.
    BOOST_TEST_EQ((uint128{Float{37} / Float{10}}.low), 3U);  // ~3.7 -> 3
    BOOST_TEST_EQ((uint128{Float{99} / Float{100}}.low), 0U); // ~0.99 -> 0

    // NaN -> 0
    const Float nan {std::numeric_limits<Float>::quiet_NaN()};
    BOOST_TEST_EQ(uint128{nan}.low, 0U);
    BOOST_TEST_EQ(uint128{nan}.high, 0U);

    // Negative -> 0 (matches libgcc)
    BOOST_TEST_EQ(uint128{Float{-1}}.low, 0U);
    BOOST_TEST_EQ(uint128{Float{-1}}.high, 0U);

    // Saturation on overflow: infinity (or any value >= 2^128) -> UINT128_MAX.
    // For float, 2^128 itself is +infinity since the 8-bit exponent saturates.
    const Float two_64 {static_cast<Float>(UINT64_C(1) << 32) * static_cast<Float>(UINT64_C(1) << 32)};
    const uint128 saturated {std::numeric_limits<Float>::infinity()};
    BOOST_TEST_EQ(saturated.low, UINT64_MAX);
    BOOST_TEST_EQ(saturated.high, UINT64_MAX);

    // 2^127 should fit (representable in float, double, long double)
    const Float two_127 {two_64 * static_cast<Float>(UINT64_C(1) << 63)};
    const uint128 large {two_127};
    BOOST_TEST_EQ(large.low, 0U);
    BOOST_TEST_EQ(large.high, UINT64_C(1) << 63);

    // Round-trip for an exactly representable mid-range value
    const Float round_trip_src {two_64};  // 2^64
    const uint128 round_trip {round_trip_src};
    BOOST_TEST_EQ(round_trip.low, 0U);
    BOOST_TEST_EQ(round_trip.high, 1U);
}

template <typename Float>
void test_int_from_float()
{
    // Basic positive and negative
    BOOST_TEST_EQ(int128{Float{0}}.low, 0U);
    BOOST_TEST_EQ(int128{Float{0}}.signed_high(), INT64_C(0));
    BOOST_TEST_EQ(int128{Float{42}}.low, 42U);
    BOOST_TEST_EQ(int128{Float{-42}}.low, static_cast<std::uint64_t>(-42));
    BOOST_TEST_EQ(int128{Float{-42}}.signed_high(), INT64_C(-1));

    // Truncation toward zero (see note in test_uint_from_float on the literal style).
    BOOST_TEST_EQ((int128{Float{37} / Float{10}}.low), 3U);                                 // ~3.7 -> 3
    BOOST_TEST_EQ((int128{Float{-37} / Float{10}}.low), static_cast<std::uint64_t>(-3));    // ~-3.7 -> -3

    // NaN -> 0
    const Float nan {std::numeric_limits<Float>::quiet_NaN()};
    BOOST_TEST_EQ(int128{nan}.low, 0U);
    BOOST_TEST_EQ(int128{nan}.signed_high(), INT64_C(0));

    // Positive saturation: f >= 2^127 -> INT128_MAX
    const Float two_64 {static_cast<Float>(UINT64_C(1) << 32) * static_cast<Float>(UINT64_C(1) << 32)};
    const Float two_127 {two_64 * static_cast<Float>(UINT64_C(1) << 63)};
    const int128 pos_sat {two_127};
    BOOST_TEST_EQ(pos_sat.signed_high(), (std::numeric_limits<std::int64_t>::max)());
    BOOST_TEST_EQ(pos_sat.low, UINT64_MAX);

    // Negative saturation: f <= -2^127 -> INT128_MIN
    const int128 neg_sat {-two_127};
    BOOST_TEST_EQ(neg_sat.signed_high(), (std::numeric_limits<std::int64_t>::min)());
    BOOST_TEST_EQ(neg_sat.low, 0U);

    // Just below the positive boundary should not saturate.
    const int128 near_max {two_127 / Float{2}};  // 2^126
    BOOST_TEST_EQ(near_max.high, UINT64_C(1) << 62);
    BOOST_TEST_EQ(near_max.low, 0U);

    // Round-trip a negative power of two through the two's-complement path
    const int128 neg_round_trip {-two_64};  // -2^64
    BOOST_TEST_EQ(neg_round_trip.low, 0U);
    BOOST_TEST_EQ(neg_round_trip.signed_high(), INT64_C(-1));
}

void test_constexpr_float_construction()
{
    constexpr uint128 u {42.5};
    static_assert(u.low == 42U, "constexpr uint from double");

    constexpr int128 i {-7.9};
    static_assert(i.signed_high() == -1, "constexpr int from double sign");

    // NaN -> 0 is exercised at runtime in test_uint_from_float / test_int_from_float.
    // It cannot be constant-evaluated on GCC 9, which rejects NaN comparisons in
    // constexpr contexts.
}

int main()
{
    test_implicit_conversion_traits();
    test_uint_to_int_construction();
    test_int_to_uint_construction();
    test_uint_to_int_assignment();
    test_int_to_uint_assignment();
    test_constexpr_cross_type();
    test_implicit_conversions_runtime();

    test_uint_from_float<float>();
    test_uint_from_float<double>();
    test_uint_from_float<long double>();
    test_int_from_float<float>();
    test_int_from_float<double>();
    test_int_from_float<long double>();
    test_constexpr_float_construction();

    return boost::report_errors();
}
