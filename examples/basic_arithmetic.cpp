// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128/int128.hpp>
#include <boost/int128/iostream.hpp>
#include <iostream>

int main()
{
    // The types of this library support all arithmetic operations one would expect.
    // They can be between values of the same type, or the same signedness by default.
    // See `mixed_type_arithmetic.cpp` for operations with different signedness.

    using boost::int128::int128;
    using boost::int128::uint128;

    // Basic arithmetic with signed 128-bit integers
    int128 x {1000000000000LL};  // 1 trillion
    int128 y {999999999999LL};   // Just under 1 trillion

    std::cout << "=== Signed 128-bit Arithmetic ===" << std::endl;
    std::cout << "x = " << x << std::endl;
    std::cout << "y = " << y << std::endl;

    // Addition and subtraction
    std::cout << "\nAddition and Subtraction:" << std::endl;
    std::cout << "x + y = " << (x + y) << std::endl;
    std::cout << "x - y = " << (x - y) << std::endl;

    // Multiplication - results that exceed 64-bit range
    std::cout << "\nMultiplication (exceeds 64-bit range):" << std::endl;
    std::cout << "x * y = " << (x * y) << std::endl;

    // Division and modulo
    std::cout << "\nDivision and Modulo:" << std::endl;
    std::cout << "x / 7 = " << (x / 7) << std::endl;
    std::cout << "x % 7 = " << (x % 7) << std::endl;

    // Comparisons
    std::cout << "\nComparisons:" << std::endl;
    std::cout << "x > y:  " << std::boolalpha << (x > y) << std::endl;
    std::cout << "x == y: " << (x == y) << std::endl;
    std::cout << "x != y: " << (x != y) << std::endl;

    // Negative values and absolute value
    int128 negative {-42};
    std::cout << "\nNegative values:" << std::endl;
    std::cout << "negative = " << negative << std::endl;
    std::cout << "abs(negative) = " << boost::int128::abs(negative) << std::endl;

    // Compound assignment operators
    std::cout << "\nCompound assignment operators:" << std::endl;
    int128 z {100};
    std::cout << "z = " << z << std::endl;

    z += 50;
    std::cout << "z += 50: " << z << std::endl;

    z -= 25;
    std::cout << "z -= 25: " << z << std::endl;

    z *= 2;
    std::cout << "z *= 2:  " << z << std::endl;

    z /= 5;
    std::cout << "z /= 5:  " << z << std::endl;

    z %= 7;
    std::cout << "z %= 7:  " << z << std::endl;

    // Unsigned 128-bit arithmetic - useful for very large positive values
    std::cout << "\n=== Unsigned 128-bit Arithmetic ===" << std::endl;
    uint128 large {UINT64_C(0x1), UINT64_C(0x0)};  // 2^64
    std::cout << "large (2^64) = " << large << std::endl;
    std::cout << "large * 2 = " << (large * 2U) << std::endl;
    std::cout << "large + large = " << (large + large) << std::endl;

    // Increment and decrement
    std::cout << "\nIncrement and Decrement:" << std::endl;
    int128 counter {10};
    std::cout << "counter = " << counter << std::endl;
    std::cout << "++counter = " << ++counter << std::endl;
    std::cout << "counter++ = " << counter++ << std::endl;
    std::cout << "counter = " << counter << std::endl;
    std::cout << "--counter = " << --counter << std::endl;

    return 0;
}