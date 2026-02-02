// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

module;

#include <limits>
#include <iostream>
#include <type_traits>
#include <iomanip>
#include <cstring>
#include <cstdint>
#include <cassert>
#include <cerrno>
#include <cstddef>
#include <compare>

#if __has_include(<__msvc_int128.hpp>) && _MSVC_LANG >= 202002L

#include <__msvc_int128.hpp>

#endif

#ifdef __x86_64__
#  include <x86intrin.h>
#  include <emmintrin.h>
#elif defined(_M_AMD64)
#  include <intrin.h>
#elif defined(__i386__)
#  include <emmintrin.h>
#elif defined(_M_IX86)
#  include <intrin.h>
#endif

#define BOOST_INT128_BUILD_MODULE

export module boost.int128;

export namespace boost::int128 {

struct int128_t;
struct uint128_t;

} // namespace boost::int128

export namespace std {

template <>
class numeric_limits<boost::int128::int128_t>;

template <>
class numeric_limits<boost::int128::uint128_t>;

} // namespace std

#ifdef _MSC_VER
#  pragma warning( push )
#  pragma warning( disable : 5244 )
#elif defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Winclude-angled-in-module-purview"
#endif

#include <boost/int128.hpp>

#ifdef _MSC_VER
#  pragma warning( pop )
#elif defined(__clang__)
#  pragma clang diagnostic pop
#endif

