// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

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
    BOOST_TEST_EQ(powm(uint128_t{2}, uint128_t{10}, uint128_t{1000}), uint128_t{24});
    BOOST_TEST_EQ(powm(uint128_t{3}, uint128_t{5}, uint128_t{7}), uint128_t{5});
    BOOST_TEST_EQ(powm(uint128_t{7}, uint128_t{13}, uint128_t{19}), uint128_t{7});
    BOOST_TEST_EQ(powm(uint128_t{5}, uint128_t{10}, uint128_t{13}), uint128_t{12});
    // 10^9 ~ -7 (mod 1000000007), so 10^18 ~ 49.
    BOOST_TEST_EQ(powm(uint128_t{10}, uint128_t{18}, uint128_t{1000000007}), uint128_t{49});

    // exp == 0
    BOOST_TEST_EQ(powm(uint128_t{5}, uint128_t{0}, uint128_t{1000}), uint128_t{1});
    BOOST_TEST_EQ(powm(uint128_t{0}, uint128_t{0}, uint128_t{7}), uint128_t{1});

    // base == 0
    BOOST_TEST_EQ(powm(uint128_t{0}, uint128_t{5}, uint128_t{7}), uint128_t{0});
    BOOST_TEST_EQ(powm(uint128_t{0}, uint128_t{1}, uint128_t{7}), uint128_t{0});

    // base == 1
    BOOST_TEST_EQ(powm(uint128_t{1}, uint128_t{1000}, uint128_t{42}), uint128_t{1});

    // exp == 1
    BOOST_TEST_EQ(powm(uint128_t{42}, uint128_t{1}, uint128_t{100}), uint128_t{42});

    // m == 1
    BOOST_TEST_EQ(powm(uint128_t{42}, uint128_t{17}, uint128_t{1}), uint128_t{0});

    // m == 0 - documented to return 0
    BOOST_TEST_EQ(powm(uint128_t{42}, uint128_t{17}, uint128_t{0}), uint128_t{0});

    // Base larger than the modulus must be reduced first.
    BOOST_TEST_EQ(powm(uint128_t{1234567}, uint128_t{2}, uint128_t{1000}), uint128_t{(1234567ULL * 1234567ULL) % 1000ULL});
}

void test_uint128_powm_power_of_two_modulus()
{
    BOOST_TEST_EQ(powm(uint128_t{3}, uint128_t{10}, uint128_t{128}), uint128_t{41});
    BOOST_TEST_EQ(powm(uint128_t{2}, uint128_t{7}, uint128_t{128}), uint128_t{0});
    BOOST_TEST_EQ(powm(uint128_t{2}, uint128_t{6}, uint128_t{128}), uint128_t{64});

    const uint128_t m32 {static_cast<std::uint64_t>(1) << 32};
    BOOST_TEST_EQ(powm(uint128_t{2}, uint128_t{32}, m32), uint128_t{0});
    BOOST_TEST_EQ(powm(uint128_t{3}, uint128_t{20}, m32), uint128_t{3486784401ULL});

    const uint128_t m64 {1U, 0U};
    BOOST_TEST_EQ(powm(uint128_t{2}, uint128_t{64}, m64), uint128_t{0});
    const uint128_t high_bit {0U, static_cast<std::uint64_t>(1) << 63};
    BOOST_TEST_EQ(powm(uint128_t{2}, uint128_t{63}, m64), high_bit);

    const uint128_t m127 {static_cast<std::uint64_t>(1) << 63, 0U};
    BOOST_TEST_EQ(powm(uint128_t{2}, uint128_t{127}, m127), uint128_t{0});
    BOOST_TEST_EQ(powm(uint128_t{2}, uint128_t{126}, m127), m127 >> 1);
}

void test_uint128_powm_fermat_64bit()
{
    // Mersenne prime p = 2^61 - 1.
    const uint128_t p {UINT64_C(2305843009213693951)};

    for (std::uint64_t a {2}; a < 25; ++a)
    {
        BOOST_TEST_EQ(powm(uint128_t{a}, p - 1U, p), uint128_t{1});
    }

    // Common 32-bit primes that show up in competitive math problems.
    const uint128_t mod_a {UINT64_C(1000000007)};
    const uint128_t mod_b {UINT64_C(998244353)};

    for (std::uint64_t a {2}; a < 10; ++a)
    {
        BOOST_TEST_EQ(powm(uint128_t{a}, mod_a - 1U, mod_a), uint128_t{1});
        BOOST_TEST_EQ(powm(uint128_t{a}, mod_b - 1U, mod_b), uint128_t{1});
    }
}

void test_uint128_powm_fermat_128bit()
{
    // Mersenne prime p = 2^127 - 1 = 170141183460469231731687303715884105727.
    const uint128_t p {UINT64_C(0x7FFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF)};

    for (std::uint64_t a {2}; a < 6; ++a)
    {
        BOOST_TEST_EQ(powm(uint128_t{a}, p - 1U, p), uint128_t{1});
    }

    // a^p mod p == a mod p for prime p (Fermat).
    const uint128_t big_base {UINT64_C(0x0123456789ABCDEF), UINT64_C(0xFEDCBA9876543210)};
    BOOST_TEST_EQ(powm(big_base, p, p), big_base % p);
}

