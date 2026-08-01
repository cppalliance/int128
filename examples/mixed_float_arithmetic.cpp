// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128.hpp>
#include <iostream>

int main()
{
    // Operations between the 128-bit types and the built-in floating point types follow the
    // C++ usual arithmetic conversions, identical to the built-in __int128 / unsigned
    // __int128 types: the 128-bit operand is converted to the floating point type first, and
    // the result is that floating point type.

    std::cout << "=== Arithmetic ===" << std::endl;

    constexpr boost::int128::uint128 unsigned_value {5};

    std::cout << "unsigned_value      = " << unsigned_value << std::endl;
    std::cout << "unsigned_value + 1.0 = " << (unsigned_value + 1.0) << std::endl;
    std::cout << "1.0 - unsigned_value = " << (1.0 - unsigned_value) << std::endl;

    // The floating point operand is not truncated first, so this is 2.5 and not 0
    std::cout << "unsigned_value * 0.5 = " << (unsigned_value * 0.5) << std::endl;
    std::cout << "unsigned_value / 2.0 = " << (unsigned_value / 2.0) << std::endl;

    constexpr boost::int128::int128 signed_value {-5};

    std::cout << "\nsigned_value        = " << signed_value << std::endl;
    std::cout << "signed_value * 1.5   = " << (signed_value * 1.5) << std::endl;
    std::cout << "2.0 + signed_value   = " << (2.0 + signed_value) << std::endl;

    std::cout << "\n=== Comparisons ===" << std::endl;

    std::cout << std::boolalpha;
    std::cout << "unsigned_value < 5.5  = " << (unsigned_value < 5.5) << std::endl;
    std::cout << "unsigned_value == 5.0 = " << (unsigned_value == 5.0) << std::endl;
    std::cout << "signed_value < 0.0    = " << (signed_value < 0.0) << std::endl;

    std::cout << "\n=== Compound Assignment ===" << std::endl;

    // The value is converted to double, the operation is applied, and the result is
    // converted back, truncating toward zero
    boost::int128::uint128 accumulator {4};
    accumulator *= 2.5;
    std::cout << "uint128{4} *= 2.5     = " << accumulator << std::endl;

    // A floating point left operand keeps its own type
    double total {1.0};
    total += boost::int128::uint128 {2};
    std::cout << "double{1.0} += uint128{2} = " << total << std::endl;

    std::cout << "\n=== Precision ===" << std::endl;

    // A comparison converts the 128-bit value to double first, so it is only as precise as
    // a double. This is what the built-in does as well
    constexpr boost::int128::uint128 two_64 {boost::int128::uint128{1} << 64U};
    constexpr boost::int128::uint128 two_64_plus_one {two_64 + boost::int128::uint128{1}};

    std::cout << "2^64 + 1                    = " << two_64_plus_one << std::endl;
    std::cout << "2^64 + 1 == 1.8446744073709552e19 = "
              << (two_64_plus_one == 18446744073709551616.0) << std::endl;

    return 0;
}
