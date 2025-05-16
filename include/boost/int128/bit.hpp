// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_BIT_HPP
#define BOOST_INT128_BIT_HPP

#include <boost/int128/int128.hpp>
#include <boost/int128/detail/config.hpp>
#include <boost/int128/detail/clz.hpp>

namespace boost {
namespace int128 {

constexpr bool has_single_bit(const uint128_t x) noexcept
{
    return x && !(x & (x - 1));
}

constexpr int countl_zero(const uint128_t x) noexcept
{
    return x.high == 0 ? 64 + detail::countl_zero(x.low) : detail::countl_zero(x.high);
}

constexpr int bit_width(const uint128_t x) noexcept
{
    return x ? 128 - countl_zero(x) : 0;
}

} // namespace int128
} // namespace boost

#endif // BOOST_INT128_BIT_HPP
