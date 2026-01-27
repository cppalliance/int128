// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// Allowing sign conversion is a required pre-requisite for Boost.Charconv
#define BOOST_INT128_ALLOW_SIGN_CONVERSION

#include <boost/int128/int128.hpp>
#include <boost/int128/iostream.hpp>
#include <boost/int128/charconv.hpp>
#include <boost/charconv.hpp>
#include <iostream>
#include <limits>
#include <cstring>

int main()
{
    using boost::int128::uint128_t;
    using boost::int128::int128_t;

    char buffer[64];

    // === to_chars: Convert integers to character strings ===
    std::cout << "=== to_chars ===" << std::endl;

    // Unsigned 128-bit to decimal string
    constexpr uint128_t max_u128 {std::numeric_limits<uint128_t>::max()};
    auto result {boost::charconv::to_chars(buffer, buffer + sizeof(buffer), max_u128)};
    *result.ptr = '\0';
    std::cout << "uint128_t max (decimal): " << buffer << std::endl;

    // Signed 128-bit to decimal string
    constexpr int128_t min_i128 {std::numeric_limits<int128_t>::min()};
    result = boost::charconv::to_chars(buffer, buffer + sizeof(buffer), min_i128);
    *result.ptr = '\0';
    std::cout << "int128_t min (decimal):  " << buffer << std::endl;

    // Hexadecimal output (base 16)
    uint128_t hex_value {UINT64_C(0xDEADBEEF), UINT64_C(0xCAFEBABE12345678)};
    result = boost::charconv::to_chars(buffer, buffer + sizeof(buffer), hex_value, 16);
    *result.ptr = '\0';
    std::cout << "uint128_t (hex): 0x" << buffer << std::endl;

    // Octal output (base 8)
    result = boost::charconv::to_chars(buffer, buffer + sizeof(buffer), int128_t{511}, 8);
    *result.ptr = '\0';
    std::cout << "int128_t 511 (octal): 0" << buffer << std::endl;

    // === from_chars: Parse character strings to integers ===
    std::cout << "\n=== from_chars ===" << std::endl;

    // Parse decimal string to uint128_t
    const char* decimal_str {"340282366920938463463374607431768211455"};
    uint128_t parsed_unsigned;
    boost::charconv::from_chars(decimal_str, decimal_str + std::strlen(decimal_str), parsed_unsigned);
    std::cout << "Parsed \"" << decimal_str << "\"" << std::endl;
    std::cout << "  Result: " << parsed_unsigned << std::endl;
    std::cout << "  Equals max? " << std::boolalpha << (parsed_unsigned == max_u128) << std::endl;

    // Parse negative decimal string to int128_t
    const char* negative_str {"-170141183460469231731687303715884105728"};
    int128_t parsed_signed;
    boost::charconv::from_chars(negative_str, negative_str + std::strlen(negative_str), parsed_signed);
    std::cout << "Parsed \"" << negative_str << "\"" << std::endl;
    std::cout << "  Result: " << parsed_signed << std::endl;
    std::cout << "  Equals min? " << (parsed_signed == min_i128) << std::endl;

    // Parse hexadecimal string (base 16)
    const char* hex_str {"DEADBEEFCAFEBABE12345678"};
    uint128_t parsed_hex;
    boost::charconv::from_chars(hex_str, hex_str + std::strlen(hex_str), parsed_hex, 16);
    std::cout << "Parsed hex \"" << hex_str << "\"" << std::endl;
    std::cout << "  Result: " << parsed_hex << std::endl;

    return 0;
}