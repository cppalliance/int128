//  Copyright 2026 Matt Borland
//  Distributed under the Boost Software License, Version 1.0.
//  https://www.boost.org/LICENSE_1_0.txt

// Compile-time counterpart of test_boundaries.cpp. Every operation is evaluated
// in a constant expression. Undefined behavior is not permitted in a constant
// expression, so any UB here is a compilation error, and the equalities are
// verified at compile time as well.

#include <boost/int128.hpp>
#include <cstdint>
#include <limits>

#ifdef _MSC_VER
#  pragma warning(disable : 4307) // integral constant overflow
#  pragma warning(disable : 4146) // unary minus operator applied to unsigned type, result still unsigned
#endif

using boost::int128::int128_t;

namespace
{

constexpr auto int_max {BOOST_INT128_INT128_MAX};
constexpr auto int_min {BOOST_INT128_INT128_MIN};
constexpr int128_t one {0, 1};

constexpr std::int64_t m {(std::numeric_limits<std::int64_t>::min)()};
constexpr int128_t m128 {m};
constexpr int128_t a {42, 1234};
constexpr int128_t b {0, 5};
constexpr int128_t neg_two_pow_64 {-1, 0};

// Signed-scalar negation paths (INT64_MIN) must be UB-free and must agree with
// promoting the scalar to int128_t first.
static_assert(a + m == a + m128, "operator+(int128_t, Signed)");
static_assert(m + a == m128 + a, "operator+(Signed, int128_t)");
static_assert(a / m == a / m128, "operator/(int128_t, Signed)");
static_assert(m / b == m128 / b, "operator/(Signed, int128_t)");

// A 128-bit divisor of exactly -2^64 has abs().low == 0; the quotient is 0 and
// must not divide by zero.
static_assert(std::int64_t{5}  / neg_two_pow_64 == int128_t{0, 0}, "operator/(Signed, int128_t) -2^64");
static_assert(std::int64_t{-5} / neg_two_pow_64 == int128_t{0, 0}, "operator/(Signed, int128_t) -2^64 negative");
static_assert(std::uint64_t{5} / neg_two_pow_64 == int128_t{0, 0}, "operator/(Unsigned, int128_t) -2^64");

// Two's-complement rollover at the range boundaries: no UB, wraps like __int128.
static_assert(int_max + one == int_min, "MAX + 1 -> MIN");
static_assert(int_min - one == int_max, "MIN - 1 -> MAX");
static_assert(int_max + 1   == int_min, "MAX + 1 (scalar) -> MIN");
static_assert(int_min - 1   == int_max, "MIN - 1 (scalar) -> MAX");
static_assert(-int_min      == int_min, "-MIN wraps to itself");
static_assert(int_min * -1  == int_min, "MIN * -1 overflows to MIN");
static_assert(int_max * 2   == -int128_t{0, 2}, "MAX * 2 overflows to -2");

} // namespace

int main()
{
    return 0;
}
