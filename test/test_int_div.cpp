// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_BUILD_MODULE

#include <boost/int128.hpp>

#else

import boost.int128;
// The convenience macros are not part of the module interface (macros never are).
#include <boost/int128/detail/literal_macros.hpp>

#endif

#include <boost/core/lightweight_test.hpp>
#include <iostream>
#include <limits>

using namespace boost::int128;

// Reference implementations over long long. The rounding decision is made by comparing
// 2 * abs(remainder) against abs(divisor), which is independent of the shifted comparison
// the library uses.

static int ref_sign(long long x, long long y)
{
    return ((x < 0) != (y < 0)) ? -1 : 1;
}

static long long ref_to_zero(long long x, long long y)
{
    return x / y;
}

static long long ref_away_zero(long long x, long long y)
{
    return x % y != 0 ? x / y + ref_sign(x, y) : x / y;
}

static long long ref_to_pos_inf(long long x, long long y)
{
    return x % y != 0 && ref_sign(x, y) > 0 ? x / y + 1 : x / y;
}

static long long ref_to_neg_inf(long long x, long long y)
{
    return x % y != 0 && ref_sign(x, y) < 0 ? x / y - 1 : x / y;
}

static long long ref_euclid(long long x, long long y)
{
    return x % y < 0 ? x / y + ref_sign(x, y) : x / y;
}

static long long ref_nearest(long long x, long long y, bool tie_magnifies)
{
    const auto rem {x % y};
    const auto abs_rem {rem < 0 ? -rem : rem};
    const auto abs_y {y < 0 ? -y : y};
    const auto doubled {2 * abs_rem};
    const auto magnify {doubled > abs_y || (doubled == abs_y && tie_magnifies)};

    return magnify ? x / y + ref_sign(x, y) : x / y;
}

static long long ref_ties_to_zero(long long x, long long y)
{
    return ref_nearest(x, y, false);
}

static long long ref_ties_away_zero(long long x, long long y)
{
    return ref_nearest(x, y, true);
}

static long long ref_ties_to_pos_inf(long long x, long long y)
{
    return ref_nearest(x, y, ref_sign(x, y) > 0);
}

static long long ref_ties_to_neg_inf(long long x, long long y)
{
    return ref_nearest(x, y, ref_sign(x, y) < 0);
}

static long long ref_ties_to_odd(long long x, long long y)
{
    return ref_nearest(x, y, (x / y) % 2 == 0);
}

static long long ref_ties_to_even(long long x, long long y)
{
    return ref_nearest(x, y, (x / y) % 2 != 0);
}

// One row per rounding mode. The overload set is resolved by the target pointer type.
struct mode_entry
{
    const char* name;
    long long (*reference)(long long, long long);
    int128_t (*signed_quotient)(int128_t, int128_t);
    div_result<int128_t> (*signed_div_rem)(int128_t, int128_t);
    uint128_t (*unsigned_quotient)(uint128_t, uint128_t);
    div_result<uint128_t> (*unsigned_div_rem)(uint128_t, uint128_t);
    bool nearest;
};

static const mode_entry modes[]
{
    {"to_zero", ref_to_zero, div_to_zero, div_rem_to_zero, div_to_zero, div_rem_to_zero, false},
    {"away_zero", ref_away_zero, div_away_zero, div_rem_away_zero, div_away_zero, div_rem_away_zero, false},
    {"to_pos_inf", ref_to_pos_inf, div_to_pos_inf, div_rem_to_pos_inf, div_to_pos_inf, div_rem_to_pos_inf, false},
    {"to_neg_inf", ref_to_neg_inf, div_to_neg_inf, div_rem_to_neg_inf, div_to_neg_inf, div_rem_to_neg_inf, false},
    {"euclid", ref_euclid, div_euclid, div_rem_euclid, div_euclid, div_rem_euclid, false},
    {"ties_to_zero", ref_ties_to_zero, div_ties_to_zero, div_rem_ties_to_zero, div_ties_to_zero, div_rem_ties_to_zero, true},
    {"ties_away_zero", ref_ties_away_zero, div_ties_away_zero, div_rem_ties_away_zero, div_ties_away_zero, div_rem_ties_away_zero, true},
    {"ties_to_pos_inf", ref_ties_to_pos_inf, div_ties_to_pos_inf, div_rem_ties_to_pos_inf, div_ties_to_pos_inf, div_rem_ties_to_pos_inf, true},
    {"ties_to_neg_inf", ref_ties_to_neg_inf, div_ties_to_neg_inf, div_rem_ties_to_neg_inf, div_ties_to_neg_inf, div_rem_ties_to_neg_inf, true},
    {"ties_to_odd", ref_ties_to_odd, div_ties_to_odd, div_rem_ties_to_odd, div_ties_to_odd, div_rem_ties_to_odd, true},
    {"ties_to_even", ref_ties_to_even, div_ties_to_even, div_rem_ties_to_even, div_ties_to_even, div_rem_ties_to_even, true}
};

