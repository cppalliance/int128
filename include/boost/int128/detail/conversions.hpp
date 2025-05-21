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

constexpr uint128_t::operator int128_t() const noexcept
{
    return {static_cast<std::int64_t>(this->high), this->low};
}

} // namespace int128
} // namespace boost

#endif // BOOST_INT128_CONVERSIONS_HPP
