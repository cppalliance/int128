// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128/int128.hpp>
#include <cstdint>

#ifdef __clang__
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wunused-variable"
#endif

int main()
{
    // The types of this library support all arithmetic operations one would expect
    // They can be between values of the same type, or same signedness by default.
    // See `mixed_type_arithmetic.cpp` for other operations
    boost::int128::int128_t x {42};
    boost::int128::int128_t y {-2};

    // Comparisons
    assert(x > y);
    assert(x >= y);
    assert(y < x);
    assert(y <= x);
    assert(x == x);
    assert(y != x);

    // Add,sub,mul,div
    assert(x + y == 40);
    assert(y - x == -44);
    assert(x * y == -84);
    assert(x / y == -21);
    assert(x % y == 0);

    // Absolute Value
    // For generic programming add using boost::int128::abs; at the top of the function,
    // so that it will be found by ADL just by writing abs(y);
    assert(boost::int128::abs(y) == 2);

    // Compound operations
    x /= y;
    assert(x == -21);

    y += 4L;
    assert(y == 2);

    y *= -5LL;
    assert(y == -10);

    return 0;
}
