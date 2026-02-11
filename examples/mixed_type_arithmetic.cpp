// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#define BOOST_INT128_ALLOW_SIGN_CONVERSION
#include <boost/int128.hpp>
#include <iostream>

int main()
{
    // By default, mixed type arithmetic is NOT ALLOWED
    // In order for this file to compile #define BOOST_INT128_ALLOW_SIGN_CONVERSION
    // BEFORE the inclusion of any file of this library (uncomment the top line)
    //
    // Unlike builtin types, we cannot enforce sign correctness via a compiler flag,
    // so we made it the default.

    std::cout << "=== Mixed Type Arithmetic with uint128_t ===" << std::endl;

    constexpr boost::int128::uint128_t unsigned_value {3};
    std::cout << "unsigned_value = " << unsigned_value << std::endl;

    constexpr auto greater_unsigned_value {unsigned_value + 5};

    std::cout << "unsigned_value + 1 = " << (unsigned_value + 1) << std::endl;
    std::cout << "unsigned_value - 1 = " << (unsigned_value - 1) << std::endl;
    std::cout << "unsigned_value * 2 = " << (unsigned_value * 2) << std::endl;
    std::cout << "unsigned_value / 3 = " << (unsigned_value / 3) << std::endl;
    std::cout << "unsigned_value % 3 = " << (unsigned_value % 3) << std::endl;
    std::cout << "unsigned_value + 5 = " << (unsigned_value + 5)
              << " (same as greater_unsigned_value: " << greater_unsigned_value << ")" << std::endl;

    std::cout << "\n=== Mixed Type Arithmetic with int128_t ===" << std::endl;

    constexpr boost::int128::int128_t signed_value {-3};
    std::cout << "signed_value = " << signed_value << std::endl;

    std::cout << "signed_value + 1U = " << (signed_value + 1U) << std::endl;
    std::cout << "signed_value - 4U = " << (signed_value - 4U) << std::endl;
    std::cout << "signed_value * 2 = " << (signed_value * 2) << std::endl;
    std::cout << "signed_value / 4U = " << (signed_value / 4U) << std::endl;

    return 0;
}
