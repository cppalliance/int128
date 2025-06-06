// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128/int128.hpp>
#include <boost/int128/iostream.hpp>
#include <iostream>
#include <iomanip>

int main()
{
    // Both types allow streaming as one would expect from a regular builtin-type
    constexpr boost::int128::int128_t signed_value {-42};
    std::cout << "Signed value: " << signed_value << std::endl;

    // We can also use <iomanip> to change the output format
    constexpr boost::int128::uint128_t unsigned_value {0x1, UINT64_MAX};
    std::cout << "Unsigned value (dec): " << unsigned_value << '\n'
              << "Unsigned value (hex): " << std::hex << unsigned_value << '\n'
              << "Unsigned value (oct): " << std::oct << unsigned_value << std::endl;

    // Hex also can be manipulated to be uppercase
    std::cout << "Upper unsigned value: " << std::hex << std::uppercase << unsigned_value << std::endl;

    return 0;
}