static void context(const mode_entry& mode, long long x, long long y)
{
    std::cerr << "    mode: " << mode.name << " x: " << x << " y: " << y << std::endl;
}

// Every small signed pair, against the reference
static void test_signed_sweep()
{
    for (long long x {-64}; x <= 64; ++x)
    {
        for (long long y {-24}; y <= 24; ++y)
        {
            if (y == 0)
            {
                continue;
            }

            for (const auto& mode : modes)
            {
                const int128_t wide_x {x};
                const int128_t wide_y {y};
                const auto expected {mode.reference(x, y)};

                if (!BOOST_TEST_EQ(mode.signed_quotient(wide_x, wide_y), int128_t{expected}))
                {
                    context(mode, x, y);
                }

                const auto res {mode.signed_div_rem(wide_x, wide_y)};

                if (!BOOST_TEST_EQ(res.quotient, int128_t{expected}))
                {
                    context(mode, x, y);
                }

                if (!BOOST_TEST_EQ(res.remainder, int128_t{x - expected * y}))
                {
                    context(mode, x, y);
                }
            }

            const auto rem {x % y};
            const auto expected_rem {rem < 0 ? rem + (y < 0 ? -y : y) : rem};
            BOOST_TEST_EQ(rem_euclid(int128_t{x}, int128_t{y}), int128_t{expected_rem});
        }
    }
}

// Every small unsigned pair, against the same reference
static void test_unsigned_sweep()
{
    for (long long x {0}; x <= 64; ++x)
    {
        for (long long y {1}; y <= 24; ++y)
        {
            for (const auto& mode : modes)
            {
                const uint128_t wide_x {static_cast<unsigned long long>(x)};
                const uint128_t wide_y {static_cast<unsigned long long>(y)};
                const auto expected {mode.reference(x, y)};
                const uint128_t wide_expected {static_cast<unsigned long long>(expected)};

                if (!BOOST_TEST_EQ(mode.unsigned_quotient(wide_x, wide_y), wide_expected))
                {
                    context(mode, x, y);
                }

                const auto res {mode.unsigned_div_rem(wide_x, wide_y)};

                if (!BOOST_TEST_EQ(res.quotient, wide_expected))
                {
                    context(mode, x, y);
                }

                // The remainder of a rounded up unsigned quotient is negative, and is
                // returned reduced modulo 2^128
                if (!BOOST_TEST_EQ(res.remainder, uint128_t{wide_x - wide_expected * wide_y}))
                {
                    context(mode, x, y);
                }
            }

            BOOST_TEST_EQ(rem_euclid(uint128_t{static_cast<unsigned long long>(x)}, uint128_t{static_cast<unsigned long long>(y)}),
                          uint128_t{static_cast<unsigned long long>(x % y)});
        }
    }
}

// The quotient does not change when both operands are scaled by the same power of two,
// which puts the full 128-bit division path under the same reference
static void test_scaled_sweep()
{
    const int128_t scale {int128_t{1} << 70};

    for (long long x {-40}; x <= 40; ++x)
    {
        for (long long y {-13}; y <= 13; ++y)
        {
            if (y == 0)
            {
                continue;
            }

            for (const auto& mode : modes)
            {
                const int128_t wide_x {int128_t{x} * scale};
                const int128_t wide_y {int128_t{y} * scale};
                const auto expected {mode.reference(x, y)};
                const auto res {mode.signed_div_rem(wide_x, wide_y)};

                if (!BOOST_TEST_EQ(res.quotient, int128_t{expected}))
                {
                    context(mode, x, y);
                }

                if (!BOOST_TEST_EQ(res.remainder, wide_x - int128_t{expected} * wide_y))
                {
                    context(mode, x, y);
                }
            }
        }
    }
}

