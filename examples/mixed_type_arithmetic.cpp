// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// #define BOOST_INT128_ALLOW_SIGN_CONVERSION
#include <boost/int128.hpp>

int main()
{
    // By default, mixed type arithmetic is NOT ALLOWED
    // In order for this file to compile #define BOOST_INT128_ALLOW_SIGN_CONVERSION
    // BEFORE the inclusion of any file of this library (uncomment the top line)
    //
    // Unlike builtin types we cannot enforce sign correctness via compiler flag,
    // so we made it the default.


    constexpr boost::int128::uint128_t unsigned_value {3};

    constexpr auto greater_unsigned_value {unsigned_value + 5};

    assert(unsigned_value + 1 == 4);
    assert(unsigned_value - 1 == 2);
    assert(unsigned_value * 2 == 6);
    assert(unsigned_value / 3 == 1);
    assert(unsigned_value % 3 == 0);
    assert(unsigned_value + 5 == greater_unsigned_value);

    constexpr boost::int128::int128_t signed_value {-3};

    assert(signed_value + 1U == -2);
    assert(signed_value - 4U == -7);
    assert(signed_value * 2 == -6);
    assert(signed_value / 4U == 0);

    return 0;
}
