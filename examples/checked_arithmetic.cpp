// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// Individual headers

#include <boost/int128/utilities.hpp>
#include <boost/int128/iostream.hpp>

// Or you can do a single header

// #include <boost/int128.hpp>

#include <cstdint>
#include <limits>
#include <iostream>

int main()
{
    using boost::int128::uint128_t;
    using boost::int128::int128_t;
    using boost::int128::ckd_add;
    using boost::int128::ckd_sub;
    using boost::int128::ckd_mul;

    std::cout << std::boolalpha;

    // ckd_add, ckd_sub, and ckd_mul implement the C23 stdckdint.h contract: the
    // operation is evaluated as if both operands had infinite range, the result
    // is written to *result wrapped to that type's width, and the function
    // returns true when the exact result did not fit.
    constexpr auto u_max {std::numeric_limits<uint128_t>::max()};
    constexpr auto i_max {std::numeric_limits<int128_t>::max()};
    constexpr auto i_min {std::numeric_limits<int128_t>::min()};

    // A result that fits returns false and holds the exact value.
    std::cout << "=== Results That Fit ===" << std::endl;
    int128_t r {};
    bool overflow {ckd_add(&r, int128_t{20}, int128_t{22})};
    std::cout << "ckd_add(20, 22): overflow=" << overflow << ", result=" << r << std::endl;

    // Addition that exceeds the type wraps modulo 2^128 and reports overflow.
    std::cout << "\n=== Addition Overflow ===" << std::endl;
    uint128_t u {};
    overflow = ckd_add(&u, u_max, uint128_t{1});
    std::cout << "ckd_add(UINT128_MAX, 1): overflow=" << overflow << ", wrapped=" << u << std::endl;

    // Subtracting below zero in an unsigned type wraps to the top of the range.
    std::cout << "\n=== Subtraction Underflow ===" << std::endl;
    overflow = ckd_sub(&u, uint128_t{0}, uint128_t{1});
    std::cout << "ckd_sub(0, 1): overflow=" << overflow << ", wrapped=" << u << std::endl;

    // Multiplication detects overflow that operator* would silently roll over,
    // including INT128_MIN * -1, whose true result is not representable.
    std::cout << "\n=== Multiplication Overflow ===" << std::endl;
    overflow = ckd_mul(&r, i_max, int128_t{2});
    std::cout << "ckd_mul(INT128_MAX, 2): overflow=" << overflow << ", wrapped=" << r << std::endl;
    overflow = ckd_mul(&r, i_min, int128_t{-1});
    std::cout << "ckd_mul(INT128_MIN, -1): overflow=" << overflow << ", wrapped=" << r << std::endl;

    // The result type and the two operand types are independent: they may differ
    // in width and signedness, and the exact mathematical value is always used.
    std::cout << "\n=== Mixed Types ===" << std::endl;
    std::int64_t narrow {};
    overflow = ckd_add(&narrow, uint128_t{5}, int128_t{-3});
    std::cout << "ckd_add<int64_t>(uint128_t{5}, int128_t{-3}): overflow=" << overflow
              << ", result=" << narrow << std::endl;

    // Narrow targets make the wrap-around easy to see (400 modulo 256 is 144).
    std::uint8_t byte {};
    overflow = ckd_mul(&byte, std::uint8_t{20}, std::uint8_t{20});
    std::cout << "ckd_mul<uint8_t>(20, 20): overflow=" << overflow
              << ", wrapped=" << static_cast<int>(byte) << std::endl;

    return 0;
}
