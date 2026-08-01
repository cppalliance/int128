// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128.hpp>
#include <cstdint>
#include <iostream>

int main()
{
    // Mixed-sign comparisons and arithmetic between int128, uint128, and
    // built-in integer types of opposite signedness follow the C++ usual
    // arithmetic conversions, identical to the built-in __int128 /
    // unsigned __int128 types.

    std::cout << "=== Mixed Type Arithmetic with uint128 ===" << std::endl;

    constexpr boost::int128::uint128 unsigned_value {3};
    std::cout << "unsigned_value = " << unsigned_value << std::endl;

    constexpr auto greater_unsigned_value {unsigned_value + 5};

    std::cout << "unsigned_value + 1 = " << (unsigned_value + 1) << std::endl;
    std::cout << "unsigned_value - 1 = " << (unsigned_value - 1) << std::endl;
    std::cout << "unsigned_value * 2 = " << (unsigned_value * 2) << std::endl;
    std::cout << "unsigned_value / 3 = " << (unsigned_value / 3) << std::endl;
    std::cout << "unsigned_value % 3 = " << (unsigned_value % 3) << std::endl;
    std::cout << "unsigned_value + 5 = " << (unsigned_value + 5)
              << " (same as greater_unsigned_value: " << greater_unsigned_value << ")" << std::endl;

    std::cout << "\n=== Mixed Type Arithmetic with int128 ===" << std::endl;

    constexpr boost::int128::int128 signed_value {-3};
    std::cout << "signed_value = " << signed_value << std::endl;

    std::cout << "signed_value + 1U = " << (signed_value + 1U) << std::endl;
    std::cout << "signed_value - 4U = " << (signed_value - 4U) << std::endl;
    std::cout << "signed_value * 2 = " << (signed_value * 2) << std::endl;
    std::cout << "signed_value / 4U = " << (signed_value / 4U) << std::endl;

    std::cout << "\n=== Compound Assignment onto a Built-in Integer ===" << std::endl;

    // A built-in integer accepts a 128-bit right operand for every compound assignment.
    // The operation happens in the common type and the result is converted back, so the
    // left operand keeps its own type exactly as it would with a built-in __int128
    unsigned flags {0};
    flags |= boost::int128::uint128 {1};
    std::cout << "unsigned{0} |= uint128{1} -> " << flags << std::endl;

    int counter {12};
    counter += boost::int128::int128 {-5};
    std::cout << "int{12} += int128{-5} -> " << counter << std::endl;

    // A right operand wider than the left operand is not truncated before the operation:
    // the sum is formed in 128 bits and only the result is narrowed
    std::uint32_t word {0};
    word += (boost::int128::uint128{1} << 64U) + boost::int128::uint128{7};
    std::cout << "uint32_t{0} += 2^64 + 7 -> " << word << std::endl;

    // A shift takes only its count from the 128-bit operand, so the value shifted and the
    // type of the result are those of the left operand
    std::uint64_t bits {1};
    bits <<= boost::int128::uint128 {40};
    std::cout << "uint64_t{1} <<= uint128{40} -> " << bits << std::endl;

    return 0;
}
