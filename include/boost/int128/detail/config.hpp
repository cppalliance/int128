// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_DETAIL_CONFIG_HPP
#define BOOST_INT128_DETAIL_CONFIG_HPP

// Use 128-bit integers
#if defined(BOOST_HAS_INT128) || (defined(__SIZEOF_INT128__) && !defined(_MSC_VER))
#define BOOST_INT128_HAS_INT128
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

#endif // BOOST_INT128_DETAIL_CONFIG_HPP