void test_uint128_powm_properties()
{
    // (a^(b+c)) mod m == ((a^b)(a^c)) mod m
    const uint128_t m {UINT64_C(1000000007)};
    const uint128_t a {42};
    const uint128_t b {17};
    const uint128_t c {23};
    BOOST_TEST_EQ(powm(a, b + c, m), (powm(a, b, m) * powm(a, c, m)) % m);

    // (a^e * b^e) mod m == ((a*b)^e) mod m
    const uint128_t aa {7};
    const uint128_t bb {11};
    const uint128_t e {30};
    BOOST_TEST_EQ(powm(aa * bb, e, m), (powm(aa, e, m) * powm(bb, e, m)) % m);

    // Same identities through the full-128-bit code path.
    const uint128_t big_m {1U, UINT64_C(0xDEADBEEFCAFEBABE)};
    const uint128_t big_a {UINT64_C(0x00000000DEADBEEF), UINT64_C(0xBEEFCAFEFACEFEED)};
    const uint128_t b1 {13};
    const uint128_t b2 {21};
    BOOST_TEST_EQ(powm(big_a, b1 + b2, big_m), detail::mulmod_shift(powm(big_a, b1, big_m), powm(big_a, b2, big_m), big_m));
}

void test_uint128_powm_extreme()
{
    // Modulus exactly 2^64 - 1: not a power of two, exercises the small-m path
    // at its upper boundary. Since 2^64 ~ 1 (mod 2^64 - 1), 2^64 mod m == 1.
    const uint128_t m_u64max {(std::numeric_limits<std::uint64_t>::max)()};
    BOOST_TEST_EQ(powm(uint128_t{2}, uint128_t{64}, m_u64max), uint128_t{1});
    BOOST_TEST_EQ(powm(uint128_t{3}, uint128_t{0}, m_u64max), uint128_t{1});

    // 2^65 - 1 forces the general 128-bit path. 2^65 ~ 1 (mod 2^65 - 1).
    const uint128_t m65 {1U, UINT64_C(0xFFFFFFFFFFFFFFFF)};
    BOOST_TEST_EQ(powm(uint128_t{2}, uint128_t{65}, m65), uint128_t{1});
    BOOST_TEST_EQ(powm(uint128_t{2}, uint128_t{130}, m65), uint128_t{1});
    BOOST_TEST_EQ(powm(uint128_t{2}, uint128_t{64}, m65), (uint128_t{1U, 0U}));
    BOOST_TEST_EQ(powm(uint128_t{4}, uint128_t{65}, m65), uint128_t{1});

    // Full 128-bit modulus with high bit set so addmod must take the overflow
    // branch when doubling values near m.
    const uint128_t m_high {UINT64_C(0x8000000000000001), 0U};
    BOOST_TEST_EQ(powm(uint128_t{1}, uint128_t{1234567}, m_high), uint128_t{1});
    BOOST_TEST_EQ(powm(uint128_t{0}, uint128_t{1234567}, m_high), uint128_t{0});

    // Small base whose square still fits in 128 bits gives a hand-verifiable
    // answer while still routing through the shift-and-add path.
    BOOST_TEST_EQ(powm(uint128_t{10}, uint128_t{4}, m_high), uint128_t{10000});
}

void test_int128_powm()
{
    BOOST_TEST_EQ(powm(int128_t{2}, int128_t{10}, int128_t{1000}), int128_t{24});
    BOOST_TEST_EQ(powm(int128_t{3}, int128_t{5}, int128_t{7}), int128_t{5});

    // Negative bases reduce to non-negative residues.
    BOOST_TEST_EQ(powm(int128_t{-3}, int128_t{2}, int128_t{5}), int128_t{4});
    BOOST_TEST_EQ(powm(int128_t{-3}, int128_t{3}, int128_t{5}), int128_t{3});
    BOOST_TEST_EQ(powm(int128_t{-1}, int128_t{100}, int128_t{1000}), int128_t{1});
    BOOST_TEST_EQ(powm(int128_t{-1}, int128_t{101}, int128_t{1000}), int128_t{999});

    // Negative base where the magnitude is a multiple of m reduces to 0.
    BOOST_TEST_EQ(powm(int128_t{-10}, int128_t{5}, int128_t{5}), int128_t{0});

    // INT128_MIN handling: abs() preserves the bit pattern, which maps to 2^127
    // when reinterpreted as uint128_t. 2^127 mod 5 == 3 (since 2^4 ~ 1 (mod 5)
    // gives 2^127 = 2^(4*31+3) ~ 8 ~ 3), so the negative residue is 5 - 3 = 2.
    BOOST_TEST_EQ(powm((std::numeric_limits<int128_t>::min)(), int128_t{1}, int128_t{5}), int128_t{2});
    // 2^127 mod 7 == 2 (since 2^3 ~ 1 (mod 7), 2^127 = 2^(3*42+1) ~ 2), so the
    // residue of -2^127 mod 7 is 7 - 2 = 5.
    BOOST_TEST_EQ(powm((std::numeric_limits<int128_t>::min)(), int128_t{1}, int128_t{7}), int128_t{5});

    // Invalid arguments are documented to return 0.
    BOOST_TEST_EQ(powm(int128_t{2}, int128_t{10}, int128_t{0}), int128_t{0});
    BOOST_TEST_EQ(powm(int128_t{2}, int128_t{10}, int128_t{-5}), int128_t{0});
    BOOST_TEST_EQ(powm(int128_t{2}, int128_t{-1}, int128_t{5}), int128_t{0});
}

void test_constexpr_powm()
{
    constexpr uint128_t r1 {powm(uint128_t{2}, uint128_t{10}, uint128_t{1000})};
    static_assert(r1 == uint128_t{24}, "powm constexpr small case");

    constexpr uint128_t r2 {powm(uint128_t{3}, uint128_t{10}, uint128_t{128})};
    static_assert(r2 == uint128_t{41}, "powm constexpr power-of-two modulus");

    constexpr int128_t r3 {powm(int128_t{-3}, int128_t{3}, int128_t{5})};
    static_assert(r3 == int128_t{3}, "powm constexpr signed");
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
