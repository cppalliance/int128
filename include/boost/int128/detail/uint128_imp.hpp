// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_DETAIL_UINT128_HPP
#define BOOST_INT128_DETAIL_UINT128_HPP

#include <boost/int128/detail/config.hpp>
#include <cstdint>

namespace boost {
namespace int128 {

struct
    #ifdef BOOST_INT128_HAS_INT128
    alignas(alignof(detail::builtin_u128))
    #else
    alignas(16)
    #endif
uint128_t
{
    #if BOOST_INT128_ENDIAN_LITTLE_BYTE
    std::uint64_t low {};
    std::int64_t high {};
    #else
    std::int64_t high {};
    std::uint64_t low {};
    #endif
};

} // namespace int128
} // namespace boost

#endif //BOOST_INT128_DETAIL_UINT128_HPP
