// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_DETAIL_INT128_HPP
#define BOOST_INT128_DETAIL_INT128_HPP

#include <boost/int128/detail/fwd.hpp>
#include <boost/int128/detail/config.hpp>
#include <boost/int128/detail/traits.hpp>
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
    #ifdef BOOST_INT128_ENDIAN_LITTLE_BYTE
    std::uint64_t low {};
    std::int64_t high {};
    #else
    std::int64_t high {};
    std::uint64_t low {};
    #endif

    // Defaulted basic construction
    constexpr int128_t() noexcept = default;
    constexpr int128_t(const int128_t&) noexcept = default;
    constexpr int128_t(int128_t&&) noexcept = default;
    constexpr int128_t& operator=(const int128_t&) noexcept = default;
    constexpr int128_t& operator=(int128_t&&) noexcept = default;

    // Requires conversion file to be implemented
    constexpr int128_t(const uint128_t& v) noexcept;

    // Construct from integral types
    #if BOOST_INT128_ENDIAN_LITTLE_BYTE

    template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
    constexpr int128_t(const SignedInteger v) noexcept : low {static_cast<std::uint64_t>(v)}, high {v < 0 ? -1 : 0} {}

    template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
    constexpr int128_t(const UnsignedInteger v) noexcept : low {static_cast<std::uint64_t>(v)}, high {} {}

    #ifdef BOOST_INT128_HAS_INT128

    constexpr int128_t(const __int128 v) noexcept : low {static_cast<std::uint64_t>(v & 0xFFFFFFFFFFFFFFFFULL)}, high {static_cast<std::int64_t>(v >> 64U)} {}
    constexpr int128_t(const unsigned __int128 v) noexcept : low {static_cast<std::uint64_t>(v & 0xFFFFFFFFFFFFFFFFULL)}, high {static_cast<std::int64_t>(v >> 64U)} {}

    #endif // BOOST_INT128_HAS_INT128

    #else // Big endian

    template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
    constexpr int128_t(const SignedInteger v) noexcept : high{v < 0 ? -1 : 0}, low{static_cast<std::uint64_t>(v)} {}

    template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
    constexpr int128_t(const UnsignedInteger v) noexcept : high {}, low {static_cast<std::uint64_t>(v)} {}

    #ifdef BOOST_INT128_HAS_INT128

    constexpr int128_t(const __int128 v) noexcept : high {static_cast<std::int64_t>(v >> 64U)}, low {static_cast<std::uint64_t>(v & 0xFFFFFFFFFFFFFFFFULL)} {}
    constexpr int128_t(const unsigned __int128 v) noexcept : high {static_cast<std::int64_t>(v >> 64U)}, low {static_cast<std::uint64_t>(v & 0xFFFFFFFFFFFFFFFFULL)} {}

    #endif // BOOST_INT128_HAS_INT128

    #endif // BOOST_INT128_ENDIAN_LITTLE_BYTE
};

} // namespace int128
} // namespace boost

#endif // BOOST_INT128_DETAIL_INT128_HPP
