// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_DETAIL_CONFIG_HPP
#define BOOST_INT128_DETAIL_CONFIG_HPP

// Use 128-bit integers
#if defined(BOOST_HAS_INT128) || (defined(__SIZEOF_INT128__) && !defined(_MSC_VER))

#define BOOST_INT128_HAS_INT128

namespace boost {
namespace int128 {
namespace detail {

// Avoids pedantic warnings
#ifdef __GNUC__

__extension__ typedef __int128 builtin_i128;
__extension__ typedef unsigned __int128 builtin_u128;

#else

using builtin_i128 = __int128 ;
using builtin_u128 = unsigned __int128;

#endif

} // namespace detail
} // namespace int128
} // namespace boost

#endif

// Determine endianness
#if defined(_WIN32)

#define BOOST_INT128_ENDIAN_BIG_BYTE 0
#define BOOST_INT128_ENDIAN_LITTLE_BYTE 1

#elif defined(__BYTE_ORDER__)

#define BOOST_INT128_ENDIAN_BIG_BYTE (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#define BOOST_INT128_ENDIAN_LITTLE_BYTE (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)

#else

#error Could not determine endian type. Please file an issue at https://github.com/cppalliance/decimal with your architecture

#endif // Determine endianness

// Determine if we can use concepts
#if (__cplusplus >= 202002L || (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L)) &&\
    (!defined(__GNUC__) || __GNUC__ != 11) && __has_include(<concepts>)

#define BOOST_INT128_HAS_CONCEPTS

#endif

#endif // BOOST_INT128_DETAIL_CONFIG_HPP
