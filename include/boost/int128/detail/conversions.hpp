// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_CONVERSIONS_HPP
#define BOOST_INT128_CONVERSIONS_HPP

#include <boost/int128/detail/int128_imp.hpp>
#include <boost/int128/detail/uint128_imp.hpp>

namespace boost {
namespace int128 {

constexpr int128_t::int128_t(const uint128_t& v) noexcept : low {v.low}, high {static_cast<std::int64_t>(v.high)} {}

} // namespace int128
} // namespace boost

#endif // BOOST_INT128_CONVERSIONS_HPP
