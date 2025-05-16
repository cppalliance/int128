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

int main()
{
    test_has_single_bit();
    test_countl_zero();
    test_bit_width();
    test_bit_ceil();
    test_bit_floor();

    return boost::report_errors();
}
