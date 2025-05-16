// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128/int128.hpp>
#include <boost/int128/bit.hpp>
#include <boost/core/lightweight_test.hpp>

int main()
{
    boost::int128::uint128_t x {1};

    for (unsigned i {1}; i < 128U; ++i)
    {
        BOOST_TEST(boost::int128::has_single_bit(x));
        x *= 2;
    }

    x = 3;
    for (unsigned i {1}; i < 128U; ++i)
    {
        BOOST_TEST(!boost::int128::has_single_bit(x));
        x *= 2;
    }

    return boost::report_errors();
}
