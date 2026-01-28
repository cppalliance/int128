// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// This example demonstrates {fmt} library integration with int128 types.
// Requires {fmt} to be installed: https://github.com/fmtlib/fmt
//
// For C++20 std::format support, use <boost/int128/format.hpp> instead,
// which provides the same formatting capabilities with std::format.

#define FMT_HEADER_ONLY
#include <boost/int128/int128.hpp>
#include <boost/int128/fmt_format.hpp>

#ifdef BOOST_INT128_HAS_FMT_FORMAT

#include <fmt/format.h>
#include <iostream>

int main()
{
    using boost::int128::int128_t;
    using boost::int128::uint128_t;

    std::cout << "=== Basic Formatting ===" << std::endl;

    constexpr uint128_t unsigned_value {0xDEADBEEF, 0xCAFEBABE12345678};
    constexpr int128_t signed_value {-123456789012345678};

    // Default decimal formatting
    std::cout << fmt::format("Default (decimal): {}", unsigned_value) << std::endl;
    std::cout << fmt::format("Signed value: {}", signed_value) << std::endl;

    std::cout << "\n=== Base Specifiers ===" << std::endl;

    // Different bases: binary, octal, decimal, hex
    constexpr uint128_t value {255};
    std::cout << fmt::format("Binary:      {:b}", value) << std::endl;
    std::cout << fmt::format("Octal:       {:o}", value) << std::endl;
    std::cout << fmt::format("Decimal:     {:d}", value) << std::endl;
    std::cout << fmt::format("Hexadecimal: {:x}", value) << std::endl;
    std::cout << fmt::format("Hex (upper): {:X}", value) << std::endl;

    std::cout << "\n=== Alternate Form (Prefixes) ===" << std::endl;

    // Using # for alternate form adds base prefixes
    std::cout << fmt::format("Binary with prefix:  {:#b}", value) << std::endl;
    std::cout << fmt::format("Octal with prefix:   {:#o}", value) << std::endl;
    std::cout << fmt::format("Hex with prefix:     {:#x}", value) << std::endl;
    std::cout << fmt::format("Hex upper prefix:    {:#X}", value) << std::endl;

    std::cout << "\n=== Sign Options ===" << std::endl;

    constexpr int128_t positive {42};
    constexpr int128_t negative {-42};

    // Sign specifiers: + (always show), - (default), space (space for positive)
    std::cout << fmt::format("Plus sign:  {:+} and {:+}", positive, negative) << std::endl;
    std::cout << fmt::format("Minus only: {} and {}", positive, negative) << std::endl;
    std::cout << fmt::format("Space sign: {: } and {: }", positive, negative) << std::endl;

    std::cout << "\n=== Zero Padding ===" << std::endl;

    // Padding with zeros
    std::cout << fmt::format("8-digit padding:  {:08}", value) << std::endl;
    std::cout << fmt::format("16-digit padding: {:016}", value) << std::endl;

    std::cout << "\n=== Large Values ===" << std::endl;

    // Demonstrate with values beyond 64-bit range
    constexpr auto uint_max {std::numeric_limits<uint128_t>::max()};
    constexpr auto int_min {std::numeric_limits<int128_t>::min()};

    std::cout << fmt::format("uint128_t max: {}", uint_max) << std::endl;
    std::cout << fmt::format("uint128_t max (hex): {:#x}", uint_max) << std::endl;
    std::cout << fmt::format("int128_t min: {}", int_min) << std::endl;

    std::cout << "\n=== Combined Format Specifiers ===" << std::endl;

    // Combining multiple specifiers
    std::cout << fmt::format("Hex with prefix, uppercase, padded: {:#016X}", unsigned_value) << std::endl;
    std::cout << fmt::format("Signed with plus, padded: {:+020}", signed_value) << std::endl;

    return 0;
}

#else

#include <iostream>

int main()
{
    std::cout << "This example requires {fmt} library to be installed." << std::endl;
    std::cout << "Install from: https://github.com/fmtlib/fmt" << std::endl;
    return 0;
}

#endif
