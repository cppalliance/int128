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

// Naive reference implementation used to cross-check the squaring loop.
template <typename T>
constexpr T ipow_ref(T base, std::uint64_t exp) noexcept
{
    T result {1};

    for (std::uint64_t i {0}; i < exp; ++i)
    {
        result *= base;
    }

    return result;
}

} // namespace

void test_uint128_ipow_basic()
{
    BOOST_TEST_EQ(ipow(uint128_t{0}, 0U), uint128_t{1});
    BOOST_TEST_EQ(ipow(uint128_t{1}, 0U), uint128_t{1});
    BOOST_TEST_EQ(ipow(uint128_t{42}, 0U), uint128_t{1});
    BOOST_TEST_EQ(ipow(uint128_t{0}, 1U), uint128_t{0});
    BOOST_TEST_EQ(ipow(uint128_t{0}, 5U), uint128_t{0});
    BOOST_TEST_EQ(ipow(uint128_t{1}, 1000U), uint128_t{1});
    BOOST_TEST_EQ(ipow(uint128_t{42}, 1U), uint128_t{42});

    BOOST_TEST_EQ(ipow(uint128_t{2}, 0U), uint128_t{1});
    BOOST_TEST_EQ(ipow(uint128_t{2}, 1U), uint128_t{2});
    BOOST_TEST_EQ(ipow(uint128_t{2}, 2U), uint128_t{4});
    BOOST_TEST_EQ(ipow(uint128_t{2}, 10U), uint128_t{1024});
    BOOST_TEST_EQ(ipow(uint128_t{3}, 5U), uint128_t{243});
    BOOST_TEST_EQ(ipow(uint128_t{10}, 9U), uint128_t{UINT64_C(1000000000)});
    BOOST_TEST_EQ(ipow(uint128_t{10}, 18U), uint128_t{UINT64_C(1000000000000000000)});
}

void test_uint128_ipow_power_of_two()
{
    // 2^k fills bit k, so we can hit every bit position up to 127.
    for (std::uint64_t k {0}; k < 64; ++k)
    {
        const uint128_t expected {static_cast<std::uint64_t>(1) << k};
        BOOST_TEST_EQ(ipow(uint128_t{2}, k), expected);
    }

    for (std::uint64_t k {64}; k < 128; ++k)
    {
        const uint128_t expected {static_cast<std::uint64_t>(1) << (k - 64), 0U};
        BOOST_TEST_EQ(ipow(uint128_t{2}, k), expected);
    }

    // 2^128 wraps to 0 in uint128 arithmetic.
    BOOST_TEST_EQ(ipow(uint128_t{2}, 128U), uint128_t{0});
    BOOST_TEST_EQ(ipow(uint128_t{2}, 200U), uint128_t{0});
}

void test_uint128_ipow_large()
{
    // 10^38 is the largest power of 10 that fits in 128 bits.
    // 10^38 = 100000000000000000000000000000000000000.
    const uint128_t ten_pow_38 {UINT64_C(0x4B3B4CA85A86C47A), UINT64_C(0x098A224000000000)};
    BOOST_TEST_EQ(ipow(uint128_t{10}, 38U), ten_pow_38);

    // Cross-check a range of bases against the naive reference for small
    // exponents where the result is hand-verifiable through repeated mul.
    for (std::uint64_t base {2}; base < 8; ++base)
    {
        for (std::uint64_t exp {0}; exp < 12; ++exp)
        {
            BOOST_TEST_EQ(ipow(uint128_t{base}, exp), ipow_ref(uint128_t{base}, exp));
        }
    }
}

void test_uint128_ipow_wrap()
{
    // Squaring 2^64 yields 2^128 which wraps to 0.
    const uint128_t two_pow_64 {1U, 0U};
    BOOST_TEST_EQ(ipow(two_pow_64, 2U), uint128_t{0});

    // (2^64 - 1)^2 mod 2^128 = 2^128 - 2^65 + 1, which has a known bit pattern.
    const uint128_t u64_max {(std::numeric_limits<std::uint64_t>::max)()};
    const uint128_t expected {UINT64_C(0xFFFFFFFFFFFFFFFE), 1U};
    BOOST_TEST_EQ(ipow(u64_max, 2U), expected);

    // Anything to a sufficiently large power eventually wraps to 0 if the base
    // shares a factor of 2 with 2^128.
    BOOST_TEST_EQ(ipow(uint128_t{4}, 64U), uint128_t{0});
    BOOST_TEST_EQ(ipow(uint128_t{6}, 200U), uint128_t{0});
}

void test_uint128_ipow_identities()
{
    // a^(b+c) == a^b * a^c (under wrap modulo 2^128).
    const uint128_t a {UINT64_C(0xDEADBEEF)};
    BOOST_TEST_EQ(ipow(a, 7U), ipow(a, 3U) * ipow(a, 4U));
    BOOST_TEST_EQ(ipow(a, 20U), ipow(a, 13U) * ipow(a, 7U));

    // (a*b)^e == a^e * b^e.
    const uint128_t aa {7};
    const uint128_t bb {11};
    BOOST_TEST_EQ(ipow(aa * bb, 6U), ipow(aa, 6U) * ipow(bb, 6U));

    // (a^b)^c == a^(b*c).
    BOOST_TEST_EQ(ipow(ipow(uint128_t{3}, 4U), 5U), ipow(uint128_t{3}, 4U * 5U));
}

