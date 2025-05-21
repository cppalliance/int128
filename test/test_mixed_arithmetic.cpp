// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128.hpp>
#include <boost/core/lightweight_test.hpp>

int main()
{
    constexpr boost::int128::uint128_t lhs {3};
    constexpr boost::int128::int128_t rhs {-3};

    BOOST_TEST(lhs + rhs == 0);
    BOOST_TEST(lhs - rhs == -6);
    BOOST_TEST(lhs * rhs == -9);
    BOOST_TEST(lhs / rhs == -1);

    return boost::report_errors();
}
