// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128/int128.hpp>
#include <boost/int128/cstdlib.hpp>
#include <boost/int128/iostream.hpp>
#include <iostream>

int main()
{
    using boost::int128::uint128;
    using boost::int128::int128;

    std::cout << "=== div() Function ===" << std::endl;
    std::cout << "Returns both quotient and remainder in a single operation" << std::endl;

    // Unsigned division
    std::cout << "\n--- Unsigned Division ---" << std::endl;

    constexpr uint128 dividend {1000000000000000000ULL};
    constexpr uint128 divisor {7};

    auto uresult {boost::int128::div(dividend, divisor)};
    std::cout << dividend << " / " << divisor << " = " << uresult.quot
              << " remainder " << uresult.rem << std::endl;

    // Verify: quot * divisor + rem == dividend
    std::cout << "Verification: " << uresult.quot << " * " << divisor
              << " + " << uresult.rem << " = " << (uresult.quot * divisor + uresult.rem) << std::endl;

    // Large value division
    std::cout << "\n--- Large Value Division ---" << std::endl;

    constexpr uint128 large_dividend {uint128{1} << 100};  // 2^100
    constexpr uint128 large_divisor {uint128{1} << 50};    // 2^50

    auto large_result {boost::int128::div(large_dividend, large_divisor)};
    std::cout << "2^100 / 2^50 = " << large_result.quot
              << " remainder " << large_result.rem << std::endl;

    // Signed division
    std::cout << "\n--- Signed Division ---" << std::endl;

    constexpr int128 signed_dividend {-100};
    constexpr int128 signed_divisor {7};

    auto sresult {boost::int128::div(signed_dividend, signed_divisor)};
    std::cout << signed_dividend << " / " << signed_divisor << " = " << sresult.quot
              << " remainder " << sresult.rem << std::endl;

    // Different sign combinations
    std::cout << "\n--- Sign Combinations ---" << std::endl;

    constexpr int128 pos {17};
    constexpr int128 neg {-17};
    constexpr int128 div_pos {5};
    constexpr int128 div_neg {-5};

    auto pp {boost::int128::div(pos, div_pos)};
    auto pn {boost::int128::div(pos, div_neg)};
    auto np {boost::int128::div(neg, div_pos)};
    auto nn {boost::int128::div(neg, div_neg)};

    std::cout << " 17 /  5 = " << pp.quot << " remainder " << pp.rem << std::endl;
    std::cout << " 17 / -5 = " << pn.quot << " remainder " << pn.rem << std::endl;
    std::cout << "-17 /  5 = " << np.quot << " remainder " << np.rem << std::endl;
    std::cout << "-17 / -5 = " << nn.quot << " remainder " << nn.rem << std::endl;

    // Edge case: dividend smaller than divisor
    std::cout << "\n--- Edge Cases ---" << std::endl;

    auto small_div {boost::int128::div(uint128{3}, uint128{10})};
    std::cout << "3 / 10 = " << small_div.quot << " remainder " << small_div.rem << std::endl;

    return 0;
}