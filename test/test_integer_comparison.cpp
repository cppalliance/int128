// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128.hpp>
#include <boost/core/lightweight_test.hpp>
#include <type_traits>
#include <limits>
#include <random>
#include <cstdint>

using boost::int128::int128;
using boost::int128::uint128;
using boost::int128::cmp_equal;
using boost::int128::cmp_not_equal;
using boost::int128::cmp_less;
using boost::int128::cmp_greater;
using boost::int128::cmp_less_equal;
using boost::int128::cmp_greater_equal;
using boost::int128::in_range;

//
// Known-answer tests: hand-computed expected results that need no oracle and so
// run on every platform, including those without a native 128-bit type.
//
void test_known_answers()
{
    constexpr auto u_max {(std::numeric_limits<uint128>::max)()};
    constexpr auto i_max {(std::numeric_limits<int128>::max)()};
    constexpr auto i_min {(std::numeric_limits<int128>::min)()};

    // The signature case: a large unsigned value against a negative signed one.
    // The built-in operators would convert -1 to 2^128-1 and report equal; the
    // cmp_* functions compare the true mathematical values.
    BOOST_TEST(!cmp_equal(u_max, int128{-1}));
    BOOST_TEST(cmp_not_equal(u_max, int128{-1}));
    BOOST_TEST(cmp_greater(u_max, int128{-1}));
    BOOST_TEST(!cmp_less(u_max, int128{-1}));
    BOOST_TEST(cmp_less(int128{-1}, u_max));
    BOOST_TEST(cmp_greater_equal(u_max, int128{-1}));
    BOOST_TEST(cmp_less_equal(int128{-1}, u_max));

    // uint128 vs builtin
    BOOST_TEST(cmp_equal(uint128{5}, 5));
    BOOST_TEST(cmp_equal(uint128{5}, 5U));
    BOOST_TEST(!cmp_equal(uint128{5}, -5));
    BOOST_TEST(cmp_not_equal(uint128{5}, -5));
    BOOST_TEST(cmp_greater(uint128{5}, -5));
    BOOST_TEST(!cmp_less(uint128{0}, -1));
    BOOST_TEST(cmp_greater(uint128{0}, -1));
    BOOST_TEST(cmp_less(uint128{5}, 10));
    BOOST_TEST(cmp_less_equal(uint128{5}, 5));
    BOOST_TEST(cmp_greater_equal(uint128{5}, 5));

    // int128 vs builtin, both signs
    BOOST_TEST(cmp_less(int128{-5}, 0U));
    BOOST_TEST(cmp_less(int128{-5}, -3));
    BOOST_TEST(cmp_greater(int128{-3}, -5));
    BOOST_TEST(cmp_equal(int128{-5}, -5));
    BOOST_TEST(!cmp_equal(int128{-5}, 5U));
    BOOST_TEST(cmp_less(int128{-1}, 0U));
    BOOST_TEST(cmp_greater_equal(int128{5}, -5));
    BOOST_TEST(cmp_less_equal(int128{-5}, 5U));

    // builtin on the left
    BOOST_TEST(cmp_less(-1, u_max));
    BOOST_TEST(!cmp_greater(-1, uint128{0}));
    BOOST_TEST(cmp_less(-1, uint128{0}));
    BOOST_TEST(!cmp_equal(-1, u_max));
    BOOST_TEST(cmp_greater(5, int128{-5}));
    BOOST_TEST(cmp_equal(5U, uint128{5}));

    // same-type 128-bit
    BOOST_TEST(cmp_equal(uint128{7}, uint128{7}));
    BOOST_TEST(cmp_not_equal(uint128{7}, uint128{8}));
    BOOST_TEST(cmp_less(uint128{7}, uint128{8}));
    BOOST_TEST(cmp_equal(int128{-7}, int128{-7}));
    BOOST_TEST(cmp_less(int128{-7}, int128{-3}));
    BOOST_TEST(cmp_greater(int128{-3}, int128{-7}));

    // extremes across the two 128-bit types
    BOOST_TEST(cmp_less(i_max, u_max));
    BOOST_TEST(cmp_greater(u_max, i_max));
    BOOST_TEST(cmp_less(i_min, uint128{0}));
    BOOST_TEST(cmp_greater(uint128{0}, i_min));
    BOOST_TEST(cmp_equal(uint128{i_max}, i_max));
    BOOST_TEST(!cmp_equal(u_max, i_max));

    // zero comparisons
    BOOST_TEST(cmp_equal(uint128{0}, 0));
    BOOST_TEST(cmp_equal(int128{0}, 0U));
    BOOST_TEST(cmp_equal(int128{0}, uint128{0}));
    BOOST_TEST(cmp_greater_equal(uint128{0}, int128{0}));
    BOOST_TEST(cmp_less_equal(uint128{0}, int128{0}));
}

