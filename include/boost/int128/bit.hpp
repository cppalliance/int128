// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_BIT_HPP
#define BOOST_INT128_BIT_HPP

#include <boost/int128/int128.hpp>
#include <boost/int128/detail/config.hpp>
#include <boost/int128/detail/clz.hpp>
#include <boost/int128/detail/ctz.hpp>

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

constexpr int countl_one(const uint128_t x) noexcept
{
    return countl_zero(~x);
}

constexpr int bit_width(const uint128_t x) noexcept
{
    return x ? 128 - countl_zero(x) : 0;
}

constexpr uint128_t bit_ceil(const uint128_t x) noexcept
{
    return x <= 1 ? static_cast<uint128_t>(1) : static_cast<uint128_t>(1) << bit_width(x - 1);
}

constexpr uint128_t bit_floor(const uint128_t x) noexcept
{
    return x >= 0 ? static_cast<uint128_t>(1) << (bit_width(x) - 1) : static_cast<uint128_t>(0);
}

constexpr int countr_zero(const uint128_t x) noexcept
{
    return x.low == 0 ? 64 + detail::countr_zero(x.high) : detail::countr_zero(x.low);
}

constexpr int countr_one(const uint128_t x) noexcept
{
    return countr_zero(~x);
}

} // namespace int128
} // namespace boost

#endif // BOOST_INT128_BIT_HPP
