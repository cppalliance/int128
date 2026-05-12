// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_HASH_HPP
#define BOOST_INT128_HASH_HPP

#include <boost/int128/int128.hpp>

#ifndef BOOST_INT128_BUILD_MODULE

#include <cstddef>
#include <cstdint>
#include <functional>

#endif

namespace std {

template <>
struct hash<boost::int128::int128_t>
{
    auto operator()(const boost::int128::int128_t v) const noexcept -> std::size_t
    {
        const std::size_t low_hash {std::hash<std::uint64_t>{}(v.low)};
        const std::size_t high_hash {std::hash<std::int64_t>{}(v.high)};

        // boost::hash_combine style mixing
        return low_hash ^ (high_hash + static_cast<std::size_t>(0x9e3779b9) + (low_hash << 6) + (low_hash >> 2));
    }
};

template <>
struct hash<boost::int128::uint128_t>
{
    auto operator()(const boost::int128::uint128_t v) const noexcept -> std::size_t
    {
        const std::size_t low_hash {std::hash<std::uint64_t>{}(v.low)};
        const std::size_t high_hash {std::hash<std::uint64_t>{}(v.high)};

        // boost::hash_combine style mixing
        return low_hash ^ (high_hash + static_cast<std::size_t>(0x9e3779b9) + (low_hash << 6) + (low_hash >> 2));
    }
};

} // namespace std

#endif // BOOST_INT128_HASH_HPP
