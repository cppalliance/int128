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
    static_assert(std::is_convertible<int128_t, uint128_t>::value, "int128_t -> uint128_t should be implicit");
    static_assert(std::is_convertible<uint128_t, int128_t>::value, "uint128_t -> int128_t should be implicit");
    static_assert(std::is_assignable<int128_t&, uint128_t>::value, "uint128_t should be assignable to int128_t");
    static_assert(std::is_assignable<uint128_t&, int128_t>::value, "int128_t should be assignable to uint128_t");

    // Implicit conversions to builtin integer types (matches __int128 behavior)
    static_assert(std::is_convertible<int128_t, int>::value, "int128_t -> int should be implicit");
    static_assert(std::is_convertible<int128_t, unsigned int>::value, "int128_t -> unsigned int should be implicit");
    static_assert(std::is_convertible<int128_t, std::int64_t>::value, "int128_t -> int64_t should be implicit");
    static_assert(std::is_convertible<int128_t, std::uint64_t>::value, "int128_t -> uint64_t should be implicit");
    static_assert(std::is_convertible<uint128_t, int>::value, "uint128_t -> int should be implicit");
    static_assert(std::is_convertible<uint128_t, unsigned int>::value, "uint128_t -> unsigned int should be implicit");

    // Implicit conversions to floating-point types
    static_assert(std::is_convertible<int128_t, float>::value, "int128_t -> float should be implicit");
    static_assert(std::is_convertible<int128_t, double>::value, "int128_t -> double should be implicit");
    static_assert(std::is_convertible<uint128_t, float>::value, "uint128_t -> float should be implicit");
    static_assert(std::is_convertible<uint128_t, double>::value, "uint128_t -> double should be implicit");

#if defined(BOOST_INT128_HAS_INT128)
    // Implicit conversions to builtin __int128
    static_assert(std::is_convertible<int128_t, detail::builtin_i128>::value, "int128_t -> __int128 should be implicit");
    static_assert(std::is_convertible<int128_t, detail::builtin_u128>::value, "int128_t -> unsigned __int128 should be implicit");
    static_assert(std::is_convertible<uint128_t, detail::builtin_i128>::value, "uint128_t -> __int128 should be implicit");
    static_assert(std::is_convertible<uint128_t, detail::builtin_u128>::value, "uint128_t -> unsigned __int128 should be implicit");
#endif
}

void test_implicit_conversions_runtime()
{
    const int128_t i {0, 42U};

    const int as_int = i;
    BOOST_TEST_EQ(as_int, 42);

    const std::uint64_t as_u64 = i;
    BOOST_TEST_EQ(as_u64, 42U);

    const double as_double = i;
    BOOST_TEST_EQ(static_cast<int>(as_double), 42);

    const uint128_t u {0U, 100U};
    const unsigned int as_uint = u;
    BOOST_TEST_EQ(as_uint, 100U);

    const float as_float = u;
    BOOST_TEST_EQ(static_cast<int>(as_float), 100);

#if defined(BOOST_INT128_HAS_INT128)
    const detail::builtin_i128 as_native_i = int128_t{1, 2U};
    BOOST_TEST(as_native_i == ((static_cast<detail::builtin_i128>(1) << 64) | 2));

    const detail::builtin_u128 as_native_u = uint128_t{3U, 4U};
    BOOST_TEST(as_native_u == ((static_cast<detail::builtin_u128>(3) << 64) | 4));
#endif
}

void test_uint_to_int_construction()
{
    const uint128_t u {1U, 42U};

    // Copy construction with braces
    const int128_t a {u};
    BOOST_TEST_EQ(a.low, u.low);
    BOOST_TEST_EQ(static_cast<std::uint64_t>(a.high), u.high);

    // Copy-initialization (implicit conversion)
    const int128_t b = u;
    BOOST_TEST_EQ(b.low, u.low);
    BOOST_TEST_EQ(static_cast<std::uint64_t>(b.high), u.high);

    // Move construction
    uint128_t u_movable {1U, 42U};
    const int128_t c {std::move(u_movable)};
    BOOST_TEST_EQ(c.low, 42U);
    BOOST_TEST_EQ(c.high, 1);
}

