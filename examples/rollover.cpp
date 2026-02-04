// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
//
// This example demonstrates the rollover behavior for both signed and unsigned int128

#include <boost/int128.hpp>
#include <iostream>
#include <iomanip>
#include <limits>

int main()
{
    using boost::int128::uint128_t;
    using boost::int128::int128_t;

    constexpr uint128_t max_unsigned_value {std::numeric_limits<uint128_t>::max()};
    constexpr uint128_t min_unsigned_value {std::numeric_limits<uint128_t>::min()};

    std::cout << "=== uint128_t behavior ===" << std::endl;

    std::cout << "Max of uint128_t: " << max_unsigned_value << '\n'
              << "Max + 1U: " << max_unsigned_value + 1U << "\n\n";

    std::cout << "Min of uint128_t: " << min_unsigned_value << '\n'
              << "Min - 1U: " << min_unsigned_value - 1U << "\n\n";

    constexpr int128_t max_signed_value {std::numeric_limits<int128_t>::max()};
    constexpr int128_t min_signed_value {std::numeric_limits<int128_t>::min()};

    std::cout << "=== int128_t behavior ===" << std::endl;

    std::cout << "Max of int128_t: " << max_signed_value << '\n'
              << "Max + 1: " << max_signed_value + 1 << "\n";

    std::cout << "\nMin of int128_t: " << min_signed_value << '\n'
              << "Min - 1: " << min_signed_value - 1 << '\n' << std::endl;
}
