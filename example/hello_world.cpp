//  Copyright Matt Borland 2026.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/int128.hpp>
#include <iostream>

int main()
{
    // 2^64 is one past the largest value a 64-bit integer can hold
    const boost::int128::uint128 x {boost::int128::uint128{1} << 64U};
    const boost::int128::uint128 y {42};

    std::cout << x << " + " << y << " = " << x + y << std::endl;

    return 0;
}
