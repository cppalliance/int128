// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#if defined(__GNUC__) && __GNUC__ == 7 && defined(__i386__)

// 32-bit GCC-7 fails with: "error: constexpr loop iteration count exceeds limit of 262144"

int main() { return 0; }

#else

#ifndef BOOST_INT128_BUILD_MODULE

#include <boost/int128.hpp>

#else

import boost.int128;

#endif

#include <boost/core/lightweight_test.hpp>
#include <cstdint>
#include <limits>

using namespace boost::int128;

namespace {

// Naive bit-by-bit reference. Independent of the Newton implementation under
// test, so it serves as a ground truth for cross-checking.
constexpr uint128_t isqrt_ref(uint128_t n) noexcept
{
    if (n < 2U)
    {
        return n;
    }

    uint128_t res {0};
    uint128_t bit {uint128_t{1} << 126};

    while (bit > n)
    {
        bit >>= 2;
    }

    while (bit != 0U)
    {
        if (n >= res + bit)
        {
            n -= res + bit;
            res = (res >> 1) + bit;
        }
        else
        {
            res >>= 1;
        }

        bit >>= 2;
    }

    return res;
}

// Verify the defining invariant: r == isqrt(n) iff r*r <= n < (r+1)^2.
// Computed with overflow-safe comparisons so callers can pass values near the
// uint128 upper bound.
void check_invariant(const uint128_t n)
{
    const uint128_t r {isqrt(n)};

    BOOST_TEST(r * r <= n);

    const uint128_t r_plus_1 {r + 1U};

    // (r+1)^2 may overflow uint128 when r is close to 2^64; in that case the
    // invariant n < (r+1)^2 is trivially satisfied since n fits in 128 bits.
    if (r_plus_1 != 0U && r_plus_1 <= ((std::numeric_limits<uint128_t>::max)() / r_plus_1))
    {
        BOOST_TEST(n < r_plus_1 * r_plus_1);
    }
}

} // namespace

void test_uint128_isqrt_small()
{
    BOOST_TEST_EQ(isqrt(uint128_t{0}), uint128_t{0});
    BOOST_TEST_EQ(isqrt(uint128_t{1}), uint128_t{1});
    BOOST_TEST_EQ(isqrt(uint128_t{2}), uint128_t{1});
    BOOST_TEST_EQ(isqrt(uint128_t{3}), uint128_t{1});
    BOOST_TEST_EQ(isqrt(uint128_t{4}), uint128_t{2});
    BOOST_TEST_EQ(isqrt(uint128_t{5}), uint128_t{2});
    BOOST_TEST_EQ(isqrt(uint128_t{8}), uint128_t{2});
    BOOST_TEST_EQ(isqrt(uint128_t{9}), uint128_t{3});
    BOOST_TEST_EQ(isqrt(uint128_t{10}), uint128_t{3});
    BOOST_TEST_EQ(isqrt(uint128_t{15}), uint128_t{3});
    BOOST_TEST_EQ(isqrt(uint128_t{16}), uint128_t{4});
    BOOST_TEST_EQ(isqrt(uint128_t{99}), uint128_t{9});
    BOOST_TEST_EQ(isqrt(uint128_t{100}), uint128_t{10});
    BOOST_TEST_EQ(isqrt(uint128_t{101}), uint128_t{10});

    // Exhaustive cross-check against the reference for every small n.
    for (std::uint64_t i {0}; i < 200; ++i)
    {
        BOOST_TEST_EQ(isqrt(uint128_t{i}), isqrt_ref(uint128_t{i}));
    }
}

void test_uint128_isqrt_perfect_squares()
{
    // Squares spanning the full 64-bit range, including the largest k whose
    // square still fits in 128 bits (k = 2^64 - 1).
    for (std::uint64_t k {0}; k < 10000; ++k)
    {
        const uint128_t kk {k};
        BOOST_TEST_EQ(isqrt(kk * kk), kk);

        if (k > 0)
        {
            BOOST_TEST_EQ(isqrt(kk * kk - 1U), kk - 1U);
            BOOST_TEST_EQ(isqrt(kk * kk + 2U * kk), kk);  // (k+1)^2 - 1
        }
    }

    // Powers of two as bases: k = 2^i for i in [0, 63] - largest exact square
    // is (2^63)^2 = 2^126.
    for (int i {0}; i < 64; ++i)
    {
        const uint128_t k {uint128_t{1} << i};
        BOOST_TEST_EQ(isqrt(k * k), k);
    }

    // Largest representable perfect square: (2^64 - 1)^2 = 2^128 - 2^65 + 1.
    const uint128_t k_max {(std::numeric_limits<std::uint64_t>::max)()};
    BOOST_TEST_EQ(isqrt(k_max * k_max), k_max);
}

void test_uint128_isqrt_bit_boundaries()
{
    // 2^(2k) has integer square root 2^k.
    for (int k {0}; k < 64; ++k)
    {
        const uint128_t n {uint128_t{1} << (2 * k)};
        BOOST_TEST_EQ(isqrt(n), uint128_t{1} << k);
    }

    // 2^(2k+1) has integer square root floor(2^(k+0.5)) = floor(sqrt(2) * 2^k).
    // Check the invariant holds rather than hard-coding the value.
    for (int k {0}; k < 63; ++k)
    {
        check_invariant(uint128_t{1} << (2 * k + 1));
    }

    // Just below and just above bit boundaries.
    for (int k {2}; k < 128; ++k)
    {
        const uint128_t boundary {uint128_t{1} << k};
        check_invariant(boundary - 1U);
        check_invariant(boundary);
        check_invariant(boundary + 1U);
    }
}

