// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128/int128.hpp>
#include <boost/int128/numeric.hpp>
#include <boost/int128/iostream.hpp>
#include <iostream>

int main()
{
    using boost::int128::uint128_t;
    using boost::int128::int128_t;

    std::cout << "=== Greatest Common Divisor (gcd) ===" << std::endl;

    // Basic gcd
    constexpr uint128_t a {48};
    constexpr uint128_t b {18};
    std::cout << "gcd(" << a << ", " << b << ") = " << boost::int128::gcd(a, b) << std::endl;

    // gcd with larger values
    constexpr uint128_t large_a {123456789012345678ULL};
    constexpr uint128_t large_b {987654321098765432ULL};
    std::cout << "gcd(" << large_a << ", " << large_b << ") = "
              << boost::int128::gcd(large_a, large_b) << std::endl;

    // gcd with 128-bit values
    constexpr uint128_t huge_a {uint128_t{1} << 100};
    constexpr uint128_t huge_b {uint128_t{1} << 80};
    std::cout << "gcd(2^100, 2^80) = " << boost::int128::gcd(huge_a, huge_b) << " (= 2^80)" << std::endl;

    // Signed gcd (always returns positive)
    constexpr int128_t neg_a {-48};
    constexpr int128_t neg_b {18};
    std::cout << "gcd(" << neg_a << ", " << neg_b << ") = "
              << boost::int128::gcd(neg_a, neg_b) << " (always positive)" << std::endl;

    std::cout << "\n=== Least Common Multiple (lcm) ===" << std::endl;

    // Basic lcm
    constexpr uint128_t x {12};
    constexpr uint128_t y {18};
    std::cout << "lcm(" << x << ", " << y << ") = " << boost::int128::lcm(x, y) << std::endl;

    // lcm with coprime numbers
    constexpr uint128_t p {7};
    constexpr uint128_t q {11};
    std::cout << "lcm(" << p << ", " << q << ") = " << boost::int128::lcm(p, q)
              << " (coprime: lcm = p * q)" << std::endl;

    // Relationship: gcd(a,b) * lcm(a,b) = a * b
    std::cout << "\nVerifying gcd * lcm = a * b:" << std::endl;
    auto g {boost::int128::gcd(x, y)};
    auto l {boost::int128::lcm(x, y)};
    std::cout << "gcd(" << x << ", " << y << ") * lcm(" << x << ", " << y << ") = "
              << (g * l) << std::endl;
    std::cout << x << " * " << y << " = " << (x * y) << std::endl;

    std::cout << "\n=== Midpoint ===" << std::endl;

    // Unsigned midpoint
    constexpr uint128_t low {10};
    constexpr uint128_t high {20};
    std::cout << "midpoint(" << low << ", " << high << ") = "
              << boost::int128::midpoint(low, high) << std::endl;

    // Midpoint with odd sum (rounds toward first argument)
    constexpr uint128_t odd_low {10};
    constexpr uint128_t odd_high {21};
    std::cout << "midpoint(" << odd_low << ", " << odd_high << ") = "
              << boost::int128::midpoint(odd_low, odd_high) << " (rounds toward first arg)" << std::endl;
    std::cout << "midpoint(" << odd_high << ", " << odd_low << ") = "
              << boost::int128::midpoint(odd_high, odd_low) << " (rounds toward first arg)" << std::endl;

    // Midpoint avoids overflow (unlike (a+b)/2)
    std::cout << "\n--- Overflow-safe midpoint ---" << std::endl;
    constexpr auto uint_max {std::numeric_limits<uint128_t>::max()};
    constexpr auto uint_max_minus_10 {uint_max - 10U};
    std::cout << "midpoint(uint128_max, uint128_max - 10) = "
              << boost::int128::midpoint(uint_max, uint_max_minus_10) << std::endl;
    std::cout << "(This would overflow if computed as (a + b) / 2)" << std::endl;

    // Signed midpoint
    std::cout << "\n--- Signed midpoint ---" << std::endl;
    constexpr int128_t neg {-100};
    constexpr int128_t pos {100};
    std::cout << "midpoint(" << neg << ", " << pos << ") = "
              << boost::int128::midpoint(neg, pos) << std::endl;

    constexpr int128_t neg2 {-100};
    constexpr int128_t neg3 {-50};
    std::cout << "midpoint(" << neg2 << ", " << neg3 << ") = "
              << boost::int128::midpoint(neg2, neg3) << std::endl;

    return 0;
}