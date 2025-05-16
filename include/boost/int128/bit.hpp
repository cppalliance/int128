// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_BIT_HPP
#define BOOST_INT128_BIT_HPP

#include <boost/int128/int128.hpp>

namespace boost {
namespace int128 {

constexpr bool has_single_bit(const uint128_t x) noexcept
{
    return x && !(x & (x - 1));
}

} // namespace int128
} // namespace boost

#endif // BOOST_INT128_BIT_HPP