void test_uint128_isqrt_extreme()
{
    // (uint128 max). (2^64)^2 = 2^128 wraps, so isqrt(2^128 - 1) = 2^64 - 1.
    const uint128_t u128_max {(std::numeric_limits<uint128_t>::max)()};
    const uint128_t u64_max {(std::numeric_limits<std::uint64_t>::max)()};
    BOOST_TEST_EQ(isqrt(u128_max), u64_max);

    // 2^128 - 2^65 + 1 = (2^64 - 1)^2 - exact square at the very top.
    BOOST_TEST_EQ(isqrt(u64_max * u64_max), u64_max);

    // One above the largest representable square: still has isqrt = 2^64 - 1.
    BOOST_TEST_EQ(isqrt(u64_max * u64_max + 1U), u64_max);

    // A handful of large hand-picked values, cross-checked against the bit-by-
    // bit reference.
    const uint128_t big_a {UINT64_C(0x0123456789ABCDEF), UINT64_C(0xFEDCBA9876543210)};
    const uint128_t big_b {UINT64_C(0xDEADBEEFCAFEBABE), UINT64_C(0x0123456789ABCDEF)};
    const uint128_t big_c {UINT64_C(0x8000000000000000), 0U};

    BOOST_TEST_EQ(isqrt(big_a), isqrt_ref(big_a));
    BOOST_TEST_EQ(isqrt(big_b), isqrt_ref(big_b));
    BOOST_TEST_EQ(isqrt(big_c), isqrt_ref(big_c));

    check_invariant(big_a);
    check_invariant(big_b);
    check_invariant(big_c);
    check_invariant(u128_max);
}

void test_int128_isqrt()
{
    BOOST_TEST_EQ(isqrt(int128_t{0}), int128_t{0});
    BOOST_TEST_EQ(isqrt(int128_t{1}), int128_t{1});
    BOOST_TEST_EQ(isqrt(int128_t{2}), int128_t{1});
    BOOST_TEST_EQ(isqrt(int128_t{100}), int128_t{10});
    BOOST_TEST_EQ(isqrt(int128_t{144}), int128_t{12});
    BOOST_TEST_EQ(isqrt(int128_t{INT64_C(1000000000000000000)}), int128_t{INT64_C(1000000000)});

    // int128 max = 2^127 - 1. floor(sqrt) = floor(2^63.5) = 6074001000.7e9
    // Use the unsigned implementation as the source of truth.
    const int128_t i128_max {(std::numeric_limits<int128_t>::max)()};
    BOOST_TEST_EQ(isqrt(i128_max), static_cast<int128_t>(isqrt(static_cast<uint128_t>(i128_max))));

    // Negative inputs are documented to return 0.
    BOOST_TEST_EQ(isqrt(int128_t{-1}), int128_t{0});
    BOOST_TEST_EQ(isqrt(int128_t{-100}), int128_t{0});
    BOOST_TEST_EQ(isqrt((std::numeric_limits<int128_t>::min)()), int128_t{0});
}

void test_isqrt_against_ipow()
{
    // isqrt(ipow(k, 2)) == k for any k whose square fits.
    for (std::uint64_t k {0}; k < 1000; ++k)
    {
        BOOST_TEST_EQ(isqrt(ipow(uint128_t{k}, 2U)), uint128_t{k});
    }

    // isqrt is monotonically non-decreasing.
    uint128_t prev {0};

    for (std::uint64_t i {0}; i < 1000; ++i)
    {
        const uint128_t curr {isqrt(uint128_t{i})};
        BOOST_TEST(curr >= prev);
        prev = curr;
    }
}

void test_constexpr_isqrt()
{
    constexpr uint128_t r1 {isqrt(uint128_t{0})};
    static_assert(r1 == uint128_t{0}, "isqrt(0) constexpr");

    constexpr uint128_t r2 {isqrt(uint128_t{1})};
    static_assert(r2 == uint128_t{1}, "isqrt(1) constexpr");

    constexpr uint128_t r3 {isqrt(uint128_t{100})};
    static_assert(r3 == uint128_t{10}, "isqrt(100) constexpr");

    constexpr uint128_t r4 {isqrt(uint128_t{UINT64_C(1000000000000000000)})};
    static_assert(r4 == uint128_t{UINT64_C(1000000000)}, "isqrt(10^18) constexpr");

    constexpr int128_t r5 {isqrt(int128_t{-5})};
    static_assert(r5 == int128_t{0}, "isqrt negative constexpr");

    constexpr int128_t r6 {isqrt(int128_t{12321})};
    static_assert(r6 == int128_t{111}, "isqrt(12321) constexpr");
}

int main()
{
    test_uint128_isqrt_small();
    test_uint128_isqrt_perfect_squares();
    test_uint128_isqrt_bit_boundaries();
    test_uint128_isqrt_extreme();
    test_int128_isqrt();
    test_isqrt_against_ipow();
    test_constexpr_isqrt();

    return boost::report_errors();
}

#endif
