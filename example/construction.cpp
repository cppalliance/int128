// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128/int128.hpp>
#include <boost/int128/literals.hpp>
#include <boost/int128/iostream.hpp>
#include <iostream>
#include <limits>
#include <sstream>

int main()
{
    using boost::int128::uint128;
    using boost::int128::int128;

    std::cout << "=== uint128 Construction ===" << std::endl;

    // 1) From a builtin integer type
    constexpr uint128 from_builtin {42U};
    std::cout << "From builtin (42U): " << from_builtin << std::endl;

    // 2) From high and low 64-bit values (high, low)
    constexpr uint128 from_parts {UINT64_C(0x1), UINT64_C(0x0)};  // 2^64
    std::cout << "From parts (1, 0) = 2^64: " << from_parts << std::endl;

    constexpr uint128 max_value {UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF)};
    std::cout << "From parts (max, max): " << max_value << std::endl;
    std::cout << "  Equals numeric_limits max? " << std::boolalpha
              << (max_value == std::numeric_limits<uint128>::max()) << std::endl;

    // 3) From user-defined literals.
    // The library provides only string-form UDLs
    // For small values like this a string is still parsed rather than direct construction
    // Using the constructors for values that fit in (unsigned) long long should be preferred for performance
    using namespace boost::int128::literals;
    const auto small_literal {12345_U128};
    std::cout << "From literal 12345_U128: " << small_literal << std::endl;

    // 4) From macro (like UINT64_C but for 128-bit), good for values that exceed unsigned long long
    const auto from_macro {BOOST_INT128_UINT128_C(340282366920938463463374607431768211455)};
    std::cout << "From BOOST_INT128_UINT128_C(max): " << from_macro << std::endl;

    // 5) From input stream
    std::stringstream ss;
    ss.str("12345678901234567890123456789");
    uint128 from_stream;
    ss >> from_stream;
    std::cout << "From stringstream: " << from_stream << std::endl;

    std::cout << "\n=== int128 Construction ===" << std::endl;

    // Signed from builtin
    constexpr int128 signed_builtin {-42};
    std::cout << "From builtin (-42): " << signed_builtin << std::endl;

    // Signed from parts. Both words are stored unsigned, but the constructor takes
    // the high word signed; read it back with signed_high().
    constexpr int128 min_value {INT64_MIN, 0};
    std::cout << "From parts (INT64_MIN, 0): " << min_value << std::endl;
    std::cout << "  Equals numeric_limits min? "
              << (min_value == std::numeric_limits<int128>::min()) << std::endl;

    // Signed literals. Values that fit in unsigned long long can be written
    // directly; the leading minus is parsed as a unary operator on the
    // literal result (lowercase and uppercase suffixes both work):
    const auto negative_literal {-12345_i128};
    std::cout << "From literal -12345_i128: " << negative_literal << std::endl;

    const auto positive_literal {12345_I128};
    std::cout << "From literal 12345_I128: " << positive_literal << std::endl;

    // For magnitudes beyond unsigned long long you can use the macro or a string literal
    const auto large_signed {BOOST_INT128_INT128_C(-99999999999999999999)};
    std::cout << "From BOOST_INT128_INT128_C(-99999999999999999999): " << large_signed << std::endl;

    const auto large_signed_string {"-99999999999999999999"_i128};
    std::cout << "From string literal: " << large_signed_string << std::endl;

    // Signed macro
    const auto from_signed_macro {BOOST_INT128_INT128_C(-170141183460469231731687303715884105728)};
    std::cout << "From BOOST_INT128_INT128_C(min): " << from_signed_macro << std::endl;

    std::cout << "\n=== Default and Copy Construction ===" << std::endl;

    // Default construction (zero-initialized)
    constexpr uint128 default_constructed {};
    std::cout << "Default constructed: " << default_constructed << std::endl;

    // Copy construction
    const uint128 copied {from_macro};
    std::cout << "Copy constructed: " << copied << std::endl;

    std::cout << "\n=== Floating-Point Construction ===" << std::endl;

    // Floating-point construction truncates toward zero, matching the behavior of
    // a static_cast from a floating-point type to a built-in integer.
    constexpr uint128 from_double {12345.9};
    std::cout << "uint128 from 12345.9 (truncated): " << from_double << std::endl;

    constexpr int128 from_negative_double {-12345.9};
    std::cout << "int128 from -12345.9 (truncated toward zero): " << from_negative_double << std::endl;

    // Values that exceed the 64-bit range are routed through the full 128-bit decomposition.
    const double two_to_the_100 {1.2676506002282294e30};  // 2^100
    const uint128 large_from_double {two_to_the_100};
    std::cout << "uint128 from 2^100: " << large_from_double << std::endl;

    std::cout << "\n=== Floating-Point Edge Cases ===" << std::endl;

    // NaN yields zero for both signed and unsigned (mirrors libgcc's __fix(uns)Xfti).
    const double nan_value {std::numeric_limits<double>::quiet_NaN()};
    const uint128 unsigned_from_nan {nan_value};
    const int128 signed_from_nan {nan_value};
    std::cout << "uint128 from NaN: " << unsigned_from_nan << std::endl;
    std::cout << "int128 from NaN: " << signed_from_nan << std::endl;

    // Negative values are clamped to zero when constructing uint128.
    const uint128 unsigned_from_negative {-1.0};
    std::cout << "uint128 from -1.0 (clamped to zero): " << unsigned_from_negative << std::endl;

    // Positive overflow saturates: anything >= 2^128 (including +infinity) becomes UINT128_MAX.
    const double infinity {std::numeric_limits<double>::infinity()};
    const uint128 saturated_unsigned {infinity};
    std::cout << "uint128 from +infinity (saturates to UINT128_MAX): " << saturated_unsigned << std::endl;

    // For int128, values >= 2^127 saturate to INT128_MAX and values <= -2^127 saturate to INT128_MIN.
    const double huge {1e40};  // Well beyond 2^127 (~ 1.7e38)
    const int128 saturated_positive {huge};
    const int128 saturated_negative {-huge};
    std::cout << "int128 from 1e40 (saturates to INT128_MAX): " << saturated_positive << std::endl;
    std::cout << "int128 from -1e40 (saturates to INT128_MIN): " << saturated_negative << std::endl;

    return 0;
}