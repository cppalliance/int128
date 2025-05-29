// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_NUMERIC_HPP
#define BOOST_INT128_NUMERIC_HPP

#include <boost/int128/detail/fwd.hpp>
#include <limits>

namespace boost {
namespace int128 {

constexpr uint128_t add_sat(const uint128_t x, const uint128_t y) noexcept
{
    const auto z {x + y};

    if (z < x)
    {
        return std::numeric_limits<uint128_t>::max();
    }

    return z;
}

constexpr uint128_t sub_sat(const uint128_t x, const uint128_t y) noexcept
{
    const auto z {x - y};

    if (z > x)
    {
        return std::numeric_limits<uint128_t>::min();
    }

    return z;
}

} // namespace int128
} // namespace boost

#endif // BOOST_INT128_NUMERIC_HPP
