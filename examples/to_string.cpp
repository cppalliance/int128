// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128/int128.hpp>
#include <boost/int128/string.hpp>
#include <boost/int128/literals.hpp>
#include <iostream>
#include <string>
#include <cstdint>

int main()
{
    using boost::int128::uint128_t;
    using boost::int128::int128_t;
    using boost::int128::to_string;
    using namespace boost::int128::literals;

    std::cout << "=== to_string with uint128_t ===" << std::endl;

    // Compare against std::to_string for values that fit in 64 bits
    constexpr uint128_t u_small {UINT64_C(1234567890)};
    const auto u_small_str {to_string(u_small)};
    const auto u_small_std {std::to_string(std::uint64_t{1234567890})};
    std::cout << "uint128_t to_string(1234567890): " << u_small_str << std::endl;
    std::cout << "std::to_string(uint64_t 1234567890): " << u_small_std << std::endl;
    std::cout << "Match: " << std::boolalpha << (u_small_str == u_small_std) << std::endl;

    constexpr uint128_t u_max64 {UINT64_MAX};
    const auto u_max64_str {to_string(u_max64)};
    const auto u_max64_std {std::to_string(UINT64_MAX)};
    std::cout << "\nuint128_t to_string(UINT64_MAX): " << u_max64_str << std::endl;
    std::cout << "std::to_string(UINT64_MAX):      " << u_max64_std << std::endl;
    std::cout << "Match: " << (u_max64_str == u_max64_std) << std::endl;

    // Values beyond 64-bit range
    const auto large_unsigned {"340282366920938463463374607431768211455"_U128};
    std::cout << "\nuint128_t max: " << to_string(large_unsigned) << std::endl;

    std::cout << "\n=== to_string with int128_t ===" << std::endl;

    // Compare against std::to_string for values that fit in 64 bits
    constexpr int128_t s_negative {-42};
    const auto s_neg_str {to_string(s_negative)};
    const auto s_neg_std {std::to_string(std::int64_t{-42})};
    std::cout << "int128_t to_string(-42): " << s_neg_str << std::endl;
    std::cout << "std::to_string(int64_t -42): " << s_neg_std << std::endl;
    std::cout << "Match: " << (s_neg_str == s_neg_std) << std::endl;

    constexpr int128_t s_large {INT64_MAX};
    const auto s_large_str {to_string(s_large)};
    const auto s_large_std {std::to_string(INT64_MAX)};
    std::cout << "\nint128_t to_string(INT64_MAX): " << s_large_str << std::endl;
    std::cout << "std::to_string(INT64_MAX):     " << s_large_std << std::endl;
    std::cout << "Match: " << (s_large_str == s_large_std) << std::endl;

    // Values beyond 64-bit range
    const auto large_negative {"-170141183460469231731687303715884105728"_i128};
    std::cout << "\nint128_t min: " << to_string(large_negative) << std::endl;

    const auto large_positive {"170141183460469231731687303715884105727"_I128};
    std::cout << "int128_t max: " << to_string(large_positive) << std::endl;

    return 0;
}
