// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#define BOOST_INT128_ALLOW_SIGN_COMPARE

// Only sign compare means that we should still be failing to compile on operations

#include <boost/int128/int128.hpp>
#include <boost/core/lightweight_test.hpp>

void test_u128()
{
    boost::int128::uint128_t x {5U};
    BOOST_TEST(x > 4);
    BOOST_TEST(x >= 4);
    BOOST_TEST(x == 5);
    BOOST_TEST(x != 0);
    BOOST_TEST(x <= 5);
    BOOST_TEST(x < 10);

    x *= 2;
    BOOST_TEST(x == 10);
    x += 2;
    BOOST_TEST(x == 12);
    x -= 2;
    BOOST_TEST(x == 10);
    x /= 2;
    BOOST_TEST(x == 5);
}

void test_int128()
{
    boost::int128::int128_t x {5};
    BOOST_TEST(x > 4U);
    BOOST_TEST(x >= 4U);
    BOOST_TEST(x == 5U);
    BOOST_TEST(x != 3U);
    BOOST_TEST(x <= 5U);
    BOOST_TEST(x < 10U);

    x *= 2U;
    BOOST_TEST(x == 10U);
    x += 2U;
    BOOST_TEST(x == 12U);
    x -= 2U;
    BOOST_TEST(x == 10U);
    x /= 2U;
    BOOST_TEST(x == 5U);
}

int main()
{
    test_u128();
    test_int128();

    return boost::report_errors();
}
