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

void test_uint128_powm_basic()
{
    BOOST_TEST_EQ(powm(uint128{2}, uint128{10}, uint128{1000}), uint128{24});
    BOOST_TEST_EQ(powm(uint128{3}, uint128{5}, uint128{7}), uint128{5});
    BOOST_TEST_EQ(powm(uint128{7}, uint128{13}, uint128{19}), uint128{7});
    BOOST_TEST_EQ(powm(uint128{5}, uint128{10}, uint128{13}), uint128{12});
    // 10^9 ~ -7 (mod 1000000007), so 10^18 ~ 49.
    BOOST_TEST_EQ(powm(uint128{10}, uint128{18}, uint128{1000000007}), uint128{49});

    // exp == 0
    BOOST_TEST_EQ(powm(uint128{5}, uint128{0}, uint128{1000}), uint128{1});
    BOOST_TEST_EQ(powm(uint128{0}, uint128{0}, uint128{7}), uint128{1});

    // base == 0
    BOOST_TEST_EQ(powm(uint128{0}, uint128{5}, uint128{7}), uint128{0});
    BOOST_TEST_EQ(powm(uint128{0}, uint128{1}, uint128{7}), uint128{0});

    // base == 1
    BOOST_TEST_EQ(powm(uint128{1}, uint128{1000}, uint128{42}), uint128{1});

    // exp == 1
    BOOST_TEST_EQ(powm(uint128{42}, uint128{1}, uint128{100}), uint128{42});

    // m == 1
    BOOST_TEST_EQ(powm(uint128{42}, uint128{17}, uint128{1}), uint128{0});

    // m == 0 - documented to return 0
    BOOST_TEST_EQ(powm(uint128{42}, uint128{17}, uint128{0}), uint128{0});

    // Base larger than the modulus must be reduced first.
    BOOST_TEST_EQ(powm(uint128{1234567}, uint128{2}, uint128{1000}), uint128{(1234567ULL * 1234567ULL) % 1000ULL});
}

void test_uint128_powm_power_of_two_modulus()
{
    BOOST_TEST_EQ(powm(uint128{3}, uint128{10}, uint128{128}), uint128{41});
    BOOST_TEST_EQ(powm(uint128{2}, uint128{7}, uint128{128}), uint128{0});
    BOOST_TEST_EQ(powm(uint128{2}, uint128{6}, uint128{128}), uint128{64});

    const uint128 m32 {static_cast<std::uint64_t>(1) << 32};
    BOOST_TEST_EQ(powm(uint128{2}, uint128{32}, m32), uint128{0});
    BOOST_TEST_EQ(powm(uint128{3}, uint128{20}, m32), uint128{3486784401ULL});

    const uint128 m64 {1U, 0U};
    BOOST_TEST_EQ(powm(uint128{2}, uint128{64}, m64), uint128{0});
    const uint128 high_bit {0U, static_cast<std::uint64_t>(1) << 63};
    BOOST_TEST_EQ(powm(uint128{2}, uint128{63}, m64), high_bit);

    const uint128 m127 {static_cast<std::uint64_t>(1) << 63, 0U};
    BOOST_TEST_EQ(powm(uint128{2}, uint128{127}, m127), uint128{0});
    BOOST_TEST_EQ(powm(uint128{2}, uint128{126}, m127), m127 >> 1);
}

void test_uint128_powm_fermat_64bit()
{
    // Mersenne prime p = 2^61 - 1.
    const uint128 p {UINT64_C(2305843009213693951)};

    for (std::uint64_t a {2}; a < 25; ++a)
    {
        BOOST_TEST_EQ(powm(uint128{a}, p - 1U, p), uint128{1});
    }

    // Common 32-bit primes that show up in competitive math problems.
    const uint128 mod_a {UINT64_C(1000000007)};
    const uint128 mod_b {UINT64_C(998244353)};

    for (std::uint64_t a {2}; a < 10; ++a)
    {
        BOOST_TEST_EQ(powm(uint128{a}, mod_a - 1U, mod_a), uint128{1});
        BOOST_TEST_EQ(powm(uint128{a}, mod_b - 1U, mod_b), uint128{1});
    }
}

void test_uint128_powm_fermat_128bit()
{
    // Mersenne prime p = 2^127 - 1 = 170141183460469231731687303715884105727.
    const uint128 p {UINT64_C(0x7FFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF)};

    for (std::uint64_t a {2}; a < 6; ++a)
    {
        BOOST_TEST_EQ(powm(uint128{a}, p - 1U, p), uint128{1});
    }

    // a^p mod p == a mod p for prime p (Fermat).
    const uint128 big_base {UINT64_C(0x0123456789ABCDEF), UINT64_C(0xFEDCBA9876543210)};
    BOOST_TEST_EQ(powm(big_base, p, p), big_base % p);
}

void test_uint128_powm_properties()
{
    // (a^(b+c)) mod m == ((a^b)(a^c)) mod m
    const uint128 m {UINT64_C(1000000007)};
    const uint128 a {42};
    const uint128 b {17};
    const uint128 c {23};
    BOOST_TEST_EQ(powm(a, b + c, m), (powm(a, b, m) * powm(a, c, m)) % m);

    // (a^e * b^e) mod m == ((a*b)^e) mod m
    const uint128 aa {7};
    const uint128 bb {11};
    const uint128 e {30};
    BOOST_TEST_EQ(powm(aa * bb, e, m), (powm(aa, e, m) * powm(bb, e, m)) % m);

    // Same identities through the full-128-bit code path.
    const uint128 big_m {1U, UINT64_C(0xDEADBEEFCAFEBABE)};
    const uint128 big_a {UINT64_C(0x00000000DEADBEEF), UINT64_C(0xBEEFCAFEFACEFEED)};
    const uint128 b1 {13};
    const uint128 b2 {21};
    BOOST_TEST_EQ(powm(big_a, b1 + b2, big_m), detail::mulmod_shift(powm(big_a, b1, big_m), powm(big_a, b2, big_m), big_m));
}