//
// in_range: the value and the target type R may each be a builtin integer or a
// library 128-bit type.
//
void test_in_range()
{
    constexpr auto u_max {(std::numeric_limits<uint128>::max)()};
    constexpr auto i_max {(std::numeric_limits<int128>::max)()};
    constexpr auto i_min {(std::numeric_limits<int128>::min)()};

    // library value -> narrow builtin target
    BOOST_TEST(in_range<std::int8_t>(int128{127}));
    BOOST_TEST(!in_range<std::int8_t>(int128{128}));
    BOOST_TEST(in_range<std::int8_t>(int128{-128}));
    BOOST_TEST(!in_range<std::int8_t>(int128{-129}));
    BOOST_TEST(!in_range<std::uint8_t>(int128{-1}));
    BOOST_TEST(in_range<std::uint8_t>(uint128{255}));
    BOOST_TEST(!in_range<std::uint8_t>(uint128{256}));

    // library value -> wide builtin target
    BOOST_TEST(!in_range<std::uint64_t>(u_max));
    BOOST_TEST(in_range<std::uint64_t>(uint128{(std::numeric_limits<std::uint64_t>::max)()}));
    BOOST_TEST(!in_range<std::int64_t>(i_max));
    BOOST_TEST(in_range<std::int64_t>(int128{(std::numeric_limits<std::int64_t>::max)()}));
    BOOST_TEST(!in_range<std::int64_t>(u_max));

    // builtin value -> library target (every builtin fits, except negatives in
    // an unsigned target)
    BOOST_TEST(!in_range<uint128>(-1));
    BOOST_TEST(in_range<uint128>(5));
    BOOST_TEST(in_range<int128>(-5));
    BOOST_TEST(in_range<int128>((std::numeric_limits<std::int64_t>::min)()));
    BOOST_TEST(in_range<uint128>((std::numeric_limits<std::uint64_t>::max)()));

    // library value -> library target
    BOOST_TEST(!in_range<int128>(u_max));
    BOOST_TEST(in_range<int128>(uint128{i_max}));
    BOOST_TEST(!in_range<uint128>(int128{-1}));
    BOOST_TEST(in_range<uint128>(i_max));
    BOOST_TEST(in_range<uint128>(u_max));
    BOOST_TEST(in_range<int128>(i_min));
}

//
// Type-constraint tests: the standard permits only genuine signed and unsigned
// integer types. bool, the character types, std::byte, and non-integers are not
// permitted operands.
//
void test_constraints()
{
    using boost::int128::detail::is_valid_comparison_type_v;

    static_assert(is_valid_comparison_type_v<int>,                "int is permitted");
    static_assert(is_valid_comparison_type_v<unsigned>,           "unsigned is permitted");
    static_assert(is_valid_comparison_type_v<signed char>,        "signed char is permitted");
    static_assert(is_valid_comparison_type_v<unsigned char>,      "unsigned char is permitted");
    static_assert(is_valid_comparison_type_v<long long>,          "long long is permitted");
    static_assert(is_valid_comparison_type_v<std::uint64_t>,      "uint64_t is permitted");

    static_assert(!is_valid_comparison_type_v<bool>,              "bool is not permitted");
    static_assert(!is_valid_comparison_type_v<char>,             "char is not permitted");
    static_assert(!is_valid_comparison_type_v<char16_t>,          "char16_t is not permitted");
    static_assert(!is_valid_comparison_type_v<char32_t>,          "char32_t is not permitted");
    static_assert(!is_valid_comparison_type_v<wchar_t>,           "wchar_t is not permitted");
    static_assert(!is_valid_comparison_type_v<double>,            "double is not permitted");
    static_assert(!is_valid_comparison_type_v<int128>,          "int128 is handled by its own overloads");
    static_assert(!is_valid_comparison_type_v<uint128>,         "uint128 is handled by its own overloads");

    #if defined(__cpp_char8_t)
    static_assert(!is_valid_comparison_type_v<char8_t>,           "char8_t is not permitted");
    #endif
}

//
// constexpr usability of every function.
//
void test_constexpr()
{
    constexpr auto u_max {(std::numeric_limits<uint128>::max)()};

    static_assert(cmp_equal(uint128{5}, 5), "");
    static_assert(cmp_not_equal(u_max, int128{-1}), "");
    static_assert(cmp_less(int128{-1}, uint128{0}), "");
    static_assert(cmp_greater(uint128{0}, int128{-1}), "");
    static_assert(cmp_less_equal(int128{-1}, u_max), "");
    static_assert(cmp_greater_equal(uint128{5}, int128{5}), "");
    static_assert(!cmp_equal(u_max, int128{-1}), "");

    static_assert(in_range<std::int8_t>(int128{127}), "");
    static_assert(!in_range<std::uint8_t>(int128{-1}), "");
    static_assert(in_range<uint128>(5), "");
    static_assert(!in_range<int128>(u_max), "");
}