// Properties that hold for arbitrary operands, checked on values with no small equivalent
static void test_signed_invariants()
{
    constexpr auto int_min {(std::numeric_limits<int128_t>::min)()};
    constexpr auto int_max {(std::numeric_limits<int128_t>::max)()};

    const int128_t dividends[]
    {
        int_max, int_min, int_min + 1, int_max - 1,
        BOOST_INT128_INT128_C(-170141183460469231731687303715884105727),
        BOOST_INT128_INT128_C(99999999999999999999999999999999999999),
        BOOST_INT128_INT128_C(-12345678901234567890123456789012345678),
        int128_t{1} << 100, -(int128_t{1} << 100)
    };

    const int128_t divisors[]
    {
        int128_t{1}, int128_t{-1}, int128_t{2}, int128_t{-2}, int128_t{3}, int128_t{-3},
        int128_t{1} << 64, -(int128_t{1} << 64), int_max, int_min,
        BOOST_INT128_INT128_C(1000000000000000000000000000000000000)
    };

    for (const auto x : dividends)
    {
        for (const auto y : divisors)
        {
            // INT128_MIN / -1 is not representable, so operator/ itself is undefined there
            if (x == int_min && y == -1)
            {
                continue;
            }

            const auto truncated {boost::int128::div(x, y)};

            for (const auto& mode : modes)
            {
                const auto res {mode.signed_div_rem(x, y)};

                BOOST_TEST_EQ(res.quotient, mode.signed_quotient(x, y));

                // The quotient is the truncated one, or one further from zero
                const auto delta {res.quotient - truncated.quot};
                BOOST_TEST(delta == 0 || delta == 1 || delta == -1);

                // x == quotient * y + remainder, evaluated modulo 2^128 so that no
                // intermediate overflows
                const uint128_t unsigned_x {x.high, x.low};
                const uint128_t unsigned_y {y.high, y.low};
                const uint128_t unsigned_quot {res.quotient.high, res.quotient.low};
                const uint128_t unsigned_rem {res.remainder.high, res.remainder.low};
                const uint128_t reconstructed {unsigned_quot * unsigned_y + unsigned_rem};
                BOOST_TEST_EQ(unsigned_x, reconstructed);

                // abs(remainder) < abs(y)
                const auto abs_rem {static_cast<uint128_t>(boost::int128::abs(res.remainder))};
                const auto abs_y {static_cast<uint128_t>(boost::int128::abs(y))};
                BOOST_TEST(abs_rem < abs_y);

                // A nearest mode never leaves more than half the divisor behind
                if (mode.nearest)
                {
                    BOOST_TEST(abs_rem <= (abs_y >> 1U));
                }
            }

            // Ordering of the directed modes
            BOOST_TEST(div_to_neg_inf(x, y) <= div_to_zero(x, y));
            BOOST_TEST(div_to_zero(x, y) <= div_to_pos_inf(x, y));
            BOOST_TEST(div_to_neg_inf(x, y) <= div_away_zero(x, y));
            BOOST_TEST(div_away_zero(x, y) <= div_to_pos_inf(x, y));

            // The Euclidean remainder is always non-negative and below abs(y)
            const auto euclid_rem {rem_euclid(x, y)};
            BOOST_TEST(euclid_rem >= 0);
            BOOST_TEST(static_cast<uint128_t>(euclid_rem) < static_cast<uint128_t>(boost::int128::abs(y)));
            BOOST_TEST_EQ(euclid_rem, div_rem_euclid(x, y).remainder);
        }
    }
}

