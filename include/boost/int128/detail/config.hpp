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

__extension__ using builtin_i128 = __int128 ;
__extension__ using builtin_u128 = unsigned __int128 ;

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

// Is constant evaluated detection
#ifdef __cpp_lib_is_constant_evaluated
#  define BOOST_INT128_HAS_IS_CONSTANT_EVALUATED
#endif

#ifdef __has_builtin
#  if __has_builtin(__builtin_is_constant_evaluated)
#    define BOOST_INT128_HAS_BUILTIN_IS_CONSTANT_EVALUATED
#  endif
#endif

//
// MSVC also supports __builtin_is_constant_evaluated if it's recent enough:
//
#if defined(_MSC_FULL_VER) && (_MSC_FULL_VER >= 192528326)
#  define BOOST_INT128_HAS_BUILTIN_IS_CONSTANT_EVALUATED
#endif

//
// As does GCC-9:
//
#if defined(__GNUC__) && (__GNUC__ >= 9) && !defined(BOOST_INT128_HAS_BUILTIN_IS_CONSTANT_EVALUATED)
#  define BOOST_INT128_HAS_BUILTIN_IS_CONSTANT_EVALUATED
#endif

#if defined(BOOST_INT128_HAS_IS_CONSTANT_EVALUATED)
#  define BOOST_INT128_IS_CONSTANT_EVALUATED(x) std::is_constant_evaluated()
#elif defined(BOOST_INT128_HAS_BUILTIN_IS_CONSTANT_EVALUATED)
#  define BOOST_INT128_IS_CONSTANT_EVALUATED(x) __builtin_is_constant_evaluated()
#else
#  define BOOST_INT128_IS_CONSTANT_EVALUATED(x) false
#  define BOOST_INT128_NO_CONSTEVAL_DETECTION
#endif

// https://github.com/llvm/llvm-project/issues/55638
#if defined(__clang__) && __cplusplus > 202002L && __clang_major__ < 17
#  undef BOOST_INT128_IS_CONSTANT_EVALUATED
#  define BOOST_INT128_IS_CONSTANT_EVALUATED(x) false
#  define BOOST_INT128_NO_CONSTEVAL_DETECTION
#endif

#if defined(_MSC_VER)
#  define BOOST_INT128_FORCE_INLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#  define BOOST_INT128_FORCE_INLINE __attribute__((always_inline)) inline
#else
#  define BOOST_INT128_FORCE_INLINE inline
#endif

#ifdef __x86_64__

#  include <x86intrin.h>
#  include <emmintrin.h>

#  ifdef __ADX__
#    define BOOST_INT128_ADD_CARRY _addcarryx_u64
#    define BOOST_INT128_SUB_BORROW _subborrow_u64
#  else
#    define BOOST_INT128_ADD_CARRY _addcarry_u64
#    define BOOST_INT128_SUB_BORROW _subborrow_u64
#  endif

#elif defined(_M_AMD64)

#  include <intrin.h>

#  define BOOST_INT128_ADD_CARRY _addcarryx_u64
#  define BOOST_INT128_SUB_BORROW _subborrow_u64

#elif defined(__i386__)

#  include <emmintrin.h>

#elif defined(_M_IX86)

#  include <intrin.h>

#endif // Platform macros

// The builtin is only constexpr from clang-7 or GCC-10
#ifdef __has_builtin
#  if __has_builtin(__builtin_sub_overflow) && ((defined(__clang__) && __clang_major__ >= 7) || (defined(__GNUC__) && __GNUC__ >= 10))
#    define BOOST_INT128_HAS_BUILTIN_SUB_OVERFLOW
#  endif
#  if __has_builtin(__builtin_add_overflow) && ((defined(__clang__) && __clang_major__ >= 7) || (defined(__GNUC__) && __GNUC__ >= 10))
#    define BOOST_INT128_HAS_BUILTIN_ADD_OVERFLOW
#  endif
#endif

#if defined(__cpp_if_constexpr) && __cpp_if_constexpr >= 201606L
#  define BOOST_INT128_HAS_IF_CONSTEXPR
#endif // if constexpr detection

#endif // BOOST_INT128_DETAIL_CONFIG_HPP
