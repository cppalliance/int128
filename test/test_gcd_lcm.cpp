// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128.hpp>
#include <boost/core/lightweight_test.hpp>
#include <limits>

using namespace boost::int128;

void test_gcd()
{
    // Basic tests
    BOOST_TEST_EQ(gcd(uint128_t(12), uint128_t(8)), uint128_t(4));
    BOOST_TEST_EQ(gcd(uint128_t(54), uint128_t(24)), uint128_t(6));
    BOOST_TEST_EQ(gcd(uint128_t(48), uint128_t(18)), uint128_t(6));

    // Edge cases with zero
    BOOST_TEST_EQ(gcd(uint128_t(0), uint128_t(5)), uint128_t(5));
    BOOST_TEST_EQ(gcd(uint128_t(5), uint128_t(0)), uint128_t(5));
    BOOST_TEST_EQ(gcd(uint128_t(0), uint128_t(0)), uint128_t(0));

    // Same numbers
    BOOST_TEST_EQ(gcd(uint128_t(17), uint128_t(17)), uint128_t(17));
    BOOST_TEST_EQ(gcd(uint128_t(100), uint128_t(100)), uint128_t(100));

    // Coprime numbers (GCD = 1)
    BOOST_TEST_EQ(gcd(uint128_t(13), uint128_t(17)), uint128_t(1));
    BOOST_TEST_EQ(gcd(uint128_t(35), uint128_t(64)), uint128_t(1));

    // Powers of 2
    BOOST_TEST_EQ(gcd(uint128_t(16), uint128_t(32)), uint128_t(16));
    BOOST_TEST_EQ(gcd(uint128_t(64), uint128_t(128)), uint128_t(64));
    BOOST_TEST_EQ(gcd(uint128_t(1024), uint128_t(512)), uint128_t(512));

    // One divides the other
    BOOST_TEST_EQ(gcd(uint128_t(10), uint128_t(100)), uint128_t(10));
    BOOST_TEST_EQ(gcd(uint128_t(7), uint128_t(49)), uint128_t(7));

    // Large 64-bit values
    BOOST_TEST_EQ(gcd(uint128_t(1000000007), uint128_t(1000000009)), uint128_t(1));
    BOOST_TEST_EQ(gcd(uint128_t(UINT64_MAX), uint128_t(UINT64_MAX)), uint128_t(UINT64_MAX));

    // Large 128-bit values
    constexpr uint128_t large1 {0x123456789ABCDEF0ULL, 0xFEDCBA9876543210ULL};
    constexpr uint128_t large2 {0x0F0F0F0F0F0F0F0FULL, 0xF0F0F0F0F0F0F0F0ULL};
    BOOST_TEST(gcd(large1, large2) > uint128_t(0)); // Just verify it doesn't crash

    // Mixed small and large
    BOOST_TEST_EQ(gcd(uint128_t(1, 0), uint128_t(100)), uint128_t(4));

    // Fibonacci numbers (interesting GCD patterns)
    BOOST_TEST_EQ(gcd(uint128_t(89), uint128_t(144)), uint128_t(1));
    BOOST_TEST_EQ(gcd(uint128_t(34), uint128_t(55)), uint128_t(1));
}

void test_lcm()
{
    // Basic tests
    BOOST_TEST_EQ(lcm(uint128_t(4), uint128_t(6)), uint128_t(12));
    BOOST_TEST_EQ(lcm(uint128_t(3), uint128_t(5)), uint128_t(15));
    BOOST_TEST_EQ(lcm(uint128_t(12), uint128_t(18)), uint128_t(36));

    // Edge cases with zero
    BOOST_TEST_EQ(lcm(uint128_t(0), uint128_t(5)), uint128_t(0));
    BOOST_TEST_EQ(lcm(uint128_t(5), uint128_t(0)), uint128_t(0));
    BOOST_TEST_EQ(lcm(uint128_t(0), uint128_t(0)), uint128_t(0));

    // Same numbers
    BOOST_TEST_EQ(lcm(uint128_t(7), uint128_t(7)), uint128_t(7));
    BOOST_TEST_EQ(lcm(uint128_t(100), uint128_t(100)), uint128_t(100));

    // Coprime numbers (LCM = a*b)
    BOOST_TEST_EQ(lcm(uint128_t(7), uint128_t(11)), uint128_t(77));
    BOOST_TEST_EQ(lcm(uint128_t(13), uint128_t(17)), uint128_t(221));

    // Powers of 2
    BOOST_TEST_EQ(lcm(uint128_t(8), uint128_t(16)), uint128_t(16));
    BOOST_TEST_EQ(lcm(uint128_t(32), uint128_t(64)), uint128_t(64));

    // One divides the other
    BOOST_TEST_EQ(lcm(uint128_t(3), uint128_t(12)), uint128_t(12));
    BOOST_TEST_EQ(lcm(uint128_t(5), uint128_t(25)), uint128_t(25));

    // Verify LCM * GCD = a * b property
    uint128_t a = uint128_t(42);
    uint128_t b = uint128_t(56);
    BOOST_TEST_EQ(lcm(a, b) * gcd(a, b), a * b);

    // Large values that won't overflow
    BOOST_TEST_EQ(lcm(uint128_t(1000000007), uint128_t(1000000009)),
                      uint128_t(1000000007) * uint128_t(1000000009));

    // Test with larger values (but still safe from overflow)
    uint128_t big1 = uint128_t(1) << 60;  // 2^60
    uint128_t big2 = uint128_t(1) << 61;  // 2^61
    BOOST_TEST_EQ(lcm(big1, big2), big2);  // LCM of powers of 2
}

void test_gcd_lcm_properties()
{
    // Test various mathematical properties

    // Property: gcd(a,b) * lcm(a,b) = a * b
    constexpr uint128_t pairs[][2] = {
        {12, 18},
        {100, 150},
        {77, 49},
        {1024, 768}
    };

    for (const auto& pair : pairs)
    {
        uint128_t a = pair[0];
        uint128_t b = pair[1];
        BOOST_TEST_EQ(gcd(a, b) * lcm(a, b), a * b);
    }

    // Property: gcd(a,b) = gcd(b,a) (commutative)
    BOOST_TEST_EQ(gcd(uint128_t(48), uint128_t(18)), gcd(uint128_t(18), uint128_t(48)));

    // Property: lcm(a,b) = lcm(b,a) (commutative)
    BOOST_TEST_EQ(lcm(uint128_t(12), uint128_t(8)), lcm(uint128_t(8), uint128_t(12)));

    // Property: gcd(a, gcd(b, c)) = gcd(gcd(a, b), c) (associative)
    uint128_t x = 60U, y = 48U, z = 36U;
    BOOST_TEST_EQ(gcd(x, gcd(y, z)), gcd(gcd(x, y), z));
}

int main()
{
    test_gcd();
    test_lcm();
    test_gcd_lcm_properties();

    return boost::report_errors();
}
