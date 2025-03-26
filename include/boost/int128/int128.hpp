// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_INT128_HPP
#define BOOST_INT128_INT128_HPP

#include <boost/int128/detail/config.hpp>
#include <cstdint>

namespace boost {
namespace int128 {

struct
    #ifdef BOOST_INT128_HAS_INT128
    alignas(alignof(__int128))
    #else
    alignas(16)
    #endif
int128_t
{
    #if BOOST_INT128_ENDIAN_LITTLE_BYTE
    std::uint64_t low {};
    std::int64_t high {};
    #else
    std::int64_t high {};
    std::uint64_t low {};
    #endif

    // Defaulted Constructors
    constexpr int128_t() noexcept = default;
    constexpr int128_t(const int128_t& other) noexcept = default;
    constexpr int128_t(int128_t&& other) noexcept = default;
    constexpr int128_t& operator=(const int128_t& other) noexcept = default;
    constexpr int128_t& operator=(int128_t&& other) noexcept = default;
    constexpr ~int128_t() noexcept = default;

    // Direct construction of the number
    constexpr int128_t(const std::int64_t hi, const std::uint64_t lo) noexcept : low{lo}, high{hi} {}

};

} // namespace int128
} // namespace boost

#endif //BOOST_INT128_INT128_HPP