void test_int128_ipow_basic()
{
    BOOST_TEST_EQ(ipow(int128_t{0}, 0U), int128_t{1});
    BOOST_TEST_EQ(ipow(int128_t{1}, 0U), int128_t{1});
    BOOST_TEST_EQ(ipow(int128_t{-1}, 0U), int128_t{1});
    BOOST_TEST_EQ(ipow(int128_t{0}, 5U), int128_t{0});
    BOOST_TEST_EQ(ipow(int128_t{42}, 1U), int128_t{42});

    BOOST_TEST_EQ(ipow(int128_t{2}, 10U), int128_t{1024});
    BOOST_TEST_EQ(ipow(int128_t{3}, 5U), int128_t{243});
    BOOST_TEST_EQ(ipow(int128_t{10}, 18U), int128_t{INT64_C(1000000000000000000)});
}

void test_int128_ipow_negative_base()
{
    // Even exponents are non-negative, odd exponents preserve the sign.
    BOOST_TEST_EQ(ipow(int128_t{-2}, 0U), int128_t{1});
    BOOST_TEST_EQ(ipow(int128_t{-2}, 1U), int128_t{-2});
    BOOST_TEST_EQ(ipow(int128_t{-2}, 2U), int128_t{4});
    BOOST_TEST_EQ(ipow(int128_t{-2}, 3U), int128_t{-8});
    BOOST_TEST_EQ(ipow(int128_t{-2}, 10U), int128_t{1024});
    BOOST_TEST_EQ(ipow(int128_t{-3}, 5U), int128_t{-243});

    BOOST_TEST_EQ(ipow(int128_t{-1}, 100U), int128_t{1});
    BOOST_TEST_EQ(ipow(int128_t{-1}, 101U), int128_t{-1});

    BOOST_TEST_EQ(ipow(int128_t{-10}, 18U), int128_t{INT64_C(1000000000000000000)});
    BOOST_TEST_EQ(ipow(int128_t{-10}, 17U), int128_t{INT64_C(-100000000000000000)});
}

void test_int128_ipow_large()
{
    // 10^38 still fits in int128_t (signed max is roughly 1.7e38).
    const int128_t ten_pow_38 {static_cast<int128_t>(uint128_t{UINT64_C(0x4B3B4CA85A86C47A), UINT64_C(0x098A224000000000)})};
    BOOST_TEST_EQ(ipow(int128_t{10}, 38U), ten_pow_38);
    BOOST_TEST_EQ(ipow(int128_t{-10}, 38U), ten_pow_38);

    // Cross-check small bases against the naive reference.
    for (std::int64_t base {-7}; base < 8; ++base)
    {
        for (std::uint64_t exp {0}; exp < 12; ++exp)
        {
            BOOST_TEST_EQ(ipow(int128_t{base}, exp), ipow_ref(int128_t{base}, exp));
        }
    }
}

void test_int128_ipow_identities()
{
    const int128_t a {12345};
    BOOST_TEST_EQ(ipow(a, 7U), ipow(a, 3U) * ipow(a, 4U));
    BOOST_TEST_EQ(ipow(ipow(int128_t{3}, 4U), 5U), ipow(int128_t{3}, 4U * 5U));

    // Sign behaves multiplicatively.
    BOOST_TEST_EQ(ipow(int128_t{-7}, 3U) * ipow(int128_t{-7}, 4U), ipow(int128_t{-7}, 7U));
}

void test_constexpr_ipow()
{
    constexpr uint128_t r1 {ipow(uint128_t{2}, 10U)};
    static_assert(r1 == uint128_t{1024}, "ipow constexpr uint128 small case");

    constexpr uint128_t r2 {ipow(uint128_t{10}, 18U)};
    static_assert(r2 == uint128_t{UINT64_C(1000000000000000000)}, "ipow constexpr uint128 18 digits");

    constexpr int128_t r3 {ipow(int128_t{-3}, 5U)};
    static_assert(r3 == int128_t{-243}, "ipow constexpr int128 negative base odd exp");

    constexpr int128_t r4 {ipow(int128_t{-3}, 4U)};
    static_assert(r4 == int128_t{81}, "ipow constexpr int128 negative base even exp");

    constexpr uint128_t r5 {ipow(uint128_t{2}, 128U)};
    static_assert(r5 == uint128_t{0}, "ipow constexpr uint128 wrap to zero");
}

int main()
{
    test_uint128_ipow_basic();
    test_uint128_ipow_power_of_two();
    test_uint128_ipow_large();
    test_uint128_ipow_wrap();
    test_uint128_ipow_identities();
    test_int128_ipow_basic();
    test_int128_ipow_negative_base();
    test_int128_ipow_large();
    test_int128_ipow_identities();
    test_constexpr_ipow();

    return boost::report_errors();
}

#endif
