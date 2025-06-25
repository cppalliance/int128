// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// Allowing sign conversion is a required pre-requisite for Boost.Random
#define BOOST_INT128_ALLOW_SIGN_CONVERSION

#include <boost/int128.hpp>
#include <boost/charconv.hpp>
#include <iostream>
#include <limits>
#include <cstring>

int main()
{
    constexpr boost::int128::uint128_t max_value = std::numeric_limits<boost::int128::uint128_t>::max();

    // Boost.Charconv can be used as it would be with any builtin type
    char buffer[64];
    auto r = boost::charconv::to_chars(buffer, buffer + sizeof(buffer), max_value);
    assert(r);
    *r.ptr = '\0'; // Boost/std charconv do not add null terminators

    std::cout << "Max unsigned value: " << buffer << std::endl;

    //boost::int128::uint128_t return_value;
    //boost::charconv::from_chars(buffer, buffer + sizeof(buffer), return_value);

    //return max_value == return_value ? 0 : 1;

    // And same for signed types
    constexpr auto min_signed_value = std::numeric_limits<boost::int128::int128_t>::min();

    std::memset(buffer, 0, sizeof(buffer));
    r = boost::charconv::to_chars(buffer, buffer + sizeof(buffer), min_signed_value);
    assert(r);
    *r.ptr = '\0';

    std::cout << "Min signed value: " << buffer << std::endl;

    return 0;
}
