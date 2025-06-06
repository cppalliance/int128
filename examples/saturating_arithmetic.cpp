// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// Individual headers

#include <boost/int128/int128.hpp>
#include <boost/int128/numeric.hpp>

// Or you can do a single header

// #include <boost/int128.hpp>

#include <limits>
#include <type_traits>

#ifdef __clang__
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wunused-variable"
#endif

#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable:4146) // MSVC 14.1 only unary minus applied to unsigned type
#endif

int main()
{
    // std::numeric_limits is overloaded for both types
    constexpr auto uint_max {std::numeric_limits<boost::int128::uint128_t>::max()};
    static_assert(std::is_same<decltype(uint_max), const boost::int128::uint128_t>::value, "Types should match");

    constexpr boost::int128::int128_t int_max {std::numeric_limits<boost::int128::int128_t>::max()};

    // Saturating arithmetic returns max on overflow, or min on underflow rather than rolling over
    assert(boost::int128::add_sat(uint_max, uint_max) == uint_max);
    assert(boost::int128::sub_sat(boost::int128::uint128_t{0}, uint_max) == 0U);

    // This is especially useful for signed types since rollover is undefined
    assert(boost::int128::mul_sat(int_max, 2) == int_max);
    assert(boost::int128::mul_sat(-(int_max - 2), 5) == std::numeric_limits<boost::int128::int128_t>::min());

    // The only case in the library where div sat overflows is x = std::numeric_limits<int128_t>::min() and y = -1
    assert(boost::int128::div_sat(std::numeric_limits<boost::int128::int128_t>::min(), -1) == int_max);

    // Saturating case allows types to be safely converted without rollover behavior
    assert(boost::int128::saturate_cast<boost::int128::int128_t>(uint_max) == int_max);

    // You can also cast to builtin types
    assert(boost::int128::saturate_cast<std::int64_t>(int_max) == INT64_MAX);

    // Even of different signedness as this is treated like a static cast
    assert(boost::int128::saturate_cast<std::int32_t>(uint_max) == INT32_MAX);

    return 0;
}
