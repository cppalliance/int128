// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
//
// Allowing sign conversion is a required pre-requisite for Boost.Charconv
#define BOOST_INT128_ALLOW_SIGN_CONVERSION

#include <boost/int128.hpp>
#include <boost/charconv.hpp>
#include <iostream>
#include <limits>
#include <cstring>
#include <string>

#ifdef __clang__
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wunused-variable"
#endif


int main()
{
    constexpr boost::int128::uint128_t max_value = std::numeric_limits<boost::int128::uint128_t>::max();

    // Boost.Charconv can be used as it would be with any builtin type
    char buffer[64];
    auto r = boost::charconv::to_chars(buffer, buffer + sizeof(buffer), max_value);
    assert(r);
    *r.ptr = '\0'; // Boost/std charconv do not add null terminators

    std::cout << "Max unsigned value: " << buffer << std::endl;

    boost::int128::uint128_t return_value;
    auto r_from = boost::charconv::from_chars(buffer, buffer + sizeof(buffer), return_value);
    assert(r_from);
    assert(max_value == return_value);

    // And same for signed types
    constexpr auto min_signed_value = std::numeric_limits<boost::int128::int128_t>::min();

    std::memset(buffer, 0, sizeof(buffer));
    r = boost::charconv::to_chars(buffer, buffer + sizeof(buffer), min_signed_value);
    assert(r);
    *r.ptr = '\0';

    std::cout << "Min signed value: " << buffer << std::endl;

    boost::int128::int128_t signed_return_value;
    r_from = boost::charconv::from_chars(buffer, buffer + sizeof(buffer), signed_return_value);
    assert(r_from);
    assert(min_signed_value == signed_return_value);

    // Boost from_chars also supports from_chars for a std::string or std::string_view

    std::string unsigned_string = "42";
    r_from = boost::charconv::from_chars(unsigned_string, return_value);
    assert(r_from);
    assert(return_value == boost::int128::uint128_t{42});

    std::string signed_string = "-12345";
    r_from = boost::charconv::from_chars(signed_string, signed_return_value);
    assert(r_from);
    assert(signed_return_value == boost::int128::int128_t{-12345});

    return 0;
}
