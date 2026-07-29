// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128/int128.hpp>
#include <boost/int128/numeric.hpp>
#include <boost/int128/iostream.hpp>
#include <iostream>
#include <limits>

int main()
{
    using boost::int128::uint128_t;
    using boost::int128::int128_t;

    std::cout << "=== Every rounding mode on -12 / 5 ===" << std::endl;

    // The exact quotient is -2.4, so the nearest integer is -2 and no mode ties
    constexpr int128_t x {-12};
    constexpr int128_t y {5};

    std::cout << "div_to_zero        = " << boost::int128::div_to_zero(x, y) << std::endl;
    std::cout << "div_away_zero      = " << boost::int128::div_away_zero(x, y) << std::endl;
    std::cout << "div_to_pos_inf     = " << boost::int128::div_to_pos_inf(x, y) << std::endl;
    std::cout << "div_to_neg_inf     = " << boost::int128::div_to_neg_inf(x, y) << std::endl;
    std::cout << "div_euclid         = " << boost::int128::div_euclid(x, y) << std::endl;
    std::cout << "div_ties_to_zero   = " << boost::int128::div_ties_to_zero(x, y) << std::endl;

    std::cout << "\n=== Tie breaking on -7 / 2 ===" << std::endl;

    // The exact quotient is -3.5, so every tie-breaking rule picks a different side
    constexpr int128_t tie_x {-7};
    constexpr int128_t tie_y {2};

    std::cout << "div_ties_to_zero     = " << boost::int128::div_ties_to_zero(tie_x, tie_y) << std::endl;
    std::cout << "div_ties_away_zero   = " << boost::int128::div_ties_away_zero(tie_x, tie_y) << std::endl;
    std::cout << "div_ties_to_pos_inf  = " << boost::int128::div_ties_to_pos_inf(tie_x, tie_y) << std::endl;
    std::cout << "div_ties_to_neg_inf  = " << boost::int128::div_ties_to_neg_inf(tie_x, tie_y) << std::endl;
    std::cout << "div_ties_to_odd      = " << boost::int128::div_ties_to_odd(tie_x, tie_y) << std::endl;
    std::cout << "div_ties_to_even     = " << boost::int128::div_ties_to_even(tie_x, tie_y) << std::endl;

    std::cout << "\n=== Quotient and remainder from one division ===" << std::endl;

    // Each div_rem_ function performs a single division and returns both halves
    constexpr auto floored {boost::int128::div_rem_to_neg_inf(x, y)};
    std::cout << "div_rem_to_neg_inf(-12, 5): quotient = " << floored.quotient
              << ", remainder = " << floored.remainder << std::endl;

    constexpr auto truncated {boost::int128::div_rem_to_zero(x, y)};
    std::cout << "div_rem_to_zero(-12, 5):    quotient = " << truncated.quotient
              << ", remainder = " << truncated.remainder << std::endl;

    // The remainder always satisfies x == quotient * y + remainder
    std::cout << "quotient * y + remainder = " << (floored.quotient * y + floored.remainder) << std::endl;

    std::cout << "\n=== Euclidean remainder is never negative ===" << std::endl;

    // operator% takes its sign from the dividend, which makes it a poor fit for
    // wrapping an offset into a range. rem_euclid always lands in [0, abs(y)).
    constexpr int128_t modulus {7};
    for (int128_t offset {-9}; offset <= -6; ++offset)
    {
        std::cout << offset << " % 7 = " << (offset % modulus)
                  << ", rem_euclid(" << offset << ", 7) = " << boost::int128::rem_euclid(offset, modulus) << std::endl;
    }

    std::cout << "\n=== Ceiling division without overflow ===" << std::endl;

    // Counting fixed size blocks needed to cover a length is the classic use for
    // rounding towards positive infinity. The usual (length + block - 1) / block
    // overflows here, while div_to_pos_inf does not.
    constexpr auto length {(std::numeric_limits<uint128_t>::max)()};
    constexpr uint128_t block {1000};

    std::cout << "length = " << length << std::endl;
    std::cout << "div_to_pos_inf(length, 1000) = " << boost::int128::div_to_pos_inf(length, block) << std::endl;
    std::cout << "div_to_zero(length, 1000)    = " << boost::int128::div_to_zero(length, block) << std::endl;
    std::cout << "(length + 999) / 1000        = " << ((length + 999U) / block) << " (wrapped)" << std::endl;

    std::cout << "\n=== Unbiased rounding of a scaled value ===" << std::endl;

    // Rounding half to even keeps a long running sum from drifting upwards, which is
    // what operator/ plus a manual half-adjustment would do
    constexpr int128_t scale {1000};
    const int128_t samples[] {int128_t{1500}, int128_t{2500}, int128_t{3500}, int128_t{-1500}};

    for (const auto sample : samples)
    {
        std::cout << sample << " / 1000: ties_to_even = " << boost::int128::div_ties_to_even(sample, scale)
                  << ", ties_away_zero = " << boost::int128::div_ties_away_zero(sample, scale) << std::endl;
    }

    return 0;
}