#if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)

//
// Oracle-based randomized testing. Each operand is reduced to (sign, magnitude)
// using native 128-bit casts, and the comparison is decided with native
// unsigned 128-bit compares, which are an independent ground truth for the
// (sign, magnitude) decomposition the library performs internally.
//
// Every call below this point is qualified. MSVC's builtin 128-bit types are
// classes in namespace std rather than fundamental types, so an unqualified call
// with one of them as an operand also finds the C++20 std::cmp_* and std::in_range
// by ADL and is ambiguous. __int128 is a fundamental type and has no associated
// namespace, so the qualification only matters on MSVC, but it is what pins the
// call to the library implementation under test on every platform.
//
using boost::int128::detail::builtin_i128;
using boost::int128::detail::builtin_u128;

static void ref_sign_mag(builtin_i128 v, bool& neg, builtin_u128& mag) noexcept
{
    neg = v < 0;
    // v + 1 first so the negation is exact even at the minimum value.
    mag = neg ? static_cast<builtin_u128>(-(v + 1)) + 1U : static_cast<builtin_u128>(v);
}

static void ref_sign_mag(builtin_u128 v, bool& neg, builtin_u128& mag) noexcept
{
    neg = false;
    mag = v;
}

static void ref_sign_mag(uint128 v, bool& neg, builtin_u128& mag) noexcept
{
    neg = false;
    mag = static_cast<builtin_u128>(v);
}

static void ref_sign_mag(int128 v, bool& neg, builtin_u128& mag) noexcept
{
    ref_sign_mag(static_cast<builtin_i128>(v), neg, mag);
}

template <typename T, std::enable_if_t<std::is_integral<T>::value && std::is_signed<T>::value, int> = 0>
static void ref_sign_mag(T v, bool& neg, builtin_u128& mag) noexcept
{
    ref_sign_mag(static_cast<builtin_i128>(v), neg, mag);
}

template <typename T, std::enable_if_t<std::is_integral<T>::value && std::is_unsigned<T>::value, int> = 0>
static void ref_sign_mag(T v, bool& neg, builtin_u128& mag) noexcept
{
    ref_sign_mag(static_cast<builtin_u128>(v), neg, mag);
}

static bool truth_equal(bool na, builtin_u128 ma, bool nb, builtin_u128 mb) noexcept
{
    return na == nb && ma == mb;
}

static bool truth_less(bool na, builtin_u128 ma, bool nb, builtin_u128 mb) noexcept
{
    if (na != nb)
    {
        return na;
    }

    return na ? (ma > mb) : (ma < mb);
}

template <typename A, typename B>
static void check_pair(A a, B b)
{
    bool na {}; bool nb {};
    builtin_u128 ma {}; builtin_u128 mb {};
    ref_sign_mag(a, na, ma);
    ref_sign_mag(b, nb, mb);

    const bool eq {truth_equal(na, ma, nb, mb)};
    const bool a_lt_b {truth_less(na, ma, nb, mb)};
    const bool b_lt_a {truth_less(nb, mb, na, ma)};

    BOOST_TEST_EQ(boost::int128::cmp_equal(a, b), eq);
    BOOST_TEST_EQ(boost::int128::cmp_not_equal(a, b), !eq);
    BOOST_TEST_EQ(boost::int128::cmp_less(a, b), a_lt_b);
    BOOST_TEST_EQ(boost::int128::cmp_greater(a, b), b_lt_a);
    BOOST_TEST_EQ(boost::int128::cmp_less_equal(a, b), !b_lt_a);
    BOOST_TEST_EQ(boost::int128::cmp_greater_equal(a, b), !a_lt_b);
}

