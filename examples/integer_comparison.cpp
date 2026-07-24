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
    using boost::int128::cmp_equal;
    using boost::int128::cmp_less;
    using boost::int128::in_range;

    std::cout << std::boolalpha;

    constexpr auto u_max {(std::numeric_limits<uint128_t>::max)()};

    // The built-in relational operators follow the usual arithmetic conversions,
    // so comparing a uint128_t with a negative int128_t converts the negative
    // value to a huge unsigned one and gives the wrong answer. The cmp_* family
    // compares the true mathematical values instead.
    std::cout << "=== Signedness-safe comparison ===" << std::endl;
    std::cout << "UINT128_MAX == int128_t{-1} (operator):   " << (u_max == int128_t{-1}) << std::endl;
    std::cout << "cmp_equal(UINT128_MAX, int128_t{-1}):     " << cmp_equal(u_max, int128_t{-1}) << std::endl;
    std::cout << "int128_t{-1} < uint128_t{0} (operator):   " << (int128_t{-1} < uint128_t{0}) << std::endl;
    std::cout << "cmp_less(int128_t{-1}, uint128_t{0}):     " << cmp_less(int128_t{-1}, uint128_t{0}) << std::endl;

    // Either operand may be a builtin integer of any width and signedness.
    std::cout << "\n=== Mixed with builtin integers ===" << std::endl;
    std::cout << "cmp_less(int128_t{-5}, 0u):               " << cmp_less(int128_t{-5}, 0U) << std::endl;
    std::cout << "cmp_equal(uint128_t{42}, 42):             " << cmp_equal(uint128_t{42}, 42) << std::endl;

    // in_range<R>(v) reports whether v is representable in the target type R.
    // R and the type of v may each be a builtin integer or a 128-bit type.
    std::cout << "\n=== in_range ===" << std::endl;
    std::cout << "in_range<std::int8_t>(int128_t{200}):     " << in_range<std::int8_t>(int128_t{200}) << std::endl;
    std::cout << "in_range<std::uint8_t>(int128_t{-1}):     " << in_range<std::uint8_t>(int128_t{-1}) << std::endl;
    std::cout << "in_range<std::uint64_t>(UINT128_MAX):     " << in_range<std::uint64_t>(u_max) << std::endl;
    std::cout << "in_range<int128_t>(UINT128_MAX):          " << in_range<int128_t>(u_max) << std::endl;

    return 0;
}