static void test_unsigned_invariants()
{
    constexpr auto uint_max {(std::numeric_limits<uint128_t>::max)()};

    const uint128_t dividends[]
    {
        uint_max, uint_max - 1U, uint128_t{1} << 127U, uint128_t{1} << 100U,
        BOOST_INT128_UINT128_C(340282366920938463463374607431768211451),
        BOOST_INT128_UINT128_C(99999999999999999999999999999999999999)
    };

    const uint128_t divisors[]
    {
        uint128_t{1}, uint128_t{2}, uint128_t{3}, uint128_t{1} << 64U, uint_max,
        BOOST_INT128_UINT128_C(1000000000000000000000000000000000000)
    };

    for (const auto x : dividends)
    {
        for (const auto y : divisors)
        {
            const auto truncated {boost::int128::div(x, y)};

            for (const auto& mode : modes)
            {
                const auto res {mode.unsigned_div_rem(x, y)};

                BOOST_TEST_EQ(res.quotient, mode.unsigned_quotient(x, y));

                const auto delta {res.quotient - truncated.quot};
                BOOST_TEST(delta == 0U || delta == 1U);

                // x == quotient * y + remainder modulo 2^128, including when the
                // remainder has wrapped
                const uint128_t reconstructed {res.quotient * y + res.remainder};
                BOOST_TEST_EQ(x, reconstructed);

                if (res.quotient == truncated.quot)
                {
                    BOOST_TEST(res.remainder < y);

                    if (mode.nearest)
                    {
                        BOOST_TEST(res.remainder <= (y >> 1U));
                    }
                }
            }

            // Truncation, rounding down, and Euclidean division all agree for unsigned
            BOOST_TEST_EQ(div_to_zero(x, y), truncated.quot);
            BOOST_TEST_EQ(div_to_neg_inf(x, y), truncated.quot);
            BOOST_TEST_EQ(div_euclid(x, y), truncated.quot);
            BOOST_TEST_EQ(rem_euclid(x, y), truncated.rem);
        }
    }
}

// The divisor whose magnitude is not representable as a positive int128_t
static void test_int128_min_divisor()
{
    constexpr auto int_min {(std::numeric_limits<int128_t>::min)()};
    constexpr auto int_max {(std::numeric_limits<int128_t>::max)()};

    // -3 / INT128_MIN is a tiny positive fraction: only the modes that round up in
    // magnitude leave zero
    BOOST_TEST_EQ(div_to_zero(int128_t{-3}, int_min), int128_t{0});
    BOOST_TEST_EQ(div_away_zero(int128_t{-3}, int_min), int128_t{1});
    BOOST_TEST_EQ(div_to_pos_inf(int128_t{-3}, int_min), int128_t{1});
    BOOST_TEST_EQ(div_to_neg_inf(int128_t{-3}, int_min), int128_t{0});
    BOOST_TEST_EQ(div_euclid(int128_t{-3}, int_min), int128_t{1});
    BOOST_TEST_EQ(div_ties_to_zero(int128_t{-3}, int_min), int128_t{0});
    BOOST_TEST_EQ(div_ties_away_zero(int128_t{-3}, int_min), int128_t{0});
    BOOST_TEST_EQ(div_ties_to_even(int128_t{-3}, int_min), int128_t{0});

    // The matching remainder is -3 - 1 * INT128_MIN, which is 2^127 - 3
    BOOST_TEST_EQ(div_rem_euclid(int128_t{-3}, int_min).remainder, int_max - 2);
    BOOST_TEST_EQ(rem_euclid(int128_t{-3}, int_min), int_max - 2);
    BOOST_TEST_EQ(div_rem_to_zero(int128_t{-3}, int_min).remainder, int128_t{-3});

    // INT128_MIN / 2 is exact, so no mode adjusts it
    for (const auto& mode : modes)
    {
        BOOST_TEST_EQ(mode.signed_quotient(int_min, int128_t{2}), int_min / 2);
        BOOST_TEST_EQ(mode.signed_div_rem(int_min, int128_t{2}).remainder, int128_t{0});
    }

    // A quotient at the limits of the type is never rounded further
    for (const auto& mode : modes)
    {
        BOOST_TEST_EQ(mode.signed_quotient(int_max, int128_t{1}), int_max);
        BOOST_TEST_EQ(mode.signed_quotient(int_min, int128_t{1}), int_min);
        BOOST_TEST_EQ(mode.signed_quotient(int_max, int128_t{-1}), -int_max);
    }

    // 2^127 is congruent to 2 modulo 7, so INT128_MIN is congruent to 5
    BOOST_TEST_EQ(rem_euclid(int_min, int128_t{7}), int128_t{5});
    BOOST_TEST_EQ(rem_euclid(int_min, int128_t{-7}), int128_t{5});
}

