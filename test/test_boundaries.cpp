//  Copyright 2026 Matt Borland
//  Distributed under the Boost Software License, Version 1.0.
//  https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128.hpp>
#include <boost/core/lightweight_test.hpp>
#include <cstdint>
#include <limits>

using boost::int128::int128_t;

namespace
{

std::int64_t opaque(std::int64_t v)
{
    volatile std::int64_t s {v};
    return s;
}

std::uint64_t opaque(std::uint64_t v)
{
    volatile std::uint64_t s {v};
    return s;
}

int128_t opaque(int128_t v)
{
    return int128_t{opaque(v.signed_high()), opaque(v.low)};
}

} // namespace

int main()
{
    const auto int_max {opaque(BOOST_INT128_INT128_MAX)};
    const auto int_min {opaque(BOOST_INT128_INT128_MIN)};
    const int128_t one {opaque(int128_t{0, 1})};

    // Negating a signed scalar of INT64_MIN must not overflow the scalar type.
    // The scalar-operand result must equal promoting the scalar to int128_t first.
    const std::int64_t m {opaque((std::numeric_limits<std::int64_t>::min)())};
    const int128_t m128 {m};
    const int128_t a {opaque(int128_t{42, 1234})};
    const int128_t b {opaque(int128_t{0, 5})};

    BOOST_TEST_EQ(a + m, a + m128);   // operator+(int128_t, Signed)   was int128_imp.hpp:1889
    BOOST_TEST_EQ(m + a, m128 + a);   // operator+(Signed, int128_t)   was int128_imp.hpp:1895
    BOOST_TEST_EQ(a / m, a / m128);   // operator/(int128_t, Signed)   was int128_imp.hpp:2295
    BOOST_TEST_EQ(m / b, m128 / b);   // operator/(Signed, int128_t)   was int128_imp.hpp:2324

    // A 128-bit divisor of exactly -2^64 has abs().low == 0; the quotient of any
    // 64-bit numerator by it is 0 and must not divide by zero.
    const int128_t neg_two_pow_64 {opaque(int128_t{-1, 0})};
    BOOST_TEST_EQ(opaque(std::int64_t{5})  / neg_two_pow_64, (int128_t{0, 0}));  // operator/(Signed, int128_t)
    BOOST_TEST_EQ(opaque(std::int64_t{-5}) / neg_two_pow_64, (int128_t{0, 0}));
    BOOST_TEST_EQ(opaque(std::uint64_t{5}) / neg_two_pow_64, (int128_t{0, 0}));  // operator/(Unsigned, int128_t)

    // Two's-complement rollover at the range boundaries: no UB, wraps like __int128.
    BOOST_TEST_EQ(int_max + one, int_min);                       // MAX + 1 -> MIN
    BOOST_TEST_EQ(int_min - one, int_max);                       // MIN - 1 -> MAX
    BOOST_TEST_EQ(int_max + opaque(std::int64_t{1}), int_min);   // MAX + 1 (scalar) -> MIN
    BOOST_TEST_EQ(int_min - opaque(std::int64_t{1}), int_max);   // MIN - 1 (scalar) -> MAX
    BOOST_TEST_EQ(-int_min, int_min);                            // -MIN wraps to itself
    BOOST_TEST_EQ(int_min * opaque(std::int64_t{-1}), int_min);  // MIN * -1 overflows to MIN
    BOOST_TEST_EQ(int_max * opaque(std::int64_t{2}), (-int128_t{0, 2}));  // MAX * 2 overflows to -2

    return boost::report_errors();
}
