// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// Validates the low-level division building blocks in detail/common_div.hpp:
//   * udiv_2by1 / divlu : 128/64 -> 64-bit quotient + remainder
//   * div3by2           : 128/128 (divisor >= 2^64) -> single 64-bit quotient + 128-bit remainder
//
// div3by2 is cross-checked against the independent 32-bit-limb Knuth Algorithm D
// (impl::knuth_divide), which is a completely separate implementation, so this check is valid
// on every platform. Where a native 128-bit integer exists it is also used as an oracle.

#include <boost/int128/int128.hpp>
#include <boost/int128/cstdlib.hpp>
#include <boost/int128/iostream.hpp>
#include <boost/int128/detail/common_div.hpp>
#include <boost/core/lightweight_test.hpp>
#include <random>
#include <cstdint>

using namespace boost::int128;

static std::mt19937_64 rng(0xC0FFEEULL);
static std::uniform_int_distribution<std::uint64_t> dist(0, UINT64_MAX);

// Independent oracle for 128/128 division with divisor >= 2^64, using the 32-bit-limb
// Knuth Algorithm D that the library retains. Returns quotient and remainder.
static void knuth_oracle(const std::uint64_t uh, const std::uint64_t ul,
                         const std::uint64_t vh, const std::uint64_t vl,
                         uint128_t& quot, uint128_t& rem)
{
    const uint128_t u_val {uh, ul};
    const uint128_t v_val {vh, vl};

    if (u_val < v_val)
    {
        quot = uint128_t{UINT64_C(0)};
        rem = u_val;
        return;
    }

    std::uint32_t u[4] {};
    std::uint32_t v[4] {};
    std::uint32_t q[4] {};

    const auto m {detail::impl::to_words(u_val, u)};
    const auto n {detail::impl::to_words(v_val, v)};

    detail::impl::knuth_divide<true>(u, m, v, n, q);

    quot = detail::impl::from_words<uint128_t>(q);
    rem = detail::impl::from_words<uint128_t>(u);
}

static void check_div3by2(const std::uint64_t uh, const std::uint64_t ul,
                          const std::uint64_t vh, const std::uint64_t vl)
{
    std::uint64_t rem_hi {};
    std::uint64_t rem_lo {};
    const auto q {detail::div3by2<true>(uh, ul, vh, vl, rem_hi, rem_lo)};

    uint128_t expected_q {};
    uint128_t expected_r {};
    knuth_oracle(uh, ul, vh, vl, expected_q, expected_r);

    // The quotient always fits in 64 bits when the divisor is >= 2^64
    BOOST_TEST_EQ(expected_q.high, UINT64_C(0));
    BOOST_TEST_EQ(q, expected_q.low);
    BOOST_TEST_EQ(uint128_t(rem_hi, rem_lo), expected_r);
}

static void test_div3by2_random()
{
    constexpr int iters {2000000};
    for (int i {}; i < iters; ++i)
    {
        const auto uh {dist(rng)};
        const auto ul {dist(rng)};
        auto vh {dist(rng)};
        const auto vl {dist(rng)};

        if (vh == 0)
        {
            vh = 1; // divisor must be >= 2^64 for div3by2
        }

        check_div3by2(uh, ul, vh, vl);
    }
}

static void test_div3by2_edges()
{
    const std::uint64_t test_words[] {
        UINT64_C(0), UINT64_C(1), UINT64_C(2), UINT64_C(3),
        UINT64_C(0x7FFFFFFFFFFFFFFF), UINT64_C(0x8000000000000000),
        UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0x0123456789ABCDEF),
        UINT64_C(0xFFFFFFFF), UINT64_C(0x100000000)
    };

    for (const auto vh : test_words)
    {
        if (vh == 0)
        {
            continue; // div3by2 requires vh != 0
        }

        for (const auto vl : test_words)
        {
            for (const auto uh : test_words)
            {
                for (const auto ul : test_words)
                {
                    check_div3by2(uh, ul, vh, vl);
                }
            }
        }
    }

    // abs(INT128_MIN) == 2^127 as a dividend, divided by a range of >= 2^64 divisors
    for (const auto vl : test_words)
    {
        check_div3by2(UINT64_C(0x8000000000000000), UINT64_C(0), UINT64_C(0x8000000000000001), vl);
        check_div3by2(UINT64_C(0x8000000000000000), UINT64_C(0), UINT64_C(0xFFFFFFFFFFFFFFFF), vl);
    }
}

#if defined(BOOST_INT128_HAS_INT128)

