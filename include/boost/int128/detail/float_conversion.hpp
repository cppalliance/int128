// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_DETAIL_FLOAT_CONVERSION_HPP
#define BOOST_INT128_DETAIL_FLOAT_CONVERSION_HPP

#include <boost/int128/detail/config.hpp>
#include <boost/int128/detail/constants.hpp>

#ifndef BOOST_INT128_BUILD_MODULE

#include <cstdint>

#endif

namespace boost {
namespace int128 {
namespace detail {

// The most correct way to do this would be std::ldexp(static_cast<T>(high), 64) + static_cast<T>(low);
// Since std::ldexp is not constexpr until C++23 we can work around this by multiplying the high word
// by 0xFFFFFFFF in order to generally replicate what ldexp is doing in the constexpr context.
// We also avoid pulling in <quadmath.h> for the __float128 case where we would need ldexpq
template <typename T>
BOOST_INT128_HOST_DEVICE constexpr T unsigned_words_to_float(const std::uint64_t high, const std::uint64_t low) noexcept
{
    return static_cast<T>(high) * offset_value_v<T> + static_cast<T>(low);
}

template <typename T>
BOOST_INT128_HOST_DEVICE constexpr T signed_words_to_float(const std::int64_t high, const std::uint64_t low) noexcept
{
    if (high < 0)
    {
        // Two's complement magnitude of the full 128-bit value.
        // INT128_MIN needs no special case since it yields high = 2^63, low = 0
        const auto abs_low {~low + UINT64_C(1)};
        const auto abs_high {~static_cast<std::uint64_t>(high) + (abs_low == UINT64_C(0) ? UINT64_C(1) : UINT64_C(0))};

        return -unsigned_words_to_float<T>(abs_high, abs_low);
    }

    return unsigned_words_to_float<T>(static_cast<std::uint64_t>(high), low);
}

} // namespace detail
} // namespace int128
} // namespace boost

#endif // BOOST_INT128_DETAIL_FLOAT_CONVERSION_HPP