void test_int_to_uint_construction()
{
    const int128_t i {-1, 0xFFFFFFFFFFFFFFFFULL};

    const uint128_t a {i};
    BOOST_TEST_EQ(a.low, i.low);
    BOOST_TEST_EQ(a.high, static_cast<std::uint64_t>(i.high));

    const uint128_t b = i;
    BOOST_TEST_EQ(b.low, i.low);
    BOOST_TEST_EQ(b.high, static_cast<std::uint64_t>(i.high));

    int128_t i_movable {-1, 0xFFFFFFFFFFFFFFFFULL};
    const uint128_t c {std::move(i_movable)};
    BOOST_TEST_EQ(c.high, 0xFFFFFFFFFFFFFFFFULL);
    BOOST_TEST_EQ(c.low, 0xFFFFFFFFFFFFFFFFULL);
}

void test_uint_to_int_assignment()
{
    const uint128_t u {7U, 99U};

    // Copy assignment via implicit conversion
    int128_t a {};
    a = u;
    BOOST_TEST_EQ(a.low, 99U);
    BOOST_TEST_EQ(a.high, 7);

    // Move assignment via implicit conversion
    int128_t b {};
    b = uint128_t{7U, 99U};
    BOOST_TEST_EQ(b.low, 99U);
    BOOST_TEST_EQ(b.high, 7);
}

void test_int_to_uint_assignment()
{
    const int128_t i {-2, 0x1234U};

    uint128_t a {};
    a = i;
    BOOST_TEST_EQ(a.low, 0x1234U);
    BOOST_TEST_EQ(a.high, static_cast<std::uint64_t>(-2));

    uint128_t b {};
    b = int128_t{-2, 0x1234U};
    BOOST_TEST_EQ(b.low, 0x1234U);
    BOOST_TEST_EQ(b.high, static_cast<std::uint64_t>(-2));
}

void test_constexpr_cross_type()
{
    constexpr uint128_t u {1U, 42U};
    constexpr int128_t a {u};
    static_assert(a.low == 42U, "constexpr cross-type construction");

    constexpr int128_t i {-1, 7U};
    constexpr uint128_t b {i};
    static_assert(b.low == 7U, "constexpr cross-type construction");
}

template <typename Float>
void test_uint_from_float()
{
    // Basic positive values
    BOOST_TEST_EQ(uint128_t{Float{0}}.low, 0U);
    BOOST_TEST_EQ(uint128_t{Float{0}}.high, 0U);
    BOOST_TEST_EQ(uint128_t{Float{42}}.low, 42U);
    BOOST_TEST_EQ(uint128_t{Float{42}}.high, 0U);

    // Truncation toward zero. Use Float{N}/Float{D} rather than a double literal
    // so the test compiles cleanly for float and long double without precision warnings.
    BOOST_TEST_EQ((uint128_t{Float{37} / Float{10}}.low), 3U);  // ~3.7 -> 3
    BOOST_TEST_EQ((uint128_t{Float{99} / Float{100}}.low), 0U); // ~0.99 -> 0

    // NaN -> 0
    const Float nan {std::numeric_limits<Float>::quiet_NaN()};
    BOOST_TEST_EQ(uint128_t{nan}.low, 0U);
    BOOST_TEST_EQ(uint128_t{nan}.high, 0U);

    // Negative -> 0 (matches libgcc)
    BOOST_TEST_EQ(uint128_t{Float{-1}}.low, 0U);
    BOOST_TEST_EQ(uint128_t{Float{-1}}.high, 0U);

    // Saturation on overflow: infinity (or any value >= 2^128) -> UINT128_MAX.
    // For float, 2^128 itself is +infinity since the 8-bit exponent saturates.
    const Float two_64 {static_cast<Float>(UINT64_C(1) << 32) * static_cast<Float>(UINT64_C(1) << 32)};
    const uint128_t saturated {std::numeric_limits<Float>::infinity()};
    BOOST_TEST_EQ(saturated.low, UINT64_MAX);
    BOOST_TEST_EQ(saturated.high, UINT64_MAX);

    // 2^127 should fit (representable in float, double, long double)
    const Float two_127 {two_64 * static_cast<Float>(UINT64_C(1) << 63)};
    const uint128_t large {two_127};
    BOOST_TEST_EQ(large.low, 0U);
    BOOST_TEST_EQ(large.high, UINT64_C(1) << 63);

    // Round-trip for an exactly representable mid-range value
    const Float round_trip_src {two_64};  // 2^64
    const uint128_t round_trip {round_trip_src};
    BOOST_TEST_EQ(round_trip.low, 0U);
    BOOST_TEST_EQ(round_trip.high, 1U);
}

