// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128.hpp>
#include <boost/core/lightweight_test.hpp>
#include <limits>

using namespace boost::int128;

template <typename T>
void test_gcd()
{
    // Basic tests
    BOOST_TEST_EQ(gcd(T(12), T(8)), T(4));
    BOOST_TEST_EQ(gcd(T(54), T(24)), T(6));
    BOOST_TEST_EQ(gcd(T(48), T(18)), T(6));

    // Edge cases with zero
    BOOST_TEST_EQ(gcd(T(0), T(5)), T(5));
    BOOST_TEST_EQ(gcd(T(5), T(0)), T(5));
    BOOST_TEST_EQ(gcd(T(0), T(0)), T(0));

    // Same numbers
    BOOST_TEST_EQ(gcd(T(17), T(17)), T(17));
    BOOST_TEST_EQ(gcd(T(100), T(100)), T(100));

    // Coprime numbers (GCD = 1)
    BOOST_TEST_EQ(gcd(T(13), T(17)), T(1));
    BOOST_TEST_EQ(gcd(T(35), T(64)), T(1));

    // Powers of 2
    BOOST_TEST_EQ(gcd(T(16), T(32)), T(16));
    BOOST_TEST_EQ(gcd(T(64), T(128)), T(64));
    BOOST_TEST_EQ(gcd(T(1024), T(512)), T(512));

    // One divides the other
    BOOST_TEST_EQ(gcd(T(10), T(100)), T(10));
    BOOST_TEST_EQ(gcd(T(7), T(49)), T(7));

    // Large 64-bit values
    BOOST_TEST_EQ(gcd(T(1000000007), T(1000000009)), T(1));
    BOOST_TEST_EQ(gcd(T(UINT64_MAX), T(UINT64_MAX)), T(UINT64_MAX));

    // Large 128-bit values
    constexpr T large1 {0x123456789ABCDEF0ULL, 0xFEDCBA9876543210ULL};
    constexpr T large2 {0x0F0F0F0F0F0F0F0FULL, 0xF0F0F0F0F0F0F0F0ULL};
    BOOST_TEST(gcd(large1, large2) > T(0)); // Just verify it doesn't crash

    // Mixed small and large
    BOOST_TEST_EQ(gcd(T(1, 0), T(100)), T(4));

    // Fibonacci numbers (interesting GCD patterns)
    BOOST_TEST_EQ(gcd(T(89), T(144)), T(1));
    BOOST_TEST_EQ(gcd(T(34), T(55)), T(1));
}

template <typename T>
void test_lcm()
{
    // Basic tests
    BOOST_TEST_EQ(lcm(T(4), T(6)), T(12));
    BOOST_TEST_EQ(lcm(T(3), T(5)), T(15));
    BOOST_TEST_EQ(lcm(T(12), T(18)), T(36));

    // Edge cases with zero
    BOOST_TEST_EQ(lcm(T(0), T(5)), T(0));
    BOOST_TEST_EQ(lcm(T(5), T(0)), T(0));
    BOOST_TEST_EQ(lcm(T(0), T(0)), T(0));

    // Same numbers
    BOOST_TEST_EQ(lcm(T(7), T(7)), T(7));
    BOOST_TEST_EQ(lcm(T(100), T(100)), T(100));

    // Coprime numbers (LCM = a*b)
    BOOST_TEST_EQ(lcm(T(7), T(11)), T(77));
    BOOST_TEST_EQ(lcm(T(13), T(17)), T(221));

    // Powers of 2
    BOOST_TEST_EQ(lcm(T(8), T(16)), T(16));
    BOOST_TEST_EQ(lcm(T(32), T(64)), T(64));

    // One divides the other
    BOOST_TEST_EQ(lcm(T(3), T(12)), T(12));
    BOOST_TEST_EQ(lcm(T(5), T(25)), T(25));

    // Verify LCM * GCD = a * b property
    T a = T(42);
    T b = T(56);
    BOOST_TEST_EQ(lcm(a, b) * gcd(a, b), a * b);

    // Large values that won't overflow
    BOOST_TEST_EQ(lcm(T(1000000007), T(1000000009)),
                      T(1000000007) * T(1000000009));

    // Test with larger values (but still safe from overflow)
    T big1 = T(1) << 60;  // 2^60
    T big2 = T(1) << 61;  // 2^61
    BOOST_TEST_EQ(lcm(big1, big2), big2);  // LCM of powers of 2
}

template <typename T>
void test_gcd_lcm_properties()
{
    // Test various mathematical properties

    // Property: gcd(a,b) * lcm(a,b) = a * b
    constexpr T pairs[][2] = {
        {12, 18},
        {100, 150},
        {77, 49},
        {1024, 768}
    };

    for (const auto& pair : pairs)
    {
        T a = pair[0];
        T b = pair[1];
        BOOST_TEST_EQ(gcd(a, b) * lcm(a, b), a * b);
    }

    // Property: gcd(a,b) = gcd(b,a) (commutative)
    BOOST_TEST_EQ(gcd(T(48), T(18)), gcd(T(18), T(48)));

    // Property: lcm(a,b) = lcm(b,a) (commutative)
    BOOST_TEST_EQ(lcm(T(12), T(8)), lcm(T(8), T(12)));

    // Property: gcd(a, gcd(b, c)) = gcd(gcd(a, b), c) (associative)
    T x = 60U, y = 48U, z = 36U;
    BOOST_TEST_EQ(gcd(x, gcd(y, z)), gcd(gcd(x, y), z));
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4146)
#endif

void test_negative_value()
{
    // These should always return positive values
    // Also exercises our constexpr-ness

    constexpr int128_t p {2 * 2 * 3};
    constexpr int128_t q {2 * 3 * 3};
    static_assert(2 * 3 == gcd(p, q), "Wrong");

    static_assert(gcd(int128_t{6},  int128_t{10}) == 2, "Wrong");
    static_assert(gcd(int128_t{6}, int128_t{-10}) == 2, "Wrong");
    static_assert(gcd(int128_t{-6}, int128_t{-10}) == 2, "Wrong");

    static_assert(gcd(int128_t{24}, int128_t{0}) == 24, "Wrong");
    static_assert(gcd(int128_t{-24}, int128_t{0}) == 24, "Wrong");

    static_assert(gcd(int128_t{0}, int128_t{24}) == 24, "Wrong");
    static_assert(gcd(int128_t{0}, int128_t{-24}) == 24, "Wrong");
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

int main()
{
    test_gcd<uint128_t>();
    test_lcm<uint128_t>();
    test_gcd_lcm_properties<uint128_t>();

    test_gcd<int128_t>();
    test_lcm<int128_t>();
    test_gcd_lcm_properties<int128_t>();
    test_negative_value();

    return boost::report_errors();
}
