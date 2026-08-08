// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// This example demonstrates std::format support for int128 types.
// Including <boost/int128/format.hpp> defines the std::formatter specializations,
// and BOOST_INT128_HAS_FORMAT reports whether C++20 <format> was available.
//
// Apart from the final section, this is the same program as fmt_format.cpp with
// fmt:: replaced by std::, and those lines of output are identical.

#include <boost/int128/int128.hpp>
#include <boost/int128/format.hpp>

// tag::exclude[]
#ifdef BOOST_INT128_HAS_FORMAT

// end::exclude[]
#include <format>
#include <iostream>
#include <iterator>
#include <limits>
#include <string>

#ifdef BOOST_INT128_HAS_CONSTEXPR_FORMAT

// With C++26 constexpr std::format the entire call happens at compile time
static_assert(std::format("{:#x}", boost::int128::uint128 {255}) == "0xff");
static_assert(std::format("{:+}", boost::int128::int128 {-42}) == "-42");

#endif

int main()
{
    using boost::int128::int128;
    using boost::int128::uint128;

    std::cout << "=== Basic Formatting ===" << std::endl;

    constexpr uint128 unsigned_value {0xDEADBEEF, 0xCAFEBABE12345678};
    constexpr int128 signed_value {-123456789012345678};

    // Default decimal formatting
    std::cout << std::format("Default (decimal): {}", unsigned_value) << std::endl;
    std::cout << std::format("Signed value: {}", signed_value) << std::endl;

    std::cout << "\n=== Base Specifiers ===" << std::endl;

    // Different bases: binary, octal, decimal, hex
    constexpr uint128 value {255};
    std::cout << std::format("Binary:      {:b}", value) << std::endl;
    std::cout << std::format("Octal:       {:o}", value) << std::endl;
    std::cout << std::format("Decimal:     {:d}", value) << std::endl;
    std::cout << std::format("Hexadecimal: {:x}", value) << std::endl;
    std::cout << std::format("Hex (upper): {:X}", value) << std::endl;

    std::cout << "\n=== Alternate Form (Prefixes) ===" << std::endl;

    // Using # for alternate form adds base prefixes
    std::cout << std::format("Binary with prefix:  {:#b}", value) << std::endl;
    std::cout << std::format("Octal with prefix:   {:#o}", value) << std::endl;
    std::cout << std::format("Hex with prefix:     {:#x}", value) << std::endl;
    std::cout << std::format("Hex upper prefix:    {:#X}", value) << std::endl;

    std::cout << "\n=== Sign Options ===" << std::endl;

    constexpr int128 positive {42};
    constexpr int128 negative {-42};

    // Sign specifiers: + (always show), - (default), space (space for positive)
    std::cout << std::format("Plus sign:  {:+} and {:+}", positive, negative) << std::endl;
    std::cout << std::format("Minus only: {} and {}", positive, negative) << std::endl;
    std::cout << std::format("Space sign: {: } and {: }", positive, negative) << std::endl;

    std::cout << "\n=== Zero Padding ===" << std::endl;

    // Padding with zeros (no alignment specifier)
    std::cout << std::format("8-digit padding:  {:08}", value) << std::endl;
    std::cout << std::format("16-digit padding: {:016}", value) << std::endl;

    std::cout << "\n=== Alignment ===" << std::endl;

    // Left, right, and center alignment with default fill (space)
    std::cout << std::format("Left align:   '{:<10}'", positive) << std::endl;
    std::cout << std::format("Right align:  '{:>10}'", positive) << std::endl;
    std::cout << std::format("Center align: '{:^10}'", positive) << std::endl;

    std::cout << "\n=== Alignment with Fill Characters ===" << std::endl;

    // Custom fill characters
    std::cout << std::format("Left with *:   '{:*<10}'", positive) << std::endl;
    std::cout << std::format("Right with 0:  '{:0>10}'", positive) << std::endl;
    std::cout << std::format("Center with -: '{:-^10}'", positive) << std::endl;

    std::cout << "\n=== Alignment with Sign ===" << std::endl;

    // Alignment combined with sign specifiers
    std::cout << std::format("Right align +:  '{:>+10}'", positive) << std::endl;
    std::cout << std::format("Left align +:   '{:<+10}'", positive) << std::endl;
    std::cout << std::format("Center align +: '{:^+11}'", positive) << std::endl;
    std::cout << std::format("Right align -:  '{:*>10}'", negative) << std::endl;

    std::cout << "\n=== Alignment with Hex and Prefix ===" << std::endl;

    // Alignment with base specifiers and prefixes
    std::cout << std::format("Right align hex:  '{:>10x}'", value) << std::endl;
    std::cout << std::format("Left align hex:   '{:<10x}'", value) << std::endl;
    std::cout << std::format("Center with prefix: '{:*^#12x}'", value) << std::endl;

    std::cout << "\n=== Large Values ===" << std::endl;

    // Demonstrate with values beyond 64-bit range
    constexpr auto uint_max {std::numeric_limits<uint128>::max()};
    constexpr auto int_min {std::numeric_limits<int128>::min()};

    std::cout << std::format("uint128 max: {}", uint_max) << std::endl;
    std::cout << std::format("uint128 max (hex): {:#x}", uint_max) << std::endl;
    std::cout << std::format("int128 min: {}", int_min) << std::endl;

    std::cout << "\n=== Combined Format Specifiers ===" << std::endl;

    // Combining multiple specifiers
    std::cout << std::format("Hex with prefix, uppercase, padded: {:#016X}", unsigned_value) << std::endl;
    std::cout << std::format("Signed with plus, padded: {:+020}", signed_value) << std::endl;

    std::cout << "\n=== Formatting Into an Existing Buffer ===" << std::endl;

    // formatted_size gives the exact length, and format_to writes through any
    // output iterator, so no intermediate std::string is required
    const auto length {std::formatted_size("{:#x}", uint_max)};
    std::string buffer {};
    buffer.reserve(length);
    std::format_to(std::back_inserter(buffer), "{:#x}", uint_max);

    std::cout << std::format("formatted_size: {}", length) << std::endl;
    std::cout << std::format("format_to:      {}", buffer) << std::endl;
    std::cout << std::format("Lengths match:  {}", buffer.size() == length) << std::endl;

    return 0;
}
// tag::exclude[]

#else

#include <iostream>

int main()
{
    std::cout << "This example requires C++20 <format> support." << std::endl;
    return 0;
}

#endif
// end::exclude[]
