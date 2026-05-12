// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128.hpp>
#include <boost/core/lightweight_test.hpp>
#include <type_traits>
#include <utility>

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
    BOOST_TEST_EQ(as_double, 42.0);

    const uint128_t u {0U, 100U};
    const unsigned int as_uint = u;
    BOOST_TEST_EQ(as_uint, 100U);

    const float as_float = u;
    BOOST_TEST_EQ(as_float, 100.0f);

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

int main()
{
    test_implicit_conversion_traits();
    test_uint_to_int_construction();
    test_int_to_uint_construction();
    test_uint_to_int_assignment();
    test_int_to_uint_assignment();
    test_constexpr_cross_type();
    test_implicit_conversions_runtime();

    return boost::report_errors();
}