// Construct dividends of the exact form V*q + offset to stress the correction / add-back path,
// where the single-digit quotient estimate is most likely to be one too large.
static void test_div3by2_boundary()
{
    const std::uint64_t div_hi[] {
        UINT64_C(1), UINT64_C(0x8000000000000000), UINT64_C(0xFFFFFFFFFFFFFFFF),
        UINT64_C(0x0123456789ABCDEF), UINT64_C(0x00000000FFFFFFFF)
    };
    const std::uint64_t div_lo[] {
        UINT64_C(0), UINT64_C(1), UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xDEADBEEFCAFEBABE)
    };
    const std::uint64_t quotients[] {
        UINT64_C(1), UINT64_C(2), UINT64_C(7), UINT64_C(0xFFFFFFFF),
        UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0x8000000000000000), UINT64_C(0x123456789)
    };

    for (const auto vh : div_hi)
    {
        for (const auto vl : div_lo)
        {
            const detail::builtin_u128 v_val {(static_cast<detail::builtin_u128>(vh) << 64) | vl};

            for (const auto q : quotients)
            {
                // offsets just below the divisor are where the maximum remainder lives
                const detail::builtin_u128 offsets[] {
                    detail::builtin_u128{0}, detail::builtin_u128{1}, v_val - 1, v_val >> 1
                };

                for (const auto off : offsets)
                {
                    const detail::builtin_u128 prod {v_val * q};

                    // skip combinations where V*q already overflows 128 bits
                    if (q != 0 && (prod / q) != v_val)
                    {
                        continue;
                    }

                    const detail::builtin_u128 u_val {prod + off};
                    if (u_val < prod)
                    {
                        continue; // offset pushed us past 2^128
                    }

                    const auto uh {static_cast<std::uint64_t>(u_val >> 64)};
                    const auto ul {static_cast<std::uint64_t>(u_val)};

                    std::uint64_t rem_hi {};
                    std::uint64_t rem_lo {};
                    const auto got_q {detail::div3by2<true>(uh, ul, vh, vl, rem_hi, rem_lo)};

                    BOOST_TEST_EQ(got_q, static_cast<std::uint64_t>(u_val / v_val));
                    BOOST_TEST_EQ(uint128_t(rem_hi, rem_lo), static_cast<uint128_t>(u_val % v_val));
                }
            }
        }
    }
}

static void check_2by1(const std::uint64_t u1, const std::uint64_t u0, const std::uint64_t d)
{
    const detail::builtin_u128 full {(static_cast<detail::builtin_u128>(u1) << 64) | u0};
    const auto expected_q {static_cast<std::uint64_t>(full / d)};
    const auto expected_r {static_cast<std::uint64_t>(full % d)};

    std::uint64_t r {};
    const auto q {detail::udiv_2by1(u1, u0, d, r)};
    BOOST_TEST_EQ(q, expected_q);
    BOOST_TEST_EQ(r, expected_r);

    // divlu is the portable fallback that udiv_2by1 uses off x86-64 / MSVC; test it directly too
    std::uint64_t r2 {};
    const auto q2 {detail::divlu(u1, u0, d, r2)};
    BOOST_TEST_EQ(q2, expected_q);
    BOOST_TEST_EQ(r2, expected_r);
}

static void test_udiv_2by1_random()
{
    constexpr int iters {2000000};
    for (int i {}; i < iters; ++i)
    {
        const auto u0 {dist(rng)};
        auto d {dist(rng)};
        if (d == 0)
        {
            d = 1;
        }
        const auto u1 {dist(rng) % d}; // precondition: u1 < d

        check_2by1(u1, u0, d);
    }
}

static void test_udiv_2by1_edges()
{
    const std::uint64_t divisors[] {
        UINT64_C(1), UINT64_C(2), UINT64_C(3), UINT64_C(10),
        UINT64_C(0xFFFFFFFF), UINT64_C(0x100000000), UINT64_C(0x80000000),
        UINT64_C(0x7FFFFFFFFFFFFFFF), UINT64_C(0x8000000000000000), UINT64_C(0xFFFFFFFFFFFFFFFF)
    };
    const std::uint64_t lows[] {
        UINT64_C(0), UINT64_C(1), UINT64_C(0x80000000),
        UINT64_C(0xFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF)
    };

    for (const auto d : divisors)
    {
        for (const auto u0 : lows)
        {
            check_2by1(UINT64_C(0), u0, d);        // u1 == 0
            check_2by1(d - 1, u0, d);              // u1 == d - 1 (maximal)
            if (d > 1)
            {
                check_2by1(d / 2, u0, d);
            }
        }
    }
}

#endif // BOOST_INT128_HAS_INT128

int main()
{
    test_div3by2_random();
    test_div3by2_edges();

    #if defined(BOOST_INT128_HAS_INT128)

    test_div3by2_boundary();
    test_udiv_2by1_random();
    test_udiv_2by1_edges();

    #endif

    return boost::report_errors();
}
