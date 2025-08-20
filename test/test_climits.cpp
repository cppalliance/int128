// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128.hpp>
#include <limits>

int main()
{
    static_assert(BOOST_INT128_UINT128_MAX == std::numeric_limits<boost::int128::uint128_t>::max(), "Should be equal");
    static_assert(BOOST_INT128_INT128_MAX == std::numeric_limits<boost::int128::int128_t>::max(), "Should be equal");
    static_assert(BOOST_INT128_INT128_MIN == std::numeric_limits<boost::int128::int128_t>::min(), "Should be equal");

    return 0;
}
