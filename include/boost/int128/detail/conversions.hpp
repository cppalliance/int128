// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_CONVERSIONS_HPP
#define BOOST_INT128_CONVERSIONS_HPP

#include <boost/int128/detail/int128_imp.hpp>
#include <boost/int128/detail/uint128_imp.hpp>

namespace boost {
namespace int128 {

#if BOOST_INT128_ENDIAN_LITTLE_BYTE

constexpr int128_t::int128_t(const uint128_t& v) noexcept : low {v.low}, high {static_cast<std::int64_t>(v.high)} {}

constexpr uint128_t::uint128_t(const int128_t& v) noexcept : low {v.low}, high {static_cast<std::uint64_t>(v.high)} {}

#else

constexpr int128_t::int128_t(const uint128_t& v) noexcept : high {static_cast<std::int64_t>(v.high)}, low {v.low} {}

constexpr uint128_t::uint128_t(const int128_t& v) noexcept : high {static_cast<std::uint64_t>(v.high)}, low {v.low} {}

#endif // BOOST_INT128_ENDIAN_LITTLE_BYTE

//=====================================
// Conversion Operators
//=====================================

constexpr uint128_t::operator int128_t() const noexcept
{
    return {static_cast<std::int64_t>(this->high), this->low};
}

constexpr int128_t::operator uint128_t() const noexcept
{
    return {static_cast<std::uint64_t>(this->high), this->low};
}

//=====================================
// Comparison Operators
//=====================================

constexpr bool operator==(const int128_t& lhs, const uint128_t& rhs) noexcept
{
    return lhs < 0 ? false : lhs.low == rhs.low && static_cast<std::uint64_t>(lhs.high) == rhs.high;
}

constexpr bool operator==(const uint128_t& lhs, const int128_t& rhs) noexcept
{
    return rhs < 0 ? false : lhs.low == rhs.low && static_cast<std::uint64_t>(lhs.high) == rhs.high;
}

constexpr bool operator!=(const int128_t& lhs, const uint128_t& rhs) noexcept
{
    return !(lhs == rhs);
}

constexpr bool operator!=(const uint128_t& lhs, const int128_t& rhs) noexcept
{
    return !(lhs == rhs);
}

} // namespace int128
} // namespace boost

#endif // BOOST_INT128_CONVERSIONS_HPP
