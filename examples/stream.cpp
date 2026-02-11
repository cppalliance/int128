// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128/int128.hpp>
#include <boost/int128/iostream.hpp>
#include <iostream>
#include <sstream>

int main()
{
    using boost::int128::int128_t;
    using boost::int128::uint128_t;

    std::cout << "=== Basic Streaming ===" << std::endl;

    // Both types allow streaming as one would expect from a regular builtin-type
    constexpr int128_t signed_value {-42};
    std::cout << "Signed value: " << signed_value << std::endl;

    // We can also use <iomanip> to change the output format
    constexpr uint128_t unsigned_value {0x1, UINT64_MAX};
    std::cout << "Unsigned value (dec): " << unsigned_value << '\n'
              << "Unsigned value (hex): " << std::hex << unsigned_value << '\n'
              << "Unsigned value (oct): " << std::oct << unsigned_value << std::endl;

    // Hex also can be manipulated to be uppercase
    std::cout << "Upper unsigned value: " << std::hex << std::uppercase << unsigned_value << std::endl;

    // And returned to default formatting
    std::cout << "Lower unsigned value: " << std::dec << std::nouppercase << unsigned_value << std::endl;

    // Large values that exceed 64-bit range
    std::cout << "\n=== Large Values (Beyond 64-bit) ===" << std::endl;

    // 2^64 = 18446744073709551616 (first value that doesn't fit in uint64_t)
    constexpr uint128_t two_to_64 {1, 0};
    std::cout << "2^64 = " << two_to_64 << std::endl;

    // 2^100 = a very large number
    constexpr uint128_t two_to_100 {uint128_t{1} << 100};
    std::cout << "2^100 = " << two_to_100 << std::endl;

    // Maximum uint128_t value
    constexpr auto uint_max {std::numeric_limits<uint128_t>::max()};
    std::cout << "uint128_t max = " << uint_max << std::endl;

    // Minimum and maximum int128_t values
    constexpr auto int_min {std::numeric_limits<int128_t>::min()};
    constexpr auto int_max {std::numeric_limits<int128_t>::max()};
    std::cout << "int128_t min = " << int_min << std::endl;
    std::cout << "int128_t max = " << int_max << std::endl;

    // String conversion using stringstream
    std::cout << "\n=== String Conversion with std::stringstream ===" << std::endl;

    // Convert uint128_t to string
    std::ostringstream oss;
    oss << two_to_100;
    auto str {oss.str()};
    std::cout << "uint128_t to string: \"" << str << "\"" << std::endl;

    // Convert string to uint128_t
    std::istringstream iss {"123456789012345678901234567890"};
    uint128_t parsed_value {};
    iss >> parsed_value;
    std::cout << "String to uint128_t: " << parsed_value << std::endl;

    // Round-trip: value -> string -> value
    std::cout << "\n=== Round-trip Conversion ===" << std::endl;
    constexpr uint128_t original {0xDEADBEEF, 0xCAFEBABE12345678};
    std::ostringstream oss2;
    oss2 << original;
    auto original_str {oss2.str()};

    std::istringstream iss2 {original_str};
    uint128_t round_tripped {};
    iss2 >> round_tripped;

    std::cout << "Original:     " << original << std::endl;
    std::cout << "As string:    \"" << original_str << "\"" << std::endl;
    std::cout << "Round-tripped: " << round_tripped << std::endl;
    std::cout << "Match: " << std::boolalpha << (original == round_tripped) << std::endl;

    return 0;
}