static void test_unsigned_limits()
{
    constexpr auto uint_max {(std::numeric_limits<uint128_t>::max)()};

    // No mode overflows a quotient that is already the maximum
    for (const auto& mode : modes)
    {
        BOOST_TEST_EQ(mode.unsigned_quotient(uint_max, uint128_t{1}), uint_max);
    }

    // UINT128_MAX is odd, so halving it is a tie in neither direction
    BOOST_TEST_EQ(div_ties_to_zero(uint_max, uint128_t{2}), uint_max / 2U);
    BOOST_TEST_EQ(div_ties_away_zero(uint_max, uint128_t{2}), uint_max / 2U + 1U);
    BOOST_TEST_EQ(div_away_zero(uint_max, uint128_t{2}), uint_max / 2U + 1U);
    BOOST_TEST_EQ(div_to_zero(uint_max, uint128_t{2}), uint_max / 2U);

    // A tie against an even divisor at the top of the range
    const uint128_t even_tie {uint_max - 1U};
    BOOST_TEST_EQ(div_ties_to_zero(even_tie, uint128_t{2}), even_tie / 2U);
    BOOST_TEST_EQ(div_ties_away_zero(even_tie, uint128_t{2}), even_tie / 2U);
}

// The worked example from P3724
static void test_paper_example()
{
    const int128_t x {-12};
    const int128_t y {5};

    BOOST_TEST_EQ(div_to_zero(x, y), int128_t{-2});
    BOOST_TEST_EQ(div_away_zero(x, y), int128_t{-3});
    BOOST_TEST_EQ(div_to_pos_inf(x, y), int128_t{-2});
    BOOST_TEST_EQ(div_to_neg_inf(x, y), int128_t{-3});
    BOOST_TEST_EQ(div_euclid(x, y), int128_t{-3});
    BOOST_TEST_EQ(div_ties_to_zero(x, y), int128_t{-2});
    BOOST_TEST_EQ(div_ties_away_zero(x, y), int128_t{-2});
    BOOST_TEST_EQ(div_ties_to_pos_inf(x, y), int128_t{-2});
    BOOST_TEST_EQ(div_ties_to_neg_inf(x, y), int128_t{-2});
    BOOST_TEST_EQ(div_ties_to_odd(x, y), int128_t{-2});
    BOOST_TEST_EQ(div_ties_to_even(x, y), int128_t{-2});

    BOOST_TEST_EQ(div_rem_to_zero(x, y).remainder, int128_t{-2});
    BOOST_TEST_EQ(div_rem_to_neg_inf(x, y).remainder, int128_t{3});
    BOOST_TEST_EQ(rem_euclid(x, y), int128_t{3});
}

// Every tie-breaking rule on the same exact tie
static void test_tie_breaking()
{
    // 7 / 2 and -7 / 2 are ties, and the truncated quotients are 3 and -3
    BOOST_TEST_EQ(div_ties_to_zero(int128_t{7}, int128_t{2}), int128_t{3});
    BOOST_TEST_EQ(div_ties_away_zero(int128_t{7}, int128_t{2}), int128_t{4});
    BOOST_TEST_EQ(div_ties_to_pos_inf(int128_t{7}, int128_t{2}), int128_t{4});
    BOOST_TEST_EQ(div_ties_to_neg_inf(int128_t{7}, int128_t{2}), int128_t{3});
    BOOST_TEST_EQ(div_ties_to_odd(int128_t{7}, int128_t{2}), int128_t{3});
    BOOST_TEST_EQ(div_ties_to_even(int128_t{7}, int128_t{2}), int128_t{4});

    BOOST_TEST_EQ(div_ties_to_zero(int128_t{-7}, int128_t{2}), int128_t{-3});
    BOOST_TEST_EQ(div_ties_away_zero(int128_t{-7}, int128_t{2}), int128_t{-4});
    BOOST_TEST_EQ(div_ties_to_pos_inf(int128_t{-7}, int128_t{2}), int128_t{-3});
    BOOST_TEST_EQ(div_ties_to_neg_inf(int128_t{-7}, int128_t{2}), int128_t{-4});
    BOOST_TEST_EQ(div_ties_to_odd(int128_t{-7}, int128_t{2}), int128_t{-3});
    BOOST_TEST_EQ(div_ties_to_even(int128_t{-7}, int128_t{2}), int128_t{-4});

    // 5 / 2 has an even quotient after rounding away from zero
    BOOST_TEST_EQ(div_ties_to_odd(int128_t{5}, int128_t{2}), int128_t{3});
    BOOST_TEST_EQ(div_ties_to_even(int128_t{5}, int128_t{2}), int128_t{2});

    BOOST_TEST_EQ(div_ties_to_zero(uint128_t{7}, uint128_t{2}), uint128_t{3});
    BOOST_TEST_EQ(div_ties_away_zero(uint128_t{7}, uint128_t{2}), uint128_t{4});
    BOOST_TEST_EQ(div_ties_to_pos_inf(uint128_t{7}, uint128_t{2}), uint128_t{4});
    BOOST_TEST_EQ(div_ties_to_neg_inf(uint128_t{7}, uint128_t{2}), uint128_t{3});
    BOOST_TEST_EQ(div_ties_to_odd(uint128_t{7}, uint128_t{2}), uint128_t{3});
    BOOST_TEST_EQ(div_ties_to_even(uint128_t{7}, uint128_t{2}), uint128_t{4});
}

