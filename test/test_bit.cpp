// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128/int128.hpp>
#include <boost/int128/bit.hpp>
#include <boost/core/lightweight_test.hpp>

void test_has_single_bit()
{
    boost::int128::uint128_t x {1};

    for (unsigned i {1}; i < 128U; ++i)
    {
        BOOST_TEST(boost::int128::has_single_bit(x));
        x <<= 1;
    }

    x = 3;
    for (unsigned i {1}; i < 128U; ++i)
    {
        BOOST_TEST(!boost::int128::has_single_bit(x));
        x <<= 1;
    }
}

void test_countl_zero()
{
    BOOST_TEST_EQ(boost::int128::countl_zero(0), 128);

    boost::int128::uint128_t x {1};

    for (unsigned i {1}; i < 128U; ++i)
    {
        BOOST_TEST_EQ(boost::int128::countl_zero(x), 128 - i);
        x <<= 1;
    }
}

void test_bit_width()
{
    BOOST_TEST_EQ(boost::int128::bit_width(0), 0);

    boost::int128::uint128_t x {1};

    for (unsigned i {1}; i < 128U; ++i)
    {
        BOOST_TEST_EQ(boost::int128::bit_width(x), i);
        x <<= 1;
    }

    x = 3;
    for (unsigned i {2}; i < 128U; ++i)
    {
        BOOST_TEST_EQ(boost::int128::bit_width(x), i);
        x <<= 1;
    }
}

void test_bit_ceil()
{
    BOOST_TEST_EQ(boost::int128::bit_ceil(0), 1);
    BOOST_TEST_EQ(boost::int128::bit_ceil(1), 1);
    BOOST_TEST_EQ(boost::int128::bit_ceil(2), 2);

    boost::int128::uint128_t x {3};
    boost::int128::uint128_t y {4};
    for (unsigned i {4}; i < 128U; ++i)
    {
        BOOST_TEST_EQ(boost::int128::bit_ceil(x), y);

        x <<= 1;
        y <<= 1;
    }
}

void test_bit_floor()
{
    BOOST_TEST_EQ(boost::int128::bit_floor(0), 0);

    boost::int128::uint128_t x {3};
    boost::int128::uint128_t y {2};

    for (unsigned i {2}; i < 127U; ++i)
    {
        BOOST_TEST_EQ(boost::int128::bit_floor(x), y);

        x <<= 1;
        y <<= 1;
    }
}

void test_countl_one()
{
    BOOST_TEST_EQ(boost::int128::countl_one(0), 0);
    boost::int128::uint128_t x {UINT64_MAX, UINT64_MAX};

    for (int i {128}; i >= 0; --i)
    {
        BOOST_TEST_EQ(boost::int128::countl_one(x), i);
        x <<= 1;
    }
}

void test_countr_zero()
{
    BOOST_TEST_EQ(boost::int128::countr_zero(0), 128);

    boost::int128::uint128_t x {0x8000000000000000ULL, 0};

    for (int i {127}; i >= 0; --i)
    {
        BOOST_TEST_EQ(boost::int128::countr_zero(x), i);
        x >>= 1;
    }
}

void test_countr_one()
{
    BOOST_TEST_EQ(boost::int128::countr_one(0), 0);

    boost::int128::uint128_t x {UINT64_MAX, UINT64_MAX};

    for (int i {128}; i >= 0; --i)
    {
        BOOST_TEST_EQ(boost::int128::countr_one(x), i);
        x >>= 1;
    }
}

void test_rotl()
{
    constexpr boost::int128::uint128_t x {1};
    boost::int128::uint128_t y {1};

    for (int i {0}; i < 128; ++i)
    {
        BOOST_TEST(boost::int128::rotl(x, i) == y);
        y <<= 1;
    }
}

void test_rotr()
{
    constexpr boost::int128::uint128_t x {0x8000000000000000ULL, 0};
    boost::int128::uint128_t y {0x8000000000000000ULL, 0};

    for (int i {0}; i < 128; ++i)
    {
        BOOST_TEST(boost::int128::rotr(x, i) == y);
        y >>= 1;
    }
}

void test_popcount()
{
    BOOST_TEST_EQ(boost::int128::popcount(0), 0);
    boost::int128::uint128_t x {0, 2};

    for (int i {1}; i < 128; ++i)
    {
        BOOST_TEST_EQ(boost::int128::popcount(x - 1), i);
        x <<= 1;
    }
}

void test_byteswap()
{
    // Test 1: Basic test with specific byte values
    {
        // Create a value with distinct byte pattern
        boost::int128::uint128_t original{
            0x0123456789ABCDEFULL,
            0xFEDCBA9876543210ULL
        };

        // Expected result after byteswap
        boost::int128::uint128_t expected{
            0x1032547698BADCFEULL,
            0xEFCDAB8967452301ULL
        };

        BOOST_TEST(boost::int128::byteswap(original) == expected);
        BOOST_TEST(boost::int128::byteswap(expected) == original);
    }

    // Test 2: Verify double byteswap returns original
    {
        boost::int128::uint128_t values[] = {
            {0, 0},                                               // All zeros
            {~0ULL, ~0ULL},                                             // All ones
            {0x0123456789ABCDEFULL, 0xFEDCBA9876543210ULL},       // Mixed pattern
            {1ULL, 0},                                            // Single bit in high
            {0, 1ULL}                                             // Single bit in low
        };

        for (const auto& val : values) {
            BOOST_TEST(boost::int128::byteswap(boost::int128::byteswap(val)) == val);
        }
    }

    // Test 3: Verify each byte position is correctly swapped
    for (int i = 0; i < 16; ++i)
    {
        // Set a single byte to 0xFF
        boost::int128::uint128_t input{0, 0};
        auto bytes = reinterpret_cast<uint8_t*>(&input);
        bytes[i] = 0xFF;

        // After byteswap, the 0xFF should be at position (15-i)
        boost::int128::uint128_t result = boost::int128::byteswap(input);
        auto result_bytes = reinterpret_cast<uint8_t*>(&result);

        for (int j = 0; j < 16; ++j)
        {
            if (j == 15 - i)
            {
                BOOST_TEST(result_bytes[j] == 0xFF);
            }
            else
            {
                BOOST_TEST(result_bytes[j] == 0);
            }
        }
    }
}

int main()
{
    test_has_single_bit();
    test_countl_zero();
    test_bit_width();
    test_bit_ceil();
    test_bit_floor();
    test_countl_one();
    test_countl_zero();
    test_countr_one();
    test_rotl();
    test_rotr();
    test_popcount();
    test_byteswap();

    return boost::report_errors();
}