void test_random_oracle()
{
    std::mt19937_64 rng {42};
    std::uniform_int_distribution<std::uint64_t> u_dist {0, UINT64_MAX};
    std::uniform_int_distribution<std::int64_t> i_dist {INT64_MIN, INT64_MAX};

    constexpr std::size_t N {4096};

    for (std::size_t i {0}; i < N; ++i)
    {
        const std::uint64_t hi {u_dist(rng)};
        const std::uint64_t lo {u_dist(rng)};
        const std::uint64_t hi2 {u_dist(rng)};
        const std::uint64_t lo2 {u_dist(rng)};

        const uint128 ua {hi, lo};
        const uint128 ub {hi2, lo2};
        const int128 ia {static_cast<std::int64_t>(hi), lo};
        const int128 ib {static_cast<std::int64_t>(hi2), lo2};

        const std::int64_t si {i_dist(rng)};
        const std::uint64_t su {u_dist(rng)};

        const builtin_u128 bua {static_cast<builtin_u128>(ua)};
        const builtin_i128 bia {static_cast<builtin_i128>(ia)};

        // library 128-bit vs library 128-bit, every signedness combination
        check_pair(ua, ub);
        check_pair(ia, ib);
        check_pair(ua, ib);
        check_pair(ia, ub);

        // library 128-bit vs builtin integer
        check_pair(ua, si);
        check_pair(ua, su);
        check_pair(ia, si);
        check_pair(ia, su);

        // builtin integer vs library 128-bit
        check_pair(si, ua);
        check_pair(su, ua);
        check_pair(si, ia);
        check_pair(su, ia);

        // builtin 128-bit types in every position
        check_pair(bua, bia);
        check_pair(bia, bua);
        check_pair(bua, ib);
        check_pair(ia, bua);
        check_pair(bia, si);
        check_pair(su, bia);
    }
}

//
// Known-answer coverage for the builtin/extended 128-bit types.
//
void test_builtin_128()
{
    const builtin_u128 bu_max {static_cast<builtin_u128>(-1)};   // 2^128 - 1
    const builtin_i128 bi_neg1 {-1};
    const builtin_i128 bi_5 {5};
    const builtin_u128 bu_5 {5};

    constexpr auto u_max {(std::numeric_limits<uint128>::max)()};
    constexpr auto i_max {(std::numeric_limits<int128>::max)()};

    // trait classification
    static_assert(boost::int128::detail::is_int128ype_v<builtin_i128>,             "builtin signed is a 128-bit type");
    static_assert(boost::int128::detail::is_int128ype_v<builtin_u128>,             "builtin unsigned is a 128-bit type");
    static_assert(boost::int128::detail::is_valid_comparison_operand_v<builtin_i128>, "builtin signed is an operand");
    static_assert(boost::int128::detail::is_valid_comparison_operand_v<builtin_u128>, "builtin unsigned is an operand");

    // library 128-bit vs builtin 128-bit
    BOOST_TEST(!boost::int128::cmp_equal(u_max, bi_neg1));
    BOOST_TEST(!boost::int128::cmp_equal(bu_max, int128{-1}));
    BOOST_TEST(boost::int128::cmp_less(bi_neg1, uint128{0}));
    BOOST_TEST(boost::int128::cmp_greater(uint128{0}, bi_neg1));
    BOOST_TEST(boost::int128::cmp_equal(uint128{bu_max}, bu_max));

    // builtin 128-bit vs builtin 128-bit, cross signedness
    BOOST_TEST(!boost::int128::cmp_equal(bu_max, bi_neg1));
    BOOST_TEST(boost::int128::cmp_less(bi_neg1, bu_max));
    BOOST_TEST(boost::int128::cmp_greater(bu_max, bi_neg1));
    BOOST_TEST(boost::int128::cmp_equal(bi_5, bu_5));
    BOOST_TEST(boost::int128::cmp_not_equal(bi_neg1, bu_5));

    // builtin 128-bit vs standard integer
    BOOST_TEST(boost::int128::cmp_less(bi_neg1, 5U));
    BOOST_TEST(boost::int128::cmp_equal(bi_5, 5));
    BOOST_TEST(boost::int128::cmp_equal(bu_5, 5));
    BOOST_TEST(boost::int128::cmp_greater(5, bi_neg1));
    BOOST_TEST(boost::int128::cmp_less_equal(bi_neg1, 0U));

    // in_range with a builtin 128-bit target and/or value
    BOOST_TEST(!boost::int128::in_range<builtin_i128>(u_max));
    BOOST_TEST(!boost::int128::in_range<builtin_u128>(int128{-1}));
    BOOST_TEST(!boost::int128::in_range<std::uint8_t>(bu_max));
    BOOST_TEST(boost::int128::in_range<std::int8_t>(bi_neg1));
    BOOST_TEST(boost::int128::in_range<builtin_u128>(bi_5));
    BOOST_TEST(boost::int128::in_range<builtin_i128>(i_max));
}

#endif // builtin 128-bit types

int main()
{
    test_known_answers();
    test_in_range();
    test_constraints();
    test_constexpr();

    #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)
    test_random_oracle();
    test_builtin_128();
    #endif

    return boost::report_errors();
}
