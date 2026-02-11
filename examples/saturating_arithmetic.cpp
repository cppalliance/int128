// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// Individual headers

#include <boost/int128/int128.hpp>
#include <boost/int128/numeric.hpp>
#include <boost/int128/iostream.hpp>

// Or you can do a single header

// #include <boost/int128.hpp>

#include <limits>
#include <type_traits>
#include <iostream>

int main()
{
    using boost::int128::uint128_t;
    using boost::int128::int128_t;

    // std::numeric_limits is overloaded for both types
    constexpr auto uint_max {std::numeric_limits<uint128_t>::max()};
    static_assert(std::is_same<decltype(uint_max), const uint128_t>::value, "Types should match");

    constexpr auto int_max {std::numeric_limits<int128_t>::max()};
    constexpr auto int_min {std::numeric_limits<int128_t>::min()};

    std::cout << "=== Saturating Arithmetic ===" << std::endl;
    std::cout << "uint128_t max = " << uint_max << std::endl;
    std::cout << "int128_t max  = " << int_max << std::endl;
    std::cout << "int128_t min  = " << int_min << std::endl;

    // Saturating arithmetic returns max on overflow, or min on underflow rather than rolling over
    std::cout << "\n=== Saturating Addition and Subtraction ===" << std::endl;
    std::cout << "add_sat(uint_max, uint_max) = " << boost::int128::add_sat(uint_max, uint_max)
              << " (saturates to uint_max)" << std::endl;
    std::cout << "sub_sat(0, uint_max) = " << boost::int128::sub_sat(uint128_t{0}, uint_max)
              << " (saturates to 0, not underflow)" << std::endl;

    // This is especially useful for signed types since rollover is undefined
    std::cout << "\n=== Saturating Multiplication ===" << std::endl;
    std::cout << "mul_sat(int_max, 2) = " << boost::int128::mul_sat(int_max, 2)
              << " (saturates to int_max)" << std::endl;
    std::cout << "mul_sat(-(int_max - 2), 5) = " << boost::int128::mul_sat(-(int_max - 2), 5)
              << " (saturates to int_min)" << std::endl;

    // The only case in the library where div_sat overflows is x = int_min and y = -1
    std::cout << "\n=== Saturating Division ===" << std::endl;
    std::cout << "div_sat(int_min, -1) = " << boost::int128::div_sat(int_min, -1)
              << " (saturates to int_max; normally this overflows)" << std::endl;

    // saturate_cast allows types to be safely converted without rollover behavior
    std::cout << "\n=== Saturating Casts ===" << std::endl;
    std::cout << "saturate_cast<int128_t>(uint_max) = " << boost::int128::saturate_cast<int128_t>(uint_max)
              << " (saturates to int_max)" << std::endl;

    // You can also cast to builtin types
    std::cout << "saturate_cast<int64_t>(int_max) = " << boost::int128::saturate_cast<std::int64_t>(int_max)
              << " (saturates to INT64_MAX)" << std::endl;

    // Even of different signedness as this is treated like a static cast
    std::cout << "saturate_cast<int32_t>(uint_max) = " << boost::int128::saturate_cast<std::int32_t>(uint_max)
              << " (saturates to INT32_MAX)" << std::endl;

    return 0;
}
