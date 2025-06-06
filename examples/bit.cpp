// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// Individual headers

#include <boost/int128/int128.hpp>
#include <boost/int128/bit.hpp>
#include <iostream>
#include <iomanip>

// Or you can do a single header

// #include <boost/int128.hpp>

#ifdef __clang__
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wunused-variable"
#endif


int main()
{
    // The functions from bit are only available for uint128_t

    constexpr boost::int128::uint128_t x {1U};

    // All the functions are constexpr

    // Does the value have only a single bit set?
    static_assert(boost::int128::has_single_bit(x), "Should have one bit");

    // How many zeros from the left
    static_assert(boost::int128::countl_zero(x) == 127U, "Should be 127");

    // The bit width of the value
    // 1 + 1 is 10 in binary which is 2 bits wide
    static_assert(boost::int128::bit_width(x + x) == 2U, "2 bits wide");

    // The smallest power of two not greater than the input value
    static_assert(boost::int128::bit_floor(3U * x) == 2U, "2 < 3");

    // The smallest power of two not Smaller than the input value
    static_assert(boost::int128::bit_ceil(5U * x) == 8U, "8 > 5");

    // How many zeros from the right?
    static_assert(boost::int128::countr_zero(2U * x) == 1, "1 zero to the right of 10");

    // How many 1-bits in the value
    static_assert(boost::int128::popcount(7U * x) == 3, "111");

    // Swap the bytes
    // Create a value with distinct byte pattern
    constexpr boost::int128::uint128_t original{
        0x0123456789ABCDEFULL,
        0xFEDCBA9876543210ULL
    };

    // Expected result after byteswap
    constexpr boost::int128::uint128_t expected{
        0x1032547698BADCFEULL,
        0xEFCDAB8967452301ULL
    };

    static_assert(boost::int128::byteswap(original) == expected, "Mismatched byteswap");
    static_assert(boost::int128::byteswap(expected) == original, "Mismatched byteswap");

    return 0;
}