void test_uint128_powm_extreme()
{
    // Modulus exactly 2^64 - 1: not a power of two, exercises the small-m path
    // at its upper boundary. Since 2^64 ~ 1 (mod 2^64 - 1), 2^64 mod m == 1.
    const uint128 m_u64max {(std::numeric_limits<std::uint64_t>::max)()};
    BOOST_TEST_EQ(powm(uint128{2}, uint128{64}, m_u64max), uint128{1});
    BOOST_TEST_EQ(powm(uint128{3}, uint128{0}, m_u64max), uint128{1});

    // 2^65 - 1 forces the general 128-bit path. 2^65 ~ 1 (mod 2^65 - 1).
    const uint128 m65 {1U, UINT64_C(0xFFFFFFFFFFFFFFFF)};
    BOOST_TEST_EQ(powm(uint128{2}, uint128{65}, m65), uint128{1});
    BOOST_TEST_EQ(powm(uint128{2}, uint128{130}, m65), uint128{1});
    BOOST_TEST_EQ(powm(uint128{2}, uint128{64}, m65), (uint128{1U, 0U}));
    BOOST_TEST_EQ(powm(uint128{4}, uint128{65}, m65), uint128{1});

    // Full 128-bit modulus with high bit set so addmod must take the overflow
    // branch when doubling values near m.
    const uint128 m_high {UINT64_C(0x8000000000000001), 0U};
    BOOST_TEST_EQ(powm(uint128{1}, uint128{1234567}, m_high), uint128{1});
    BOOST_TEST_EQ(powm(uint128{0}, uint128{1234567}, m_high), uint128{0});

    // Small base whose square still fits in 128 bits gives a hand-verifiable
    // answer while still routing through the shift-and-add path.
    BOOST_TEST_EQ(powm(uint128{10}, uint128{4}, m_high), uint128{10000});
}

void test_int128_powm()
{
    BOOST_TEST_EQ(powm(int128{2}, int128{10}, int128{1000}), int128{24});
    BOOST_TEST_EQ(powm(int128{3}, int128{5}, int128{7}), int128{5});

    // Negative bases reduce to non-negative residues.
    BOOST_TEST_EQ(powm(int128{-3}, int128{2}, int128{5}), int128{4});
    BOOST_TEST_EQ(powm(int128{-3}, int128{3}, int128{5}), int128{3});
    BOOST_TEST_EQ(powm(int128{-1}, int128{100}, int128{1000}), int128{1});
    BOOST_TEST_EQ(powm(int128{-1}, int128{101}, int128{1000}), int128{999});

    // Negative base where the magnitude is a multiple of m reduces to 0.
    BOOST_TEST_EQ(powm(int128{-10}, int128{5}, int128{5}), int128{0});

    // INT128_MIN handling: abs() preserves the bit pattern, which maps to 2^127
    // when reinterpreted as uint128. 2^127 mod 5 == 3 (since 2^4 ~ 1 (mod 5)
    // gives 2^127 = 2^(4*31+3) ~ 8 ~ 3), so the negative residue is 5 - 3 = 2.
    BOOST_TEST_EQ(powm((std::numeric_limits<int128>::min)(), int128{1}, int128{5}), int128{2});
    // 2^127 mod 7 == 2 (since 2^3 ~ 1 (mod 7), 2^127 = 2^(3*42+1) ~ 2), so the
    // residue of -2^127 mod 7 is 7 - 2 = 5.
    BOOST_TEST_EQ(powm((std::numeric_limits<int128>::min)(), int128{1}, int128{7}), int128{5});

    // Invalid arguments are documented to return 0.
    BOOST_TEST_EQ(powm(int128{2}, int128{10}, int128{0}), int128{0});
    BOOST_TEST_EQ(powm(int128{2}, int128{10}, int128{-5}), int128{0});
    BOOST_TEST_EQ(powm(int128{2}, int128{-1}, int128{5}), int128{0});
}

// Only present with MSVC 14.1
#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable : 4307) // integral constant overflow
#  pragma warning(disable : 4146) // unary minus operator applied to unsigned type, result still unsigned
#endif

void test_constexpr_powm()
{
    constexpr uint128 r1 {powm(uint128{2}, uint128{10}, uint128{1000})};
    static_assert(r1 == uint128{24}, "powm constexpr small case");

    constexpr uint128 r2 {powm(uint128{3}, uint128{10}, uint128{128})};
    static_assert(r2 == uint128{41}, "powm constexpr power-of-two modulus");

    constexpr int128 r3 {powm(int128{-3}, int128{3}, int128{5})};
    static_assert(r3 == int128{3}, "powm constexpr signed");
}

int main()
{
    test_uint128_powm_basic();
    test_uint128_powm_power_of_two_modulus();
    test_uint128_powm_fermat_64bit();
    test_uint128_powm_fermat_128bit();
    test_uint128_powm_properties();
    test_uint128_powm_extreme();
    test_int128_powm();
    test_constexpr_powm();

    return boost::report_errors();
}

#endif