template <typename Float>
void test_int_from_float()
{
    // Basic positive and negative
    BOOST_TEST_EQ(int128_t{Float{0}}.low, 0U);
    BOOST_TEST_EQ(int128_t{Float{0}}.high, 0);
    BOOST_TEST_EQ(int128_t{Float{42}}.low, 42U);
    BOOST_TEST_EQ(int128_t{Float{-42}}.low, static_cast<std::uint64_t>(-42));
    BOOST_TEST_EQ(int128_t{Float{-42}}.high, -1);

    // Truncation toward zero (see note in test_uint_from_float on the literal style).
    BOOST_TEST_EQ((int128_t{Float{37} / Float{10}}.low), 3U);                                 // ~3.7 -> 3
    BOOST_TEST_EQ((int128_t{Float{-37} / Float{10}}.low), static_cast<std::uint64_t>(-3));    // ~-3.7 -> -3

    // NaN -> 0
    const Float nan {std::numeric_limits<Float>::quiet_NaN()};
    BOOST_TEST_EQ(int128_t{nan}.low, 0U);
    BOOST_TEST_EQ(int128_t{nan}.high, 0);

    // Positive saturation: f >= 2^127 -> INT128_MAX
    const Float two_64 {static_cast<Float>(UINT64_C(1) << 32) * static_cast<Float>(UINT64_C(1) << 32)};
    const Float two_127 {two_64 * static_cast<Float>(UINT64_C(1) << 63)};
    const int128_t pos_sat {two_127};
    BOOST_TEST_EQ(pos_sat.high, (std::numeric_limits<std::int64_t>::max)());
    BOOST_TEST_EQ(pos_sat.low, UINT64_MAX);

    // Negative saturation: f <= -2^127 -> INT128_MIN
    const int128_t neg_sat {-two_127};
    BOOST_TEST_EQ(neg_sat.high, (std::numeric_limits<std::int64_t>::min)());
    BOOST_TEST_EQ(neg_sat.low, 0U);

    // Just below the positive boundary should not saturate.
    const int128_t near_max {two_127 / Float{2}};  // 2^126
    BOOST_TEST_EQ(near_max.high, UINT64_C(1) << 62);
    BOOST_TEST_EQ(near_max.low, 0U);

    // Round-trip a negative power of two through the two's-complement path
    const int128_t neg_round_trip {-two_64};  // -2^64
    BOOST_TEST_EQ(neg_round_trip.low, 0U);
    BOOST_TEST_EQ(neg_round_trip.high, -1);
}

void test_constexpr_float_construction()
{
    constexpr uint128_t u {42.5};
    static_assert(u.low == 42U, "constexpr uint from double");

    constexpr int128_t i {-7.9};
    static_assert(i.high == -1, "constexpr int from double sign");

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