static void test_div_result()
{
    const div_result<int128_t> a {int128_t{3}, int128_t{1}};
    const div_result<int128_t> b {int128_t{3}, int128_t{2}};
    const div_result<int128_t> c {int128_t{4}, int128_t{1}};

    BOOST_TEST(a == a);
    BOOST_TEST(a != b);
    BOOST_TEST(!(a == b));
    BOOST_TEST(a == div_rem_to_zero(int128_t{10}, int128_t{3}));

    const div_result<uint128_t> ua {uint128_t{3}, uint128_t{1}};
    BOOST_TEST(ua == div_rem_to_zero(uint128_t{10}, uint128_t{3}));

    #ifdef BOOST_INT128_HAS_SPACESHIP_OPERATOR

    BOOST_TEST(a < b);
    BOOST_TEST(b < c);
    BOOST_TEST(c > a);
    BOOST_TEST(a <= a);
    BOOST_TEST((a <=> a) == std::strong_ordering::equal);
    BOOST_TEST((ua <=> div_result<uint128_t>{uint128_t{3}, uint128_t{2}}) == std::strong_ordering::less);

    #else

    static_cast<void>(c);

    #endif
}

static void test_constexpr()
{
    static_assert(div_to_zero(int128_t{-7}, int128_t{2}) == -3, "div_to_zero");
    static_assert(div_away_zero(int128_t{-7}, int128_t{2}) == -4, "div_away_zero");
    static_assert(div_to_pos_inf(int128_t{-7}, int128_t{2}) == -3, "div_to_pos_inf");
    static_assert(div_to_neg_inf(int128_t{-7}, int128_t{2}) == -4, "div_to_neg_inf");
    static_assert(div_euclid(int128_t{-7}, int128_t{2}) == -4, "div_euclid");
    static_assert(div_ties_to_zero(int128_t{-7}, int128_t{2}) == -3, "div_ties_to_zero");
    static_assert(div_ties_away_zero(int128_t{-7}, int128_t{2}) == -4, "div_ties_away_zero");
    static_assert(div_ties_to_pos_inf(int128_t{-7}, int128_t{2}) == -3, "div_ties_to_pos_inf");
    static_assert(div_ties_to_neg_inf(int128_t{-7}, int128_t{2}) == -4, "div_ties_to_neg_inf");
    static_assert(div_ties_to_odd(int128_t{-7}, int128_t{2}) == -3, "div_ties_to_odd");
    static_assert(div_ties_to_even(int128_t{-7}, int128_t{2}) == -4, "div_ties_to_even");
    static_assert(rem_euclid(int128_t{-7}, int128_t{2}) == 1, "rem_euclid");

    static_assert(div_rem_to_neg_inf(int128_t{-7}, int128_t{2}).remainder == 1, "div_rem_to_neg_inf");
    static_assert(div_rem_euclid(int128_t{-7}, int128_t{2}).quotient == -4, "div_rem_euclid");

    static_assert(div_to_zero(uint128_t{7}, uint128_t{2}) == 3U, "unsigned div_to_zero");
    static_assert(div_away_zero(uint128_t{7}, uint128_t{2}) == 4U, "unsigned div_away_zero");
    static_assert(div_euclid(uint128_t{7}, uint128_t{2}) == 3U, "unsigned div_euclid");
    static_assert(rem_euclid(uint128_t{7}, uint128_t{2}) == 1U, "unsigned rem_euclid");
    static_assert(div_rem_ties_to_even(uint128_t{7}, uint128_t{2}).quotient == 4U, "unsigned ties_to_even");
}

int main()
{
    test_signed_sweep();
    test_unsigned_sweep();
    test_scaled_sweep();
    test_signed_invariants();
    test_unsigned_invariants();
    test_int128_min_divisor();
    test_unsigned_limits();
    test_paper_example();
    test_tie_breaking();
    test_div_result();
    test_constexpr();

    return boost::report_errors();
}
