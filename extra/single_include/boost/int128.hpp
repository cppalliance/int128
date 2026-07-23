// boost.int128 - amalgamated single-header build.
// Auto-generated: all internal boost/int128 headers inlined into one file
// for use in Compiler Explorer and other single-file environments.
// Do not edit by hand; regenerate with extra/amalgamate.py.
//
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// ===== BEGIN boost/int128.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_HPP
#define BOOST_INT128_HPP

// ===== BEGIN boost/int128/int128.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_INT128_HPP
#define BOOST_INT128_INT128_HPP

// ===== BEGIN boost/int128/detail/fwd.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_DETAIL_FWD_HPP
#define BOOST_INT128_DETAIL_FWD_HPP

// ===== BEGIN boost/int128/detail/config.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_DETAIL_CONFIG_HPP
#define BOOST_INT128_DETAIL_CONFIG_HPP

// A handful of detail-namespace entities are exercised directly by the module
// test suite. BOOST_INT128_TEST_EXPORT exports them only when the module is built
// for testing (BOOST_INT128_EXPORT_TESTING), so the normal module API stays limited
// to the public interface. It expands to nothing in ordinary (header) builds.
#if defined(BOOST_INT128_BUILD_MODULE) && defined(BOOST_INT128_EXPORT_TESTING)
#  define BOOST_INT128_TEST_EXPORT export
#else
#  define BOOST_INT128_TEST_EXPORT
#endif

// The SYCL device target (spir64) has no native 128-bit integer, so force the portable
// code path on the device pass. This mirrors a user-supplied BOOST_INT128_NO_BUILTIN_INT128
// and keeps host/device selection consistent even though __x86_64__ stays defined on device.
#if defined(__SYCL_DEVICE_ONLY__) && !defined(BOOST_INT128_NO_BUILTIN_INT128)
#  define BOOST_INT128_NO_BUILTIN_INT128
#endif

// Use 128-bit integers.
// The SYCL device target (spir64) has no native 128-bit integer, so on the device pass
// we fall back to the portable path (the same one used on platforms without __int128).
#if (defined(BOOST_HAS_INT128) || (defined(__SIZEOF_INT128__) && !defined(_MSC_VER)) && !defined(BOOST_INT128_NO_BUILTIN_INT128)) && !defined(__SYCL_DEVICE_ONLY__)

#define BOOST_INT128_HAS_INT128

#define BOOST_INT128_BUILTIN_CONSTEXPR constexpr

namespace boost {
namespace int128 {
namespace detail {

// A module consumer receives these aliases from the import, so only declare them
// in ordinary builds and in the module interface unit itself; declaring them again
// in a consumer would give a second, distinct type and break overload resolution.
#if !defined(BOOST_INT128_BUILD_MODULE) || defined(BOOST_INT128_INTERFACE_UNIT)

// Avoids pedantic warnings
#ifdef __GNUC__

BOOST_INT128_TEST_EXPORT __extension__ using builtin_i128 = __int128 ;
BOOST_INT128_TEST_EXPORT __extension__ using builtin_u128 = unsigned __int128 ;

#else

BOOST_INT128_TEST_EXPORT using builtin_i128 = __int128 ;
BOOST_INT128_TEST_EXPORT using builtin_u128 = unsigned __int128;

#endif

#endif // declare builtin aliases

} // namespace detail
} // namespace int128
} // namespace boost

#elif __has_include(<__msvc_int128.hpp>) && _MSVC_LANG >= 202002L && !defined(__SYCL_DEVICE_ONLY__)

#ifndef BOOST_INT128_BUILD_MODULE
#include <__msvc_int128.hpp>
#endif

#define BOOST_INT128_HAS_MSVC_INT128

#if _MSC_VER >= 1945
#define BOOST_INT128_BUILTIN_CONSTEXPR constexpr
#else
#define BOOST_INT128_BUILTIN_CONSTEXPR inline
#endif

namespace boost {
namespace int128 {
namespace detail {

// See the note above: skip the re-declaration in a module consumer.
#if !defined(BOOST_INT128_BUILD_MODULE) || defined(BOOST_INT128_INTERFACE_UNIT)

BOOST_INT128_TEST_EXPORT using builtin_i128 = std::_Signed128;
BOOST_INT128_TEST_EXPORT using builtin_u128 = std::_Unsigned128;

#endif

} // namespace detail
} // namespace int128
} // namespace boost

#endif // builtin 128-bit detection

// Determine endianness
#if defined(_WIN32)

#define BOOST_INT128_ENDIAN_BIG_BYTE 0
#define BOOST_INT128_ENDIAN_LITTLE_BYTE 1

#elif defined(__BYTE_ORDER__)

#define BOOST_INT128_ENDIAN_BIG_BYTE (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#define BOOST_INT128_ENDIAN_LITTLE_BYTE (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)

#else

#error Could not determine endian type. Please file an issue at https://github.com/cppalliance/INT128 with your architecture

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

#if defined(BOOST_INT128_HAS_IS_CONSTANT_EVALUATED) && !(defined(__CUDACC__) && defined(BOOST_INT128_ENABLE_CUDA))
#  define BOOST_INT128_IS_CONSTANT_EVALUATED(x) std::is_constant_evaluated()
#elif defined(BOOST_INT128_HAS_BUILTIN_IS_CONSTANT_EVALUATED) && !(defined(__CUDACC__) && defined(BOOST_INT128_ENABLE_CUDA))
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

#ifndef BOOST_INT128_BUILD_MODULE
#  include <x86intrin.h>
#  include <emmintrin.h>
#endif

#  ifdef __ADX__
#    define BOOST_INT128_ADD_CARRY _addcarryx_u64
#    define BOOST_INT128_SUB_BORROW _subborrow_u64
#  else
#    define BOOST_INT128_ADD_CARRY _addcarry_u64
#    define BOOST_INT128_SUB_BORROW _subborrow_u64
#  endif

#elif defined(_M_AMD64)

#ifndef BOOST_INT128_BUILD_MODULE
#  include <intrin.h>
#endif

#  ifdef __ADX__
#    define BOOST_INT128_ADD_CARRY _addcarryx_u64
#    define BOOST_INT128_SUB_BORROW _subborrow_u64
#  else
#    define BOOST_INT128_ADD_CARRY _addcarry_u64
#    define BOOST_INT128_SUB_BORROW _subborrow_u64
#  endif

#elif defined(__i386__)

#ifndef BOOST_INT128_BUILD_MODULE
#  include <emmintrin.h>
#endif

#elif defined(_M_IX86)

#ifndef BOOST_INT128_BUILD_MODULE
#  include <intrin.h>
#endif

#endif // Platform macros

// Hardware 128-bit by 64-bit unsigned division via the x86-64 DIV instruction
// Excluded on the CUDA and SYCL device passes (the device target is not x86-64)
#if defined(__x86_64__) && (defined(__GNUC__) || defined(__clang__)) && !defined(_MSC_VER) && !defined(__CUDA_ARCH__) && !defined(__SYCL_DEVICE_ONLY__)
#  define BOOST_INT128_HAS_X86_64_DIVQ
#endif

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

#ifndef BOOST_INT128_BUILD_MODULE
#include <cassert>
#endif

#define BOOST_INT128_ASSERT(x) assert(x)
#define BOOST_INT128_ASSERT_MSG(expr, msg) assert((expr)&&(msg))

#ifdef _MSC_VER
#  define BOOST_INT128_ASSUME(expr) __assume(expr)
#elif defined(__clang__)
#  define BOOST_INT128_ASSUME(expr) __builtin_assume(expr)
#elif defined(__GNUC__)
#  if __GNUC__ >= 5 && __GNUC__ < 13
#    define BOOST_INT128_ASSUME(expr) if (expr) {} else { __builtin_unreachable(); }
#  else
#    define BOOST_INT128_ASSUME(expr) __attribute__((assume(expr)))
#  endif
#elif defined(__has_cpp_attribute)
#  if __has_cpp_attribute(assume)
#    define BOOST_INT128_ASSUME(expr) [[assume(expr)]]
#  else
#    define BOOST_INT128_ASSUME(expr) BOOST_INT128_ASSERT(expr)
#  endif
#else
#  define BOOST_INT128_ASSUME(expr) BOOST_INT128_ASSERT(expr)
#endif

#if defined(__has_builtin)
#define BOOST_INT128_HAS_BUILTIN(x) __has_builtin(x)
#else
#define BOOST_INT128_HAS_BUILTIN(x) false
#endif

#if BOOST_INT128_HAS_BUILTIN(__builtin_expect)
#  define BOOST_INT128_LIKELY(x) __builtin_expect(x, 1)
#  define BOOST_INT128_UNLIKELY(x) __builtin_expect(x, 0)
#else
#  define BOOST_INT128_LIKELY(x) x
#  define BOOST_INT128_UNLIKELY(x) x
#endif

#if !defined(__cpp_if_constexpr) || (__cpp_if_constexpr < 201606L)
#  define BOOST_INT128_NO_CXX17_IF_CONSTEXPR
#endif

#ifndef BOOST_INT128_NO_CXX17_IF_CONSTEXPR
#  define BOOST_INT128_IF_CONSTEXPR if constexpr
#else
#  define BOOST_INT128_IF_CONSTEXPR if
#endif

#if defined(__GNUC__) || defined(__clang__)
#  define BOOST_INT128_UNREACHABLE __builtin_unreachable()
#elif defined(_MSC_VER)
#  define BOOST_INT128_UNREACHABLE __assume(0)
#else
#  define BOOST_INT128_UNREACHABLE std::abort()
#endif

#ifdef BOOST_INT128_BUILD_MODULE
#  define BOOST_INT128_INLINE_CONSTEXPR inline constexpr
#  define BOOST_INT128_EXPORT export
#else
#  define BOOST_INT128_INLINE_CONSTEXPR static constexpr
#  define BOOST_INT128_EXPORT
#endif

// Detect if we can throw or not
// First check if the user said no explicitly
// Then check if it's been disabled elsewhere

#ifdef BOOST_INT128_DISABLE_EXCEPTIONS

#  define BOOST_INT128_THROW_EXCEPTION(expr)

#else

#  ifdef _MSC_VER
#    ifdef _CPPUNWIND
#      define BOOST_INT128_THROW_EXCEPTION(expr) throw expr;
#    else
#      define BOOST_INT128_THROW_EXCEPTION(expr)
#      define BOOST_INT128_DISABLE_EXCEPTIONS
#    endif
#  else
#    ifdef __EXCEPTIONS
#      define BOOST_INT128_THROW_EXCEPTION(expr) throw expr;
#    else
#      define BOOST_INT128_THROW_EXCEPTION(expr)
#      define BOOST_INT128_DISABLE_EXCEPTIONS
#    endif
#endif

#endif // Exceptions

#if defined(__cpp_impl_three_way_comparison) && __cpp_impl_three_way_comparison >= 201907L && __has_include(<compare>)
#  define BOOST_INT128_HAS_SPACESHIP_OPERATOR
#  ifndef BOOST_INT128_BUILD_MODULE
#    include <compare>
#  endif
#endif

// GPU device support. CUDA is auto-detected via __CUDACC__ (opt-in with
// BOOST_INT128_ENABLE_CUDA). SYCL is fully opt-in via BOOST_INT128_ENABLE_SYCL;
// <sycl/sycl.hpp> must be included before <boost/int128.hpp> so SYCL_EXTERNAL exists.
#if defined(__CUDACC__) && defined(BOOST_INT128_ENABLE_CUDA)
#  define BOOST_INT128_CUDA_ENABLED __host__ __device__
#  define BOOST_INT128_HAS_GPU_SUPPORT
#elif defined(BOOST_INT128_ENABLE_SYCL)
#  define BOOST_INT128_SYCL_ENABLED SYCL_EXTERNAL
#  define BOOST_INT128_HAS_GPU_SUPPORT
#endif

#ifndef BOOST_INT128_CUDA_ENABLED
#  define BOOST_INT128_CUDA_ENABLED
#endif
#ifndef BOOST_INT128_SYCL_ENABLED
#  define BOOST_INT128_SYCL_ENABLED
#endif

// Exactly one sub-macro is ever non-empty; expands to "__host__ __device__" (CUDA),
// "SYCL_EXTERNAL" (SYCL), or nothing (host).
#define BOOST_INT128_HOST_DEVICE BOOST_INT128_CUDA_ENABLED BOOST_INT128_SYCL_ENABLED

#endif // BOOST_INT128_DETAIL_CONFIG_HPP
// ===== END boost/int128/detail/config.hpp =====

namespace boost {
namespace int128 {

BOOST_INT128_EXPORT struct uint128_t;
BOOST_INT128_EXPORT struct int128_t;

} // namespace int128
} // namespace boost

#endif // BOOST_INT128_DETAIL_FWD_HPP
// ===== END boost/int128/detail/fwd.hpp =====
// ===== BEGIN boost/int128/detail/int128_imp.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_DETAIL_INT128_HPP
#define BOOST_INT128_DETAIL_INT128_HPP

// [amalgamate] skipped duplicate include of boost/int128/detail/fwd.hpp
// [amalgamate] skipped duplicate include of boost/int128/detail/config.hpp
// ===== BEGIN boost/int128/detail/traits.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_DETAIL_TRAITS_HPP
#define BOOST_INT128_DETAIL_TRAITS_HPP

// [amalgamate] skipped duplicate include of boost/int128/detail/config.hpp

#ifndef BOOST_INT128_BUILD_MODULE

#include <type_traits>
#include <cstdint>

#endif

namespace boost {
namespace int128 {
namespace detail {

template <typename T>
struct signed_integer
{
    static constexpr bool value = (std::is_signed<T>::value && std::is_integral<T>::value)
    #ifdef BOOST_INT128_HAS_INT128
    || std::is_same<T, builtin_i128>::value;
    #else
    ;
    #endif
};

template <typename T>
BOOST_INT128_INLINE_CONSTEXPR bool is_signed_integer_v = signed_integer<T>::value;

template <typename T>
struct unsigned_integer
{
    static constexpr bool value = (std::is_unsigned<T>::value && std::is_integral<T>::value)
    #ifdef BOOST_INT128_HAS_INT128
    || std::is_same<T, builtin_u128>::value;
    #else
    ;
    #endif
};

template <typename T>
BOOST_INT128_INLINE_CONSTEXPR bool is_unsigned_integer_v = unsigned_integer<T>::value;

template <typename T>
BOOST_INT128_INLINE_CONSTEXPR bool is_any_integer_v = signed_integer<T>::value || unsigned_integer<T>::value;

// Decides if we can use a u32 or u64 implementation for some operations

#ifdef BOOST_INT128_HAS_INT128

template <typename T>
using evaluation_type_t = std::conditional_t<sizeof(T) <= sizeof(std::uint32_t), std::uint32_t,
                            std::conditional_t<sizeof(T) <= sizeof(std::uint64_t), std::uint64_t, builtin_u128>>;

#else

template <typename T>
using evaluation_type_t = std::conditional_t<sizeof(T) <= sizeof(std::uint32_t), std::uint32_t, std::uint64_t>;

#endif

} // namespace detail
} // namespace int128
} // namespace boost

#define BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT typename SignedInteger, std::enable_if_t<detail::is_signed_integer_v<SignedInteger>, bool> = true
#define BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT typename UnsignedInteger, std::enable_if_t<detail::is_unsigned_integer_v<UnsignedInteger>, bool> = true
#define BOOST_INT128_DEFAULTED_INTEGER_CONCEPT typename Integer, std::enable_if_t<detail::is_any_integer_v<Integer>, bool> = true
#define BOOST_INT128_DEFAULTED_FLOATING_POINT_CONCEPT typename Float, std::enable_if_t<std::is_floating_point<Float>::value, bool> = true

#define BOOST_INT128_SIGNED_INTEGER_CONCEPT typename SignedInteger, std::enable_if_t<detail::is_signed_integer_v<SignedInteger>, bool>
#define BOOST_INT128_UNSIGNED_INTEGER_CONCEPT typename UnsignedInteger, std::enable_if_t<detail::is_unsigned_integer_v<UnsignedInteger>, bool>
#define BOOST_INT128_INTEGER_CONCEPT typename Integer, std::enable_if_t<detail::is_any_integer_v<Integer>, bool>
#define BOOST_INT128_FLOATING_POINT_CONCEPT typename Float, std::enable_if_t<std::is_floating_point<Float>::value, bool>

#if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)

#define BOOST_INT128_DEFAULTED_SIGNED_128BIT_INTEGER_CONCEPT typename SignedInteger, std::enable_if_t<std::is_same<SignedInteger, detail::builtin_i128>::value, bool> = true
#define BOOST_INT128_DEFAULTED_UNSIGNED_128BIT_INTEGER_CONCEPT typename UnsignedInteger, std::enable_if_t<std::is_same<UnsignedInteger, detail::builtin_u128>::value, bool> = true
#define BOOST_INT128_DEFAULTED_128BIT_INTEGER_CONCEPT typename Integer, std::enable_if_t<std::is_same<Integer, detail::builtin_u128>::value || std::is_same<Integer, detail::builtin_i128>::value, bool> = true

#define BOOST_INT128_SIGNED_128BIT_INTEGER_CONCEPT typename SignedInteger, std::enable_if_t<std::is_same<SignedInteger, detail::builtin_i128>::value, bool>
#define BOOST_INT128_UNSIGNED_128BIT_INTEGER_CONCEPT typename UnsignedInteger, std::enable_if_t<std::is_same<UnsignedInteger, detail::builtin_u128>::value, bool>
#define BOOST_INT128_128BIT_INTEGER_CONCEPT typename Integer, std::enable_if_t<std::is_same<Integer, detail::builtin_u128>::value || std::is_same<Integer, detail::builtin_i128>::value, bool>

#endif

#endif // BOOST_INT128_DETAIL_TRAITS_HPP
// ===== END boost/int128/detail/traits.hpp =====
// ===== BEGIN boost/int128/detail/constants.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_DETAIL_CONSTANTS_HPP
#define BOOST_INT128_DETAIL_CONSTANTS_HPP

#ifndef BOOST_INT128_BUILD_MODULE

#include <cstdint>
#include <limits>

#endif

namespace boost {
namespace int128 {
namespace detail {

BOOST_INT128_INLINE_CONSTEXPR std::uint64_t low_word_mask {(std::numeric_limits<std::uint64_t>::max)()};

template <typename T>
BOOST_INT128_INLINE_CONSTEXPR T offset_value_v = static_cast<T>((std::numeric_limits<std::uint64_t>::max)());

} // namespace detail
} // namespace int128
} // namespace boost

#endif // BOOST_INT128_DETAIL_CONSTANTS_HPP
// ===== END boost/int128/detail/constants.hpp =====
// ===== BEGIN boost/int128/detail/clz.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_DETAIL_CLZ_HPP
#define BOOST_INT128_DETAIL_CLZ_HPP

// [amalgamate] skipped duplicate include of boost/int128/detail/config.hpp

#ifndef BOOST_INT128_BUILD_MODULE

#include <limits>
#include <cstdint>

#endif

namespace boost {
namespace int128 {
namespace detail {

// The whole impl namespace is exported when building the module for testing so
// the low-level bit helpers can be exercised directly; it is an ordinary
// namespace in every other build.
BOOST_INT128_TEST_EXPORT namespace impl {

#if !(defined(__CUDACC__) && defined(BOOST_INT128_ENABLE_CUDA))

// See: http://graphics.stanford.edu/~seander/bithacks.html#IntegerLogDeBruijn
BOOST_INT128_INLINE_CONSTEXPR int index64[64] = {
    0, 47,  1, 56, 48, 27,  2, 60,
    57, 49, 41, 37, 28, 16,  3, 61,
    54, 58, 35, 52, 50, 42, 21, 44,
    38, 32, 29, 23, 17, 11,  4, 62,
    46, 55, 26, 59, 40, 36, 15, 53,
    34, 51, 20, 43, 31, 22, 10, 45,
    25, 39, 14, 33, 19, 30,  9, 24,
    13, 18,  8, 12,  7,  6,  5, 63
};

#endif

BOOST_INT128_HOST_DEVICE constexpr int bit_scan_reverse(std::uint64_t bb) noexcept
{
    #if defined(__CUDACC__) && defined(BOOST_INT128_ENABLE_CUDA)

    constexpr int index64[64] = {
        0, 47,  1, 56, 48, 27,  2, 60,
        57, 49, 41, 37, 28, 16,  3, 61,
        54, 58, 35, 52, 50, 42, 21, 44,
        38, 32, 29, 23, 17, 11,  4, 62,
        46, 55, 26, 59, 40, 36, 15, 53,
        34, 51, 20, 43, 31, 22, 10, 45,
        25, 39, 14, 33, 19, 30,  9, 24,
        13, 18,  8, 12,  7,  6,  5, 63
    };

    #endif

    constexpr auto debruijn64 {UINT64_C(0x03f79d71b4cb0a89)};

    BOOST_INT128_ASSUME(bb != 0); // LCOV_EXCL_LINE

    bb |= bb >> 1;
    bb |= bb >> 2;
    bb |= bb >> 4;
    bb |= bb >> 8;
    bb |= bb >> 16;
    bb |= bb >> 32;

    return index64[(bb * debruijn64) >> 58];
}

#if !(defined(__CUDACC__) && defined(BOOST_INT128_ENABLE_CUDA))

BOOST_INT128_INLINE_CONSTEXPR int countl_mod37[37] = {
    32, 31, 6, 30, 9, 5, 0, 29,
    16, 8, 2, 4, 21, 0, 19, 28,
    25, 15, 0, 7, 10, 1, 17, 3,
    22, 20, 26, 0, 11, 18, 23,
    27, 12, 24, 13, 14, 0
};

#endif

BOOST_INT128_HOST_DEVICE constexpr int backup_countl_impl(std::uint32_t x) noexcept
{
    #if defined(__CUDACC__) && defined(BOOST_INT128_ENABLE_CUDA)

    constexpr int countl_mod37[37] = {
        32, 31, 6, 30, 9, 5, 0, 29,
        16, 8, 2, 4, 21, 0, 19, 28,
        25, 15, 0, 7, 10, 1, 17, 3,
        22, 20, 26, 0, 11, 18, 23,
        27, 12, 24, 13, 14, 0
    };

    #endif

    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;

    return countl_mod37[x % 37];
}

#if BOOST_INT128_HAS_BUILTIN(__builtin_clz) && !(defined(__CUDACC__) && defined(BOOST_INT128_ENABLE_CUDA))

constexpr int countl_impl(unsigned int x) noexcept
{
    return x ? __builtin_clz(x) : std::numeric_limits<unsigned int>::digits;
}

constexpr int countl_impl(unsigned long x) noexcept
{
    return x ? __builtin_clzl(x) : std::numeric_limits<unsigned long>::digits;
}

constexpr int countl_impl(unsigned long long x) noexcept
{
    return x ? __builtin_clzll(x) : std::numeric_limits<unsigned long long>::digits;
}

#elif (defined(_M_AMD64) || defined(_M_ARM64)) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION) && !(defined(__CUDACC__) && defined(BOOST_INT128_ENABLE_CUDA))

constexpr int countl_impl(std::uint32_t x) noexcept
{
    if (BOOST_INT128_IS_CONSTANT_EVALUATED(x))
    {
        return backup_countl_impl(x); // LCOV_EXCL_LINE
    }
    else
    {
        unsigned long r {};

        if (_BitScanReverse(&r, x))
        {
            return 31 - static_cast<int>(r);
        }
        else
        {
            return 32;
        }
    }
}

constexpr int countl_impl(std::uint64_t x) noexcept
{
    if (BOOST_INT128_IS_CONSTANT_EVALUATED(x))
    {
        return x ? bit_scan_reverse(static_cast<std::uint64_t>(x)) ^ 63 : std::numeric_limits<std::uint64_t>::digits; // LCOV_EXCL_LINE
    }
    else
    {
        unsigned long r {};

        if (_BitScanReverse64(&r, x))
        {
            return 63 - static_cast<int>(r);
        }
        else
        {
            return 64;
        }
    }
}

#elif defined(_M_IX86) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION)

constexpr int countl_impl(std::uint32_t x) noexcept
{
    if (BOOST_INT128_IS_CONSTANT_EVALUATED(x))
    {
        return backup_countl_impl(x); // LCOV_EXCL_LINE
    }
    else
    {
        unsigned long r {};

        if (_BitScanReverse(&r, x))
        {
            return 31 - static_cast<int>(r);
        }
        else
        {
            return 32;
        }
    }
}

BOOST_INT128_HOST_DEVICE constexpr int countl_impl(std::uint64_t x) noexcept
{
    return x ? bit_scan_reverse(static_cast<std::uint64_t>(x)) ^ 63 : std::numeric_limits<std::uint64_t>::digits;
}

#else

template <typename T>
BOOST_INT128_HOST_DEVICE constexpr int countl_impl(T x) noexcept
{
    return x ? bit_scan_reverse(static_cast<std::uint64_t>(x)) ^ 63 : std::numeric_limits<T>::digits;
}

BOOST_INT128_HOST_DEVICE constexpr int countl_impl(std::uint32_t x) noexcept
{
    return backup_countl_impl(x);
}


#endif

} // namespace impl

template <typename T>
BOOST_INT128_HOST_DEVICE constexpr int countl_zero(T x) noexcept
{
    static_assert(std::numeric_limits<T>::is_integer && !std::numeric_limits<T>::is_signed,
                  "Can only count with unsigned integers");

    return impl::countl_impl(x);
}

} // namespace detail
} // namespace int128
} // namespace boost

#endif // BOOST_INT128_DETAIL_CLZ_HPP
// ===== END boost/int128/detail/clz.hpp =====
// ===== BEGIN boost/int128/detail/common_mul.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_DETAIL_COMMON_MUL_HPP
#define BOOST_INT128_DETAIL_COMMON_MUL_HPP

// [amalgamate] skipped duplicate include of boost/int128/detail/config.hpp

#ifndef BOOST_INT128_BUILD_MODULE

#include <cstdint>

#endif

namespace boost {
namespace int128 {
namespace detail {

// High 64 bits of the 64x64 -> 128 product, computed with four 32-bit partial products
BOOST_INT128_HOST_DEVICE BOOST_INT128_FORCE_INLINE constexpr std::uint64_t umulh_generic(const std::uint64_t a, const std::uint64_t b) noexcept
{
    const std::uint64_t a_lo {a & UINT32_MAX};
    const std::uint64_t a_hi {a >> 32U};
    const std::uint64_t b_lo {b & UINT32_MAX};
    const std::uint64_t b_hi {b >> 32U};

    const std::uint64_t lo_lo {a_lo * b_lo};
    const std::uint64_t hi_lo {a_hi * b_lo};
    const std::uint64_t lo_hi {a_lo * b_hi};
    const std::uint64_t hi_hi {a_hi * b_hi};

    const std::uint64_t cross {(lo_lo >> 32U) + (hi_lo & UINT32_MAX) + (lo_hi & UINT32_MAX)};

    return hi_hi + (hi_lo >> 32U) + (lo_hi >> 32U) + (cross >> 32U);
}

// Full 64x64 -> 128 product
BOOST_INT128_HOST_DEVICE BOOST_INT128_FORCE_INLINE constexpr std::uint64_t umul(const std::uint64_t a, const std::uint64_t b, std::uint64_t& hi) noexcept
{
    #ifndef BOOST_INT128_NO_CONSTEVAL_DETECTION

    if (!BOOST_INT128_IS_CONSTANT_EVALUATED(a))
    {
        #if defined(BOOST_INT128_HAS_INT128)

        const detail::builtin_u128 product {static_cast<detail::builtin_u128>(a) * static_cast<detail::builtin_u128>(b)};
        hi = static_cast<std::uint64_t>(product >> 64U);
        return static_cast<std::uint64_t>(product);

        #elif defined(_M_AMD64) && !defined(__GNUC__) && !defined(__CUDA_ARCH__) && !defined(__SYCL_DEVICE_ONLY__)

        return _umul128(a, b, &hi);

        #elif defined(_M_ARM64) && !defined(__CUDA_ARCH__) && !defined(__SYCL_DEVICE_ONLY__)

        hi = __umulh(a, b);
        return a * b;

        #endif
    }

    #endif

    hi = umulh_generic(a, b);
    return a * b;
}

// Low 128 bits of a 128x128 product
template <typename ReturnType, typename T>
BOOST_INT128_HOST_DEVICE BOOST_INT128_FORCE_INLINE constexpr ReturnType low_word_mul(const T& lhs, const T& rhs) noexcept
{
    using high_word_type = decltype(ReturnType{}.high);

    std::uint64_t result_high {};
    const std::uint64_t result_low {umul(lhs.low, rhs.low, result_high)};

    result_high += lhs.low * static_cast<std::uint64_t>(rhs.high);
    result_high += static_cast<std::uint64_t>(lhs.high) * rhs.low;

    return ReturnType{static_cast<high_word_type>(result_high), result_low};
}

// Low 128 bits of a 128x64 product
template <typename ReturnType, typename T>
BOOST_INT128_HOST_DEVICE BOOST_INT128_FORCE_INLINE constexpr ReturnType low_word_mul(const T& lhs, const std::uint64_t rhs) noexcept
{
    using high_word_type = decltype(ReturnType{}.high);

    std::uint64_t result_high {};
    const std::uint64_t result_low {umul(lhs.low, rhs, result_high)};

    result_high += static_cast<std::uint64_t>(lhs.high) * rhs;

    return ReturnType{static_cast<high_word_type>(result_high), result_low};
}

// Low 128 bits of a 128x32 product
template <typename ReturnType, typename T>
BOOST_INT128_HOST_DEVICE BOOST_INT128_FORCE_INLINE constexpr ReturnType low_word_mul(const T& lhs, const std::uint32_t rhs) noexcept
{
    return low_word_mul<ReturnType>(lhs, static_cast<std::uint64_t>(rhs));
}

} // namespace detail
} // namespace int128
} // namespace boost

#endif // BOOST_INT128_DETAIL_COMMON_MUL_HPP
// ===== END boost/int128/detail/common_mul.hpp =====
// ===== BEGIN boost/int128/detail/common_div.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_DETAIL_COMMON_DIV_HPP
#define BOOST_INT128_DETAIL_COMMON_DIV_HPP

// [amalgamate] skipped duplicate include of boost/int128/detail/config.hpp
// [amalgamate] skipped duplicate include of boost/int128/detail/clz.hpp
// [amalgamate] skipped duplicate include of boost/int128/detail/common_mul.hpp

#ifndef BOOST_INT128_BUILD_MODULE

#include <cstdint>
#include <cstring>

#endif

namespace boost {
namespace int128 {
namespace detail {

#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wassume"
#endif

template <typename T>
BOOST_INT128_HOST_DEVICE BOOST_INT128_FORCE_INLINE constexpr void half_word_div(const T& lhs, const std::uint32_t rhs, T& quotient, T& remainder) noexcept
{
    using high_word_type = decltype(T{}.high);

    BOOST_INT128_ASSUME(rhs != 0); // LCOV_EXCL_LINE

    // Use Barrett reduction-inspired approach
    const std::uint64_t divisor {rhs};

    const auto q_high {static_cast<std::uint64_t>(lhs.high) / divisor};
    auto r {static_cast<std::uint64_t>(lhs.high) % divisor};

    const auto low_high {static_cast<std::uint32_t>(lhs.low >> 32U)};
    const auto low_low {static_cast<std::uint32_t>(lhs.low)};

    r = (r << 32U) | low_high;
    const auto q_mid {r / divisor};
    r %= divisor;

    r = (r << 32U) | low_low;
    const auto q_low {r / divisor};
    r %= divisor;

    quotient.high = static_cast<high_word_type>(q_high);
    quotient.low = (q_mid << 32U) | q_low;
    remainder.low = r;
}

template <typename T>
BOOST_INT128_HOST_DEVICE BOOST_INT128_FORCE_INLINE constexpr void half_word_div(const T& lhs, const std::uint32_t rhs, T& quotient) noexcept
{
    using high_word_type = decltype(T{}.high);

    BOOST_INT128_ASSUME(rhs != 0); // LCOV_EXCL_LINE

    quotient.high = static_cast<high_word_type>(static_cast<std::uint64_t>(lhs.high) / rhs);
    auto remainder {((static_cast<std::uint64_t>(lhs.high) % rhs) << 32) | (lhs.low >> 32)};
    quotient.low = (remainder / rhs) << 32;
    remainder = ((remainder % rhs) << 32) | (lhs.low & UINT32_MAX);
    quotient.low |= (remainder / rhs) & UINT32_MAX;
}

// Portable 128-bit by 64-bit unsigned division producing a 64-bit quotient and remainder.
// This is the classic Hacker's Delight divlu (two 32-bit "digit" steps over 64-bit words).
// Precondition: u1 < d so the quotient is guaranteed to fit in 64 bits. It is constexpr-safe
// and serves as the fallback for udiv_2by1 on every target without a hardware 128/64 divide.
BOOST_INT128_HOST_DEVICE BOOST_INT128_FORCE_INLINE constexpr std::uint64_t divlu(std::uint64_t u1, std::uint64_t u0, std::uint64_t d, std::uint64_t& r) noexcept
{
    constexpr std::uint64_t b {UINT64_C(1) << 32U}; // Number base (2^32)

    BOOST_INT128_ASSUME(u1 < d); // LCOV_EXCL_LINE

    // D.1: normalize so that the divisor's most significant bit is set
    const auto s {countl_zero(d)};
    d <<= s;

    const auto vn1 {d >> 32U};
    const auto vn0 {d & UINT32_MAX};

    // Shift the dividend left by s. The (64 - s) shift is undefined when s == 0, so guard it.
    const auto un32 {s == 0 ? u1 : ((u1 << s) | (u0 >> (64 - s)))};
    const auto un10 {u0 << s};

    const auto un1 {un10 >> 32U};
    const auto un0 {un10 & UINT32_MAX};

    // First quotient digit
    auto q1 {un32 / vn1};
    auto rhat {un32 - (q1 * vn1)};

    while (q1 >= b || (q1 * vn0) > ((b * rhat) + un1))
    {
        --q1;
        rhat += vn1;
        if (rhat >= b)
        {
            break;
        }
    }

    const auto un21 {(un32 * b) + un1 - (q1 * d)};

    // Second quotient digit
    auto q0 {un21 / vn1};
    rhat = un21 - (q0 * vn1);

    while (q0 >= b || (q0 * vn0) > ((b * rhat) + un0))
    {
        --q0;
        rhat += vn1;
        if (rhat >= b)
        {
            break;
        }
    }

    // The remainder is shifted back down by the normalization amount
    r = ((un21 * b) + un0 - (q0 * d)) >> s;
    return (q1 * b) + q0;
}

#if defined(BOOST_INT128_HAS_X86_64_DIVQ)

// Inline asm cannot appear in a constexpr function body before C++20, so the x86-64 DIV
// instruction is wrapped in a non-constexpr helper that udiv_2by1 only calls at runtime.
BOOST_INT128_FORCE_INLINE std::uint64_t udiv_2by1_divq(const std::uint64_t u1, const std::uint64_t u0, const std::uint64_t d, std::uint64_t& r) noexcept
{
    std::uint64_t q {};
    __asm__("divq %[d]" : "=a"(q), "=d"(r) : [d] "r"(d), "a"(u0), "d"(u1) : "cc");
    return q;
}

#endif // BOOST_INT128_HAS_X86_64_DIVQ

// Divides the 128-bit value (u1:u0) by d, returning a 64-bit quotient and the true remainder.
// Precondition: u1 < d. Mirrors common_mul.hpp::umul: a hardware instruction at runtime where
// one exists, and the portable divlu in constexpr evaluation and everywhere else.
BOOST_INT128_HOST_DEVICE BOOST_INT128_FORCE_INLINE constexpr std::uint64_t udiv_2by1(const std::uint64_t u1, const std::uint64_t u0, const std::uint64_t d, std::uint64_t& r) noexcept
{
    BOOST_INT128_ASSUME(u1 < d); // LCOV_EXCL_LINE

    #if (defined(BOOST_INT128_HAS_X86_64_DIVQ) || (defined(_M_AMD64) && !defined(__GNUC__) && !defined(__clang__) && _MSC_VER >= 1920)) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION)

    if (!BOOST_INT128_IS_CONSTANT_EVALUATED(u1))
    {
        #if defined(BOOST_INT128_HAS_X86_64_DIVQ)

        return udiv_2by1_divq(u1, u0, d, r);

        #else

        return _udiv128(u1, u0, d, &r);

        #endif
    }

    #endif

    return divlu(u1, u0, d, r);
}

#if defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 4127) // Pre c++17 the if constexpr remainder part will hit this
#endif

// Divides the 128-bit value (uh:ul) by the 128-bit divisor (vh:vl) where vh != 0. Because the
// divisor is >= 2^64 the quotient is guaranteed to fit in a single 64-bit word, which is
// returned. When need_remainder is true the 128-bit remainder is written to (rem_hi:rem_lo).
//
// This is one normalized quotient digit (Knuth Algorithm D specialized to a 2-word divisor).
// The top-limb estimate qhat (reusing udiv_2by1, a hardware divq on x86-64) is bounded by
// Knuth Theorem B to q <= qhat <= q + 2; the D3 refinement against d0 tightens it to q <= qhat
// <= q + 1, and the conditional add-back then corrects the remaining off-by-one.
template <bool need_remainder>
BOOST_INT128_HOST_DEVICE BOOST_INT128_FORCE_INLINE constexpr std::uint64_t div3by2(const std::uint64_t uh, const std::uint64_t ul,
    const std::uint64_t vh, const std::uint64_t vl, std::uint64_t& rem_hi, std::uint64_t& rem_lo) noexcept
{
    BOOST_INT128_ASSUME(vh != 0); // LCOV_EXCL_LINE

    // D.1: normalize so the divisor's most significant bit is set
    const auto s {countl_zero(vh)};
    const auto cs {64 - s};

    std::uint64_t d1 {};
    std::uint64_t d0 {};
    std::uint64_t u2 {};
    std::uint64_t u1 {};
    std::uint64_t u0 {};

    if (s == 0)
    {
        d1 = vh;
        d0 = vl;
        u2 = 0;
        u1 = uh;
        u0 = ul;
    }
    else
    {
        d1 = (vh << s) | (vl >> cs);
        d0 = vl << s;
        u2 = uh >> cs;
        u1 = (uh << s) | (ul >> cs);
        u0 = ul << s;
    }

    BOOST_INT128_ASSUME(u2 <= d1); // LCOV_EXCL_LINE

    // D.3: estimate the single quotient digit qhat = floor((u2:u1) / d1), clamped to 2^64 - 1.
    // rhat is the remainder of that estimate.
    std::uint64_t qhat {};
    std::uint64_t rhat {};
    bool rhat_overflow {false};
    if (u2 < d1)
    {
        qhat = udiv_2by1(u2, u1, d1, rhat);
    }
    else
    {
        // u2 == d1: floor((u2:u1)/d1) clamps to 2^64 - 1, leaving rhat == u1 + d1 (may carry).
        qhat = UINT64_MAX;
        rhat = u1 + d1;
        rhat_overflow = rhat < u1;
    }

    std::uint64_t qd0_hi {};
    auto qd0_lo {umul(qhat, d0, qd0_hi)};

    // Refine qhat against d0 (Knuth D3). The top-limb estimate alone can exceed the true quotient
    // by up to 2; this brings it down to at most one too large, which the add-back below corrects.
    // At most two iterations run, and only while the running remainder rhat stays below 2^64.
    if (!rhat_overflow)
    {
        while (qd0_hi > rhat || (qd0_hi == rhat && qd0_lo > u0))
        {
            --qhat;
            rhat += d1;
            const auto rhat_carry {rhat < d1};
            qd0_lo = umul(qhat, d0, qd0_hi);
            if (rhat_carry)
            {
                break;
            }
        }
    }

    // D.4: multiply and subtract (u2:u1:u0) - qhat * (d1:d0). qd0 already holds qhat * d0.
    std::uint64_t qd1_hi {};
    const auto qd1_lo {umul(qhat, d1, qd1_hi)};

    const auto p0 {qd0_lo};
    const auto p1 {qd0_hi + qd1_lo};
    const auto p2 {qd1_hi + static_cast<std::uint64_t>(p1 < qd0_hi)};

    const auto r0 {u0 - p0};
    const auto borrow0 {static_cast<std::uint64_t>(u0 < p0)};
    const auto t1 {u1 - p1};
    auto r1 {t1 - borrow0};
    const auto borrow1 {static_cast<std::uint64_t>(u1 < p1) + static_cast<std::uint64_t>(t1 < borrow0)};

    // D.5/D.6: if the top limb borrowed, qhat was one too large. Correct it and add the divisor
    // back into the remainder. The probability of this branch is small.
    auto r0_final {r0};
    if (BOOST_INT128_UNLIKELY((u2 < p2) || ((u2 - p2) < borrow1)))
    {
        --qhat;                                                  // LCOV_EXCL_LINE
        const auto sum0 {r0 + d0};                               // LCOV_EXCL_LINE
        r0_final = sum0;                                         // LCOV_EXCL_LINE
        r1 = r1 + d1 + static_cast<std::uint64_t>(sum0 < r0);    // LCOV_EXCL_LINE
    }

    BOOST_INT128_IF_CONSTEXPR (need_remainder)
    {
        if (s == 0)
        {
            rem_hi = r1;
            rem_lo = r0_final;
        }
        else
        {
            rem_lo = (r0_final >> s) | (r1 << cs);
            rem_hi = r1 >> s;
        }
    }
    else
    {
        static_cast<void>(rem_hi);
        static_cast<void>(rem_lo);
    }

    return qhat;
}

#if defined(_MSC_VER)
#  pragma warning(pop)
#endif

namespace impl {

#if defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 4127) // Pre c++17 the if constexpr remainder part will hit this
#endif

template <std::size_t v_size>
BOOST_INT128_HOST_DEVICE BOOST_INT128_FORCE_INLINE constexpr void unpack_v(std::uint32_t (&vn)[4], const std::uint32_t (&v)[v_size],
    const bool needs_shift, const int s, const int complement_s, const std::integral_constant<std::size_t, 2>&) noexcept
{
    vn[1] = needs_shift ? ((v[1] << s) | (v[0] >> complement_s)) : v[1];
    vn[0] = needs_shift ? (v[0] << s) : v[0];
}

template <std::size_t v_size>
BOOST_INT128_HOST_DEVICE BOOST_INT128_FORCE_INLINE constexpr void unpack_v(std::uint32_t (&vn)[4], const std::uint32_t (&v)[v_size],
    const bool needs_shift, const int s, const int complement_s, const std::integral_constant<std::size_t, 4>&) noexcept
{
    vn[3] = needs_shift ? ((v[3] << s) | (v[2] >> complement_s)) : v[3];
    vn[2] = needs_shift ? ((v[2] << s) | (v[1] >> complement_s)) : v[2];
    vn[1] = needs_shift ? ((v[1] << s) | (v[0] >> complement_s)) : v[1];
    vn[0] = needs_shift ? (v[0] << s) : v[0];
}

// See: The Art of Computer Programming Volume 2 (Semi-numerical algorithms) section 4.3.1
// Algorithm D: Division of Non-negative integers
template <bool need_remainder, std::size_t u_size, std::size_t v_size, std::size_t q_size>
BOOST_INT128_HOST_DEVICE constexpr void knuth_divide(std::uint32_t (&u)[u_size], const std::size_t m,
                            const std::uint32_t (&v)[v_size], const std::size_t n,
                            std::uint32_t (&q)[q_size]) noexcept
{
    // D.1
    const auto s {countl_zero(v[n - 1])};
    const auto complement_s {32 - s};
    const bool needs_shift {s > 0};

    // Create normalized versions of u and v
    std::uint32_t un[5] {};
    std::uint32_t vn[4] {};

    un[4] = needs_shift ? (u[3] >> complement_s) : 0;
    un[3] = needs_shift ? ((u[3] << s) | (u[2] >> complement_s)) : u[3];
    un[2] = needs_shift ? ((u[2] << s) | (u[1] >> complement_s)) : u[2];
    un[1] = needs_shift ? ((u[1] << s) | (u[0] >> complement_s)) : u[1];
    un[0] = needs_shift ? (u[0] << s) : u[0];

    static_assert(v_size == 4 || v_size == 2, "Unknown size for denominator");
    unpack_v(vn, v, needs_shift, s, complement_s, std::integral_constant<std::size_t, v_size>{});

    // D.2
    for (std::size_t j {m - n}; j != static_cast<std::size_t>(-1); --j)
    {
        // D.3
        const auto dividend {(static_cast<std::uint64_t>(un[j+n]) << 32) | un[j+n-1]};
        const auto divisor {static_cast<std::uint64_t>(vn[n-1])};
        auto q_hat {dividend / divisor};
        auto r_hat {dividend % divisor};

        while (q_hat > UINT32_MAX ||
               (q_hat * vn[n-2]) > ((r_hat << 32) | un[j+n-2]))
        {
            --q_hat;
            r_hat += vn[n-1];
            if (r_hat > UINT32_MAX)
            {
                break;
            }
        }

        // D.4
        std::int64_t borrow {};
        for (std::size_t i {}; i < n; ++i)
        {
            const auto p {q_hat * vn[i]};
            const auto p_lo {static_cast<std::uint32_t>(p & UINT32_MAX)};
            const auto p_hi {static_cast<std::uint32_t>(p >> 32)};

            borrow += static_cast<std::int64_t>(un[j+i]) - static_cast<std::int64_t>(p_lo);
            un[j+i] = static_cast<std::uint32_t>(borrow & UINT32_MAX);
            borrow >>= 32;

            borrow -= p_hi;
        }
        borrow += un[j+n];
        un[j+n] = static_cast<std::uint32_t>(borrow & UINT32_MAX);

        // D.5
        q[j] = static_cast<std::uint32_t>(q_hat & UINT32_MAX);
        if (BOOST_INT128_UNLIKELY(borrow < 0))
        {
            // D.6
            // The probability of hitting this path is about 4.7e-10
            --q[j];                                                             // LCOV_EXCL_LINE
            std::uint64_t carry {};                                             // LCOV_EXCL_LINE
            for (std::size_t i = 0; i < n; ++i)                                 // LCOV_EXCL_LINE
            {                                                                   // LCOV_EXCL_LINE
                carry += static_cast<std::uint64_t>(un[j+i]) + vn[i];           // LCOV_EXCL_LINE
                un[j+i] = static_cast<std::uint32_t>(carry & UINT32_MAX);       // LCOV_EXCL_LINE
                carry >>= 32U;                                                  // LCOV_EXCL_LINE
            }                                                                   // LCOV_EXCL_LINE
            un[j+n] += static_cast<std::uint32_t>(carry & UINT32_MAX);          // LCOV_EXCL_LINE
        }
    }

    // D.8
    // If we are only calculating division we can completely skip this step
    BOOST_INT128_IF_CONSTEXPR (need_remainder)
    {
        if (s > 0)
        {
            for (std::size_t i {}; i < n-1; i++)
            {
                u[i] = (un[i] >> s) | (un[i+1] << (32 - s));
            }
            u[n-1] = un[n-1] >> s;
        }
        else
        {
            for (std::size_t i {}; i < n; i++)
            {
                u[i] = un[i];
            }
        }

        // Clear anything left in u
        for (std::size_t i {n}; i < m; i++)
        {
            u[i] = 0;
        }
    }
}

#if defined(_MSC_VER)
#  pragma warning(pop)
#endif

template <typename T>
BOOST_INT128_HOST_DEVICE BOOST_INT128_FORCE_INLINE constexpr std::size_t to_words(const T& x, std::uint32_t (&words)[4]) noexcept
{
    #if !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION) && !BOOST_INT128_ENDIAN_BIG_BYTE
    if (!BOOST_INT128_IS_CONSTANT_EVALUATED(x))
    {
        std::memcpy(&words, &x, sizeof(T));
    }
    else
    #endif
    {
        words[0] = static_cast<std::uint32_t>(x.low & UINT32_MAX);                              // LCOV_EXCL_LINE
        words[1] = static_cast<std::uint32_t>(x.low >> 32);                                     // LCOV_EXCL_LINE
        words[2] = static_cast<std::uint32_t>(static_cast<std::uint64_t>(x.high) & UINT32_MAX); // LCOV_EXCL_LINE
        words[3] = static_cast<std::uint32_t>(static_cast<std::uint64_t>(x.high) >> 32);        // LCOV_EXCL_LINE
    }

    BOOST_INT128_ASSERT_MSG(x != static_cast<T>(0), "Division by 0");

    std::size_t word_count {4};
    while (words[word_count - 1U] == 0U)
    {
        word_count--;
    }

    return word_count;
}

BOOST_INT128_HOST_DEVICE BOOST_INT128_FORCE_INLINE constexpr std::size_t to_words(const std::uint64_t x, std::uint32_t (&words)[2]) noexcept
{
    #if !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION) && !BOOST_INT128_ENDIAN_BIG_BYTE
    if (!BOOST_INT128_IS_CONSTANT_EVALUATED(x))
    {
        std::memcpy(&words, &x, sizeof(std::uint64_t));
    }
    else
    #endif
    {
        words[0] = static_cast<std::uint32_t>(x & UINT32_MAX);  // LCOV_EXCL_LINE
        words[1] = static_cast<std::uint32_t>(x >> 32);         // LCOV_EXCL_LINE
    }

    return x > UINT32_MAX ? 2 : 1;
}

BOOST_INT128_HOST_DEVICE BOOST_INT128_FORCE_INLINE constexpr std::size_t to_words(const std::uint32_t x, std::uint32_t (&words)[1]) noexcept
{
    words[0] = x;

    return 1;
}

template <typename T>
BOOST_INT128_HOST_DEVICE BOOST_INT128_FORCE_INLINE constexpr T from_words(const std::uint32_t (&words)[4]) noexcept
{
    using high_word_type = decltype(T{}.high);

    const auto low {static_cast<std::uint64_t>(words[0]) | (static_cast<std::uint64_t>(words[1]) << 32)};
    const auto high {static_cast<std::uint64_t>(words[2]) | (static_cast<std::uint64_t>(words[3]) << 32)};

    return {static_cast<high_word_type>(high), low};
}

} // namespace impl

// We only need to take the time to process the remainder in the modulo case
// In the division case it is a waste of cycles
//
// 128/64 -> 128-bit quotient (and optional 64-bit remainder) by two-step long division.
// The leading 64/64 yields the high quotient word and a remainder r < rhs, which satisfies
// the udiv_2by1 precondition for the low quotient word. This covers every rhs (including
// rhs <= UINT32_MAX) through the single hardware-or-portable udiv_2by1 primitive.

template <typename T>
BOOST_INT128_HOST_DEVICE BOOST_INT128_FORCE_INLINE constexpr void one_word_div(const T& lhs, const std::uint64_t rhs, T& quotient) noexcept
{
    using high_word_type = decltype(T{}.high);

    BOOST_INT128_ASSUME(rhs != 0); // LCOV_EXCL_LINE

    const auto u_high {static_cast<std::uint64_t>(lhs.high)};

    quotient.high = static_cast<high_word_type>(u_high / rhs);
    auto r {u_high % rhs};
    quotient.low = udiv_2by1(r, lhs.low, rhs, r);
}

template <typename T>
BOOST_INT128_HOST_DEVICE BOOST_INT128_FORCE_INLINE constexpr void one_word_div(const T& lhs, const std::uint64_t rhs, T& quotient, T& remainder) noexcept
{
    using high_word_type = decltype(T{}.high);

    BOOST_INT128_ASSUME(rhs != 0); // LCOV_EXCL_LINE

    const auto u_high {static_cast<std::uint64_t>(lhs.high)};

    quotient.high = static_cast<high_word_type>(u_high / rhs);
    auto r {u_high % rhs};
    quotient.low = udiv_2by1(r, lhs.low, rhs, r);

    remainder.high = static_cast<high_word_type>(0);
    remainder.low = r;
}

template <typename T>
BOOST_INT128_HOST_DEVICE BOOST_INT128_FORCE_INLINE constexpr void one_word_div(const T& lhs, const std::uint32_t rhs, T& quotient, T& remainder) noexcept
{
    one_word_div(lhs, static_cast<std::uint64_t>(rhs), quotient, remainder);
}

template <typename T>
BOOST_INT128_HOST_DEVICE BOOST_INT128_FORCE_INLINE constexpr void one_word_div(const T& lhs, const std::uint32_t rhs, T& quotient) noexcept
{
    one_word_div(lhs, static_cast<std::uint64_t>(rhs), quotient);
}

#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable : 4127) // Conditional expression is constant is true pre-C++17
#  pragma warning(disable : 4804) // Unsafe comparison with bool
#endif

template <typename T>
BOOST_INT128_HOST_DEVICE BOOST_INT128_FORCE_INLINE constexpr T knuth_div(const T& dividend, const T& divisor) noexcept
{
    BOOST_INT128_ASSUME(divisor != static_cast<T>(0));

    using high_word_type = decltype(T{}.high);

    std::uint64_t rem_hi {};
    std::uint64_t rem_lo {};

    const auto q {div3by2<false>(static_cast<std::uint64_t>(dividend.high), dividend.low,
                                 static_cast<std::uint64_t>(divisor.high), divisor.low, rem_hi, rem_lo)};

    return T{static_cast<high_word_type>(0), q};
}

template <typename T>
BOOST_INT128_HOST_DEVICE BOOST_INT128_FORCE_INLINE constexpr T knuth_div(const T& dividend, const T& divisor, T& remainder) noexcept
{
    BOOST_INT128_ASSUME(divisor != static_cast<T>(0));

    using high_word_type = decltype(T{}.high);

    std::uint64_t rem_hi {};
    std::uint64_t rem_lo {};

    const auto q {div3by2<true>(static_cast<std::uint64_t>(dividend.high), dividend.low,
                                static_cast<std::uint64_t>(divisor.high), divisor.low, rem_hi, rem_lo)};

    remainder = T{static_cast<high_word_type>(rem_hi), rem_lo};

    return T{static_cast<high_word_type>(0), q};
}

#ifdef _MSC_VER
#  pragma warning(pop)
#endif

#if defined(__clang__)
#  pragma clang diagnostic pop
#endif


} // namespace detail
} // namespace int128
} // namespace boost

#endif // BOOST_INT128_DETAIL_COMMON_DIV_HPP
// ===== END boost/int128/detail/common_div.hpp =====

#ifndef BOOST_INT128_BUILD_MODULE

#include <cstdint>
#include <cstring>

#endif

namespace boost {
namespace int128 {

struct
    #if (defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)) && !defined(_M_IX86)
    alignas(alignof(detail::builtin_i128))
    #endif
int128_t
{
    #if BOOST_INT128_ENDIAN_LITTLE_BYTE
    std::uint64_t low {};
    std::int64_t high {};
    #else

    #ifdef __GNUC__
    #  pragma GCC diagnostic push
    #  pragma GCC diagnostic ignored "-Wreorder"
    #endif

    std::int64_t high {};
    std::uint64_t low {};

    #ifdef __GNUC__
    #  pragma GCC diagnostic pop
    #endif

    #endif

    // Defaulted basic construction
    constexpr int128_t() noexcept = default;
    constexpr int128_t(const int128_t&) noexcept = default;
    constexpr int128_t(int128_t&&) noexcept = default;
    constexpr int128_t& operator=(const int128_t&) noexcept = default;
    constexpr int128_t& operator=(int128_t&&) noexcept = default;

    // Requires a conversion file to be implemented
    BOOST_INT128_HOST_DEVICE constexpr int128_t(const uint128_t& v) noexcept;

    // Construct from integral types
    #if BOOST_INT128_ENDIAN_LITTLE_BYTE

    BOOST_INT128_HOST_DEVICE constexpr int128_t(const std::int64_t hi, const std::uint64_t lo) noexcept : low{lo}, high{hi} {}

    template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE constexpr int128_t(const SignedInteger v) noexcept : low {static_cast<std::uint64_t>(v)}, high {v < 0 ? -1 : 0} {}

    template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE constexpr int128_t(const UnsignedInteger v) noexcept : low {static_cast<std::uint64_t>(v)}, high {} {}

    #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)

    BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR int128_t(const detail::builtin_i128 v) noexcept : low {static_cast<std::uint64_t>(v & static_cast<detail::builtin_i128>(detail::low_word_mask))}, high {static_cast<std::int64_t>(v >> static_cast<detail::builtin_i128>(64U))} {}
    BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR int128_t(const detail::builtin_u128 v) noexcept : low {static_cast<std::uint64_t>(v & static_cast<detail::builtin_u128>(detail::low_word_mask))}, high {static_cast<std::int64_t>(v >> static_cast<detail::builtin_u128>(64U))} {}

    #endif // BOOST_INT128_HAS_INT128

    #else // Big endian

    BOOST_INT128_HOST_DEVICE constexpr int128_t(const std::int64_t hi, const std::uint64_t lo) noexcept : high{hi}, low{lo} {}

    template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE constexpr int128_t(const SignedInteger v) noexcept : high{v < 0 ? -1 : 0}, low{static_cast<std::uint64_t>(v)} {}

    template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE constexpr int128_t(const UnsignedInteger v) noexcept : high {}, low {static_cast<std::uint64_t>(v)} {}

    #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)

    BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR int128_t(const detail::builtin_i128 v) noexcept : high {static_cast<std::int64_t>(v >> 64U)}, low {static_cast<std::uint64_t>(v & detail::low_word_mask)} {}
    BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR int128_t(const detail::builtin_u128 v) noexcept : high {static_cast<std::int64_t>(v >> 64U)}, low {static_cast<std::uint64_t>(v & detail::low_word_mask)} {}

    #endif // BOOST_INT128_HAS_INT128

    #endif // BOOST_INT128_ENDIAN_LITTLE_BYTE

    // Construct from floating-point types
    template <BOOST_INT128_DEFAULTED_FLOATING_POINT_CONCEPT>
    BOOST_INT128_HOST_DEVICE constexpr int128_t(Float f) noexcept;

    // Integer Conversion operators
    BOOST_INT128_HOST_DEVICE explicit constexpr operator bool() const noexcept { return low || high; }

    template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE constexpr operator SignedInteger() const noexcept { return static_cast<SignedInteger>(low); }

    template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE constexpr operator UnsignedInteger() const noexcept { return static_cast<UnsignedInteger>(low); }

    #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)

    BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR operator detail::builtin_i128() const noexcept { return static_cast<detail::builtin_i128>(static_cast<detail::builtin_u128>(high) << static_cast<detail::builtin_u128>(64)) | static_cast<detail::builtin_i128>(low); }

    BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR operator detail::builtin_u128() const noexcept { return (static_cast<detail::builtin_u128>(high) << static_cast<detail::builtin_u128>(64)) | static_cast<detail::builtin_u128>(low); }

    #endif // BOOST_INT128_HAS_INT128

    // Conversion to float
    // This is basically the same as ldexp(static_cast<T>(high), 64) + static_cast<T>(low),
    // but can be constexpr at C++11 instead of C++26
    BOOST_INT128_HOST_DEVICE constexpr operator float() const noexcept;
    BOOST_INT128_HOST_DEVICE constexpr operator double() const noexcept;

    // Long double does not exist on the CUDA or SYCL (spir64) device
    #if !defined(BOOST_INT128_HAS_GPU_SUPPORT)
    constexpr operator long double() const noexcept;
    #endif

    // Compound Or
    template <BOOST_INT128_DEFAULTED_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE constexpr int128_t& operator|=(Integer rhs) noexcept;

    BOOST_INT128_HOST_DEVICE constexpr int128_t& operator|=(int128_t rhs) noexcept;

    #ifdef BOOST_INT128_HAS_MSVC_INT128

    template <BOOST_INT128_DEFAULTED_128BIT_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE inline int128_t& operator|=(Integer rhs) noexcept;

    #endif // BOOST_INT128_HAS_MSVC_INT128

    // Compound And
    template <BOOST_INT128_DEFAULTED_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE constexpr int128_t& operator&=(Integer rhs) noexcept;

    BOOST_INT128_HOST_DEVICE constexpr int128_t& operator&=(int128_t rhs) noexcept;

    #ifdef BOOST_INT128_HAS_MSVC_INT128

    template <BOOST_INT128_DEFAULTED_128BIT_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE inline int128_t& operator&=(Integer rhs) noexcept;

    #endif // BOOST_INT128_HAS_MSVC_INT128

    // Compound XOR
    template <BOOST_INT128_DEFAULTED_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE constexpr int128_t& operator^=(Integer rhs) noexcept;

    BOOST_INT128_HOST_DEVICE constexpr int128_t& operator^=(int128_t rhs) noexcept;

    #ifdef BOOST_INT128_HAS_MSVC_INT128

    template <BOOST_INT128_DEFAULTED_128BIT_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE inline int128_t& operator^=(Integer rhs) noexcept;

    #endif // BOOST_INT128_HAS_MSVC_INT128

    // Compound Left Shift
    template <BOOST_INT128_DEFAULTED_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE constexpr int128_t& operator<<=(Integer rhs) noexcept;

    BOOST_INT128_HOST_DEVICE constexpr int128_t& operator<<=(int128_t rhs) noexcept;

    #ifdef BOOST_INT128_HAS_MSVC_INT128

    template <BOOST_INT128_DEFAULTED_128BIT_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE inline int128_t& operator<<=(Integer rhs) noexcept;

    #endif // BOOST_INT128_HAS_MSVC_INT128

    // Compound Right Shift
    template <BOOST_INT128_DEFAULTED_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE constexpr int128_t& operator>>=(Integer rhs) noexcept;

    BOOST_INT128_HOST_DEVICE constexpr int128_t& operator>>=(int128_t rhs) noexcept;

    #ifdef BOOST_INT128_HAS_MSVC_INT128

    template <BOOST_INT128_DEFAULTED_128BIT_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE inline int128_t& operator>>=(Integer rhs) noexcept;

    #endif // BOOST_INT128_HAS_MSVC_INT128

    // Prefix and postfix increment
    BOOST_INT128_HOST_DEVICE constexpr int128_t& operator++() noexcept;
    BOOST_INT128_HOST_DEVICE constexpr int128_t operator++(int) noexcept;

    // Prefix and postfix decrment
    BOOST_INT128_HOST_DEVICE constexpr int128_t& operator--() noexcept;
    BOOST_INT128_HOST_DEVICE constexpr int128_t operator--(int) noexcept;

    // Compound Addition
    template <BOOST_INT128_DEFAULTED_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE constexpr int128_t& operator+=(Integer rhs) noexcept;

    BOOST_INT128_HOST_DEVICE constexpr int128_t& operator+=(int128_t rhs) noexcept;

    #ifdef BOOST_INT128_HAS_MSVC_INT128

    template <BOOST_INT128_DEFAULTED_128BIT_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE inline int128_t& operator+=(Integer rhs) noexcept;

    #endif // BOOST_INT128_HAS_MSVC_INT128

    // Compound Subtraction
    template <BOOST_INT128_DEFAULTED_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE constexpr int128_t& operator-=(Integer rhs) noexcept;

    BOOST_INT128_HOST_DEVICE constexpr int128_t& operator-=(int128_t rhs) noexcept;

    #ifdef BOOST_INT128_HAS_MSVC_INT128

    template <BOOST_INT128_DEFAULTED_128BIT_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE inline int128_t& operator-=(Integer rhs) noexcept;

    #endif // BOOST_INT128_HAS_MSVC_INT128

    // Compound Multiplication
    template <BOOST_INT128_DEFAULTED_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE constexpr int128_t& operator*=(Integer rhs) noexcept;

    BOOST_INT128_HOST_DEVICE constexpr int128_t& operator*=(int128_t rhs) noexcept;

    #ifdef BOOST_INT128_HAS_MSVC_INT128

    template <BOOST_INT128_DEFAULTED_128BIT_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE inline int128_t& operator*=(Integer rhs) noexcept;

    #endif // BOOST_INT128_HAS_MSVC_INT128

    // Compound Division
    template <BOOST_INT128_DEFAULTED_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE constexpr int128_t& operator/=(Integer rhs) noexcept;

    BOOST_INT128_HOST_DEVICE constexpr int128_t& operator/=(int128_t rhs) noexcept;

    #ifdef BOOST_INT128_HAS_MSVC_INT128

    template <BOOST_INT128_DEFAULTED_128BIT_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE inline int128_t& operator/=(Integer rhs) noexcept;

    #endif // BOOST_INT128_HAS_MSVC_INT128

    // Compound Modulo
    template <BOOST_INT128_DEFAULTED_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE constexpr int128_t& operator%=(Integer rhs) noexcept;

    BOOST_INT128_HOST_DEVICE constexpr int128_t& operator%=(int128_t rhs) noexcept;

    #ifdef BOOST_INT128_HAS_MSVC_INT128

    template <BOOST_INT128_DEFAULTED_128BIT_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE inline int128_t& operator%=(Integer rhs) noexcept;

    #endif // BOOST_INT128_HAS_MSVC_INT128
};

//=====================================
// Absolute Value function
//=====================================

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr int128_t abs(int128_t value) noexcept
{
    if (value.high < 0)
    {
        value.low = ~value.low + 1U;
        value.high = static_cast<std::int64_t>(~static_cast<std::uint64_t>(value.high) + static_cast<std::uint64_t>(value.low == 0 ? 1 : 0));
    }
    
    return value;
}

//=====================================
// Float Conversion Operators
//=====================================

// The most correct way to do this would be std::ldexp(static_cast<T>(high), 64) + static_cast<T>(low);
// Since std::ldexp is not constexpr until C++23 we can work around this by multiplying the high word
// by 0xFFFFFFFF in order to generally replicate what ldexp is doing in the constexpr context.
// We also avoid pulling in <quadmath.h> for the __float128 case where we would need ldexpq

BOOST_INT128_HOST_DEVICE constexpr int128_t::operator float() const noexcept
{
    return static_cast<float>(high) * detail::offset_value_v<float> + static_cast<float>(low);
}

BOOST_INT128_HOST_DEVICE constexpr int128_t::operator double() const noexcept
{
    return static_cast<double>(high) * detail::offset_value_v<double> + static_cast<double>(low);
}

#if !defined(BOOST_INT128_HAS_GPU_SUPPORT)

constexpr int128_t::operator long double() const noexcept
{
    return static_cast<long double>(high) * detail::offset_value_v<long double> + static_cast<long double>(low);
}

#endif

//=====================================
// Float Construction
//=====================================

// Inverse of operator(Float).
// NaN -> 0;
// f >= 2^127 -> INT128_MAX;
// f < -2^127 -> INT128_MIN.
template <BOOST_INT128_FLOATING_POINT_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t::int128_t(Float f) noexcept
{
    constexpr Float two_32 {static_cast<Float>(UINT64_C(1) << 32)};
    constexpr Float two_64 {two_32 * two_32};
    constexpr Float two_127 {two_64 * static_cast<Float>(UINT64_C(1) << 63)};

    // NaN: leave default-initialized (zero). NaN compares false to everything,
    // so neither >= 0 nor <= 0 holds.
    if (!(f >= Float{0}) && !(f <= Float{0}))
    {
        return;
    }

    if (f >= two_127)
    {
        high = (std::numeric_limits<std::int64_t>::max)();
        low = UINT64_MAX;
        return;
    }

    if (f <= -two_127)
    {
        high = (std::numeric_limits<std::int64_t>::min)();
        low = UINT64_C(0);
        return;
    }

    const bool negative {f < Float{0}};
    const Float abs_f {negative ? -f : f};

    std::uint64_t h {static_cast<std::uint64_t>(abs_f / two_64)};
    const Float remainder {abs_f - static_cast<Float>(h) * two_64};
    std::uint64_t l {static_cast<std::uint64_t>(remainder)};

    if (negative)
    {
        // Two's complement negation of (h, l): new_l = -l (with wraparound),
        // new_h = ~h if a borrow occurred (l != 0), else ~h + 1.
        const bool low_was_zero {l == UINT64_C(0)};
        l = UINT64_C(0) - l;
        h = ~h + (low_was_zero ? UINT64_C(1) : UINT64_C(0));
    }

    high = static_cast<std::int64_t>(h);
    low = l;
}

//=====================================
// Unary Operators
//=====================================

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr int128_t operator+(const int128_t value) noexcept
{
    return value;
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr int128_t operator-(const int128_t value) noexcept
{
    return (value.low == 0) ? int128_t{static_cast<std::int64_t>(0ULL - static_cast<std::uint64_t>(value.high)), 0} :
                              int128_t{~value.high, ~value.low + 1};
}

//=====================================
// Equality Operators
//=====================================

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr bool operator==(const int128_t lhs, const bool rhs) noexcept
{
    return lhs.high == 0 && lhs.low == static_cast<std::uint64_t>(rhs);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr bool operator==(const bool lhs, const int128_t rhs) noexcept
{
    return rhs.high == 0 && rhs.low == static_cast<std::uint64_t>(lhs);
}

#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wsign-conversion"
#  pragma clang diagnostic ignored "-Wsign-compare"
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wsign-conversion"
#  pragma GCC diagnostic ignored "-Wsign-compare"
#endif

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr bool operator==(const int128_t lhs, const int128_t rhs) noexcept
{
    // x64 and ARM64 like the values in opposite directions

    #if defined(__aarch64__) || defined(_M_ARM64) || defined(__x86_64__) || defined(_M_X64) || defined(_M_IX86)

    return lhs.low == rhs.low && lhs.high == rhs.high;

    #else

    return lhs.high == rhs.high && lhs.low == rhs.low;

    #endif
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr bool operator==(const int128_t lhs, const SignedInteger rhs) noexcept
{
    return lhs.high == (rhs < 0 ? -1 : 0) && lhs.low == static_cast<std::uint64_t>(rhs);
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr bool operator==(const SignedInteger lhs, const int128_t rhs) noexcept
{
    return rhs.high == (lhs < 0 ? -1 : 0) && rhs.low == static_cast<std::uint64_t>(lhs);
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr bool operator==(const int128_t lhs, const UnsignedInteger rhs) noexcept
{
    return lhs.high == 0 && lhs.low == static_cast<std::uint64_t>(rhs);
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr bool operator==(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    return rhs.high == 0 && rhs.low == static_cast<std::uint64_t>(lhs);
}

#if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR bool operator==(const int128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs == static_cast<int128_t>(rhs);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR bool operator==(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    return static_cast<int128_t>(lhs) == rhs;
}

#endif // BOOST_INT128_HAS_INT128

//=====================================
// Inequality Operators
//=====================================

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr bool operator!=(const int128_t lhs, const int128_t rhs) noexcept
{
    // x64 and ARM64 like the values in opposite directions

    #if defined(__aarch64__) || defined(_M_ARM64) || defined(_M_X64) || defined(_M_IX86)

    return lhs.low != rhs.low || lhs.high != rhs.high;

    #elif defined(__x86_64__) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION) && defined(__GNUC__) && !defined(__clang__) && defined(BOOST_INT128_HAS_INT128)

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return lhs.high != rhs.high || lhs.low != rhs.low;
    }
    else
    {
        detail::builtin_i128 builtin_lhs {};
        detail::builtin_i128 builtin_rhs {};

        std::memcpy(&builtin_lhs, &lhs, sizeof(builtin_lhs));
        std::memcpy(&builtin_rhs, &rhs, sizeof(builtin_rhs));

        return builtin_lhs != builtin_rhs;
    }

    #else

    return lhs.high != rhs.high || lhs.low != rhs.low;

    #endif
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr bool operator!=(const int128_t lhs, const bool rhs) noexcept
{
    return lhs.high != 0 || lhs.low != static_cast<std::uint64_t>(rhs);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr bool operator!=(const bool lhs, const int128_t rhs) noexcept
{
    return rhs.high != 0 || rhs.low != static_cast<std::uint64_t>(lhs);
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr bool operator!=(const int128_t lhs, const SignedInteger rhs) noexcept
{
    return lhs.high != (rhs < 0 ? -1 : 0) || lhs.low != static_cast<std::uint64_t>(rhs);
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr bool operator!=(const SignedInteger lhs, const int128_t rhs) noexcept
{
    return rhs.high != (lhs < 0 ? -1 : 0) || rhs.low != static_cast<std::uint64_t>(lhs);
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr bool operator!=(const int128_t lhs, const UnsignedInteger rhs) noexcept
{
    return lhs.high != 0 || lhs.low != static_cast<std::uint64_t>(rhs);
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr bool operator!=(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    return rhs.high != 0 || rhs.low != static_cast<std::uint64_t>(lhs);
}

#if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR bool operator!=(const int128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs != static_cast<int128_t>(rhs);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR bool operator!=(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    return static_cast<int128_t>(lhs) != rhs;
}

#endif // BOOST_INT128_HAS_INT128

//=====================================
// Less than Operators
//=====================================

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr bool operator<(const int128_t lhs, const int128_t rhs) noexcept
{
    // On ARM macs only with the clang compiler is casting to __int128 uniformly better (and seemingly cost free)
    #if defined(__aarch64__) && defined(__APPLE__) && defined(__clang__) && defined(BOOST_INT128_HAS_INT128)

    return static_cast<detail::builtin_i128>(lhs) < static_cast<detail::builtin_i128>(rhs);

    #elif defined(__x86_64__) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION) && defined(__GNUC__) && !defined(__clang__) && defined(BOOST_INT128_HAS_INT128)

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return lhs.high == rhs.high ? lhs.low < rhs.low : lhs.high < rhs.high;
    }
    else
    {
        detail::builtin_i128 builtin_lhs {};
        detail::builtin_i128 builtin_rhs {};

        std::memcpy(&builtin_lhs, &lhs, sizeof(builtin_lhs));
        std::memcpy(&builtin_rhs, &rhs, sizeof(builtin_rhs));

        return builtin_lhs < builtin_rhs;
    }

    #else

    return lhs.high == rhs.high ? lhs.low < rhs.low : lhs.high < rhs.high;

    #endif
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr bool operator<(const int128_t lhs, const UnsignedInteger rhs) noexcept
{
    return lhs.high < 0 || (lhs.high == 0 && lhs.low < static_cast<std::uint64_t>(rhs));
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr bool operator<(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    return rhs.high > 0 || (rhs.high == 0 && static_cast<std::uint64_t>(lhs) < rhs.low);
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr bool operator<(const int128_t lhs, const SignedInteger rhs) noexcept
{
    if (lhs.high < 0)
    {
        return rhs >= 0 ? true : lhs < static_cast<int128_t>(rhs);
    }

    if (lhs.high > 0 || rhs < 0)
    {
        return false;
    }

    return lhs.low < static_cast<std::uint64_t>(rhs);
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr bool operator<(const SignedInteger lhs, const int128_t rhs) noexcept
{
    if (rhs.high < 0)
    {
        return lhs >= 0 ? false : static_cast<int128_t>(lhs) < rhs;
    }

    // rhs is positive
    if (rhs.high > 0 || lhs < 0)
    {
        return true;
    }

    return static_cast<std::uint64_t>(lhs) < rhs.low;
}

#if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR bool operator<(const int128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs < static_cast<int128_t>(rhs);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR bool operator<(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    return static_cast<int128_t>(lhs) < rhs;
}

#endif // BOOST_INT128_HAS_INT128

//=====================================
// Greater than Operators
//=====================================

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr bool operator>(const int128_t lhs, const int128_t rhs) noexcept
{
    // On ARM macs only with the clang compiler is casting to __int128 uniformly better (and seemingly cost free)
    #if defined(__aarch64__) && defined(__APPLE__) && defined(__clang__) && defined(BOOST_INT128_HAS_INT128)

    return static_cast<detail::builtin_i128>(lhs) > static_cast<detail::builtin_i128>(rhs);

    #elif defined(__x86_64__) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION) && defined(__GNUC__) && !defined(__clang__) && defined(BOOST_INT128_HAS_INT128)

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return lhs.high == rhs.high ? lhs.low > rhs.low : lhs.high > rhs.high;
    }
    else
    {
        detail::builtin_i128 builtin_lhs {};
        detail::builtin_i128 builtin_rhs {};

        std::memcpy(&builtin_lhs, &lhs, sizeof(builtin_lhs));
        std::memcpy(&builtin_rhs, &rhs, sizeof(builtin_rhs));

        return builtin_lhs > builtin_rhs;
    }

    #else

    return lhs.high == rhs.high ? lhs.low > rhs.low : lhs.high > rhs.high;

    #endif
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr bool operator>(const int128_t lhs, const SignedInteger rhs) noexcept
{
    return !(lhs < rhs) && !(lhs == rhs);
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr bool operator>(const SignedInteger lhs, const int128_t rhs) noexcept
{
    return !(lhs < rhs) && !(lhs == rhs);
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr bool operator>(const int128_t lhs, const UnsignedInteger rhs) noexcept
{
    return lhs.high > 0 || (lhs.high == 0 && lhs.low > static_cast<std::uint64_t>(rhs));
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr bool operator>(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    return rhs.high < 0 || (rhs.high == 0 && static_cast<std::uint64_t>(lhs) > rhs.low);
}

#if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR bool operator>(const int128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs > static_cast<int128_t>(rhs);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR bool operator>(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    return static_cast<int128_t>(lhs) > rhs;
}

#endif // BOOST_INT128_HAS_INT128

//=====================================
// Less Equal Operators
//=====================================

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr bool operator<=(const int128_t lhs, const int128_t rhs) noexcept
{
    // On ARM macs only with the clang compiler is casting to __int128 uniformly better (and seemingly cost free)
    #if defined(__aarch64__) && defined(__APPLE__) && defined(__clang__) && defined(BOOST_INT128_HAS_INT128)

    return static_cast<detail::builtin_i128>(lhs) <= static_cast<detail::builtin_i128>(rhs);

    #elif defined(__x86_64__) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION) && defined(__GNUC__) && !defined(__clang__) && defined(BOOST_INT128_HAS_INT128)

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return lhs.high == rhs.high ? lhs.low <= rhs.low : lhs.high <= rhs.high;
    }
    else
    {
        detail::builtin_i128 builtin_lhs {};
        detail::builtin_i128 builtin_rhs {};

        std::memcpy(&builtin_lhs, &lhs, sizeof(builtin_lhs));
        std::memcpy(&builtin_rhs, &rhs, sizeof(builtin_rhs));

        return builtin_lhs <= builtin_rhs;
    }

    #else

    return lhs.high == rhs.high ? lhs.low <= rhs.low : lhs.high <= rhs.high;

    #endif
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr bool operator<=(const int128_t lhs, const SignedInteger rhs) noexcept
{
    return !(lhs > rhs);
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr bool operator<=(const SignedInteger lhs, const int128_t rhs) noexcept
{
    return !(lhs > rhs);
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr bool operator<=(const int128_t lhs, const UnsignedInteger rhs) noexcept
{
    return lhs.high < 0 || (lhs.high == 0 && lhs.low <= static_cast<std::uint64_t>(rhs));
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr bool operator<=(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    return rhs.high > 0 || (rhs.high == 0 && static_cast<std::uint64_t>(lhs) <= rhs.low);
}

#if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR bool operator<=(const int128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs <= static_cast<int128_t>(rhs);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR bool operator<=(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    return static_cast<int128_t>(lhs) <= rhs;
}

#endif // BOOST_INT128_HAS_INT128

//=====================================
// Greater Equal Operators
//=====================================

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr bool operator>=(const int128_t lhs, const int128_t rhs) noexcept
{
    // On ARM macs only with the clang compiler is casting to __int128 uniformly better (and seemingly cost free)
    #if defined(__aarch64__) && defined(__APPLE__) && defined(__clang__) && defined(BOOST_INT128_HAS_INT128)

    return static_cast<detail::builtin_i128>(lhs) >= static_cast<detail::builtin_i128>(rhs);

    #elif defined(__x86_64__) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION) && defined(__GNUC__) && !defined(__clang__) && defined(BOOST_INT128_HAS_INT128)

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return lhs.high == rhs.high ? lhs.low >= rhs.low : lhs.high >= rhs.high;
    }
    else
    {
        detail::builtin_i128 builtin_lhs {};
        detail::builtin_i128 builtin_rhs {};

        std::memcpy(&builtin_lhs, &lhs, sizeof(builtin_lhs));
        std::memcpy(&builtin_rhs, &rhs, sizeof(builtin_rhs));

        return builtin_lhs >= builtin_rhs;
    }

    #else

    return lhs.high == rhs.high ? lhs.low >= rhs.low : lhs.high >= rhs.high;

    #endif
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr bool operator>=(const int128_t lhs, const SignedInteger rhs) noexcept
{
    return !(lhs < rhs);
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr bool operator>=(const SignedInteger lhs, const int128_t rhs) noexcept
{
    return !(lhs < rhs);
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr bool operator>=(const int128_t lhs, const UnsignedInteger rhs) noexcept
{
    return lhs.high > 0 || (lhs.high == 0 && lhs.low >= static_cast<std::uint64_t>(rhs));
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr bool operator>=(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    return rhs.high < 0 || (rhs.high == 0 && static_cast<std::uint64_t>(lhs) >= rhs.low);
}

#if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR bool operator>=(const int128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs >= static_cast<int128_t>(rhs);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR bool operator>=(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    return static_cast<int128_t>(lhs) >= rhs;
}

#endif // BOOST_INT128_HAS_INT128

//=====================================
// Spaceship Operator
//=====================================

#ifdef BOOST_INT128_HAS_SPACESHIP_OPERATOR

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr std::strong_ordering operator<=>(const int128_t lhs, const int128_t rhs) noexcept
{
    if (lhs < rhs)
    {
        return std::strong_ordering::less;
    }
    else if (lhs == rhs)
    {
        return std::strong_ordering::equivalent;
    }
    else
    {
        return std::strong_ordering::greater;
    }
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr std::strong_ordering operator<=>(const int128_t lhs, const SignedInteger rhs) noexcept
{
    if (lhs < rhs)
    {
        return std::strong_ordering::less;
    }
    else if (lhs == rhs)
    {
        return std::strong_ordering::equivalent;
    }
    else
    {
        return std::strong_ordering::greater;
    }
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr std::strong_ordering operator<=>(const SignedInteger lhs, const int128_t rhs) noexcept
{
    if (lhs < rhs)
    {
        return std::strong_ordering::less;
    }
    else if (lhs == rhs)
    {
        return std::strong_ordering::equivalent;
    }
    else
    {
        return std::strong_ordering::greater;
    }
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr std::strong_ordering operator<=>(const int128_t lhs, const UnsignedInteger rhs) noexcept
{
    if (lhs < rhs)
    {
        return std::strong_ordering::less;
    }
    else if (lhs == rhs)
    {
        return std::strong_ordering::equivalent;
    }
    else
    {
        return std::strong_ordering::greater;
    }
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr std::strong_ordering operator<=>(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    if (lhs < rhs)
    {
        return std::strong_ordering::less;
    }
    else if (lhs == rhs)
    {
        return std::strong_ordering::equivalent;
    }
    else
    {
        return std::strong_ordering::greater;
    }
}

#endif

//=====================================
// Not Operator
//=====================================

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr int128_t operator~(const int128_t rhs) noexcept
{
    return {~rhs.high, ~rhs.low};
}

//=====================================
// Or Operator
//=====================================

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr int128_t operator|(const int128_t lhs, const int128_t rhs) noexcept
{
    return {lhs.high | rhs.high, lhs.low | rhs.low};
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t operator|(const int128_t lhs, const SignedInteger rhs) noexcept
{
    return {lhs.high | (rhs < 0 ? -1 : 0), lhs.low | static_cast<std::uint64_t>(rhs)};
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t operator|(const SignedInteger lhs, const int128_t rhs) noexcept
{
    return {rhs.high | (lhs < 0 ? -1 : 0), static_cast<std::uint64_t>(lhs) | rhs.low};
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t operator|(const int128_t lhs, const UnsignedInteger rhs) noexcept
{
    return {lhs.high, lhs.low | static_cast<std::uint64_t>(rhs)};
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t operator|(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    return {rhs.high, static_cast<std::uint64_t>(lhs) | rhs.low};
}

#if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR int128_t operator|(const int128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs | static_cast<int128_t>(rhs);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR int128_t operator|(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    return static_cast<int128_t>(lhs) | rhs;
}


#endif // BOOST_INT128_HAS_INT128

//=====================================
// Compound OR Operator
//=====================================

template <BOOST_INT128_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t& int128_t::operator|=(const Integer rhs) noexcept
{
    *this = static_cast<int128_t>(*this | rhs);
    return *this;
}

BOOST_INT128_HOST_DEVICE constexpr int128_t& int128_t::operator|=(const int128_t rhs) noexcept
{
    *this = *this | rhs;
    return *this;
}

#ifdef BOOST_INT128_HAS_MSVC_INT128

template <BOOST_INT128_128BIT_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE inline int128_t& int128_t::operator|=(const Integer rhs) noexcept
{
    *this = static_cast<int128_t>(*this | rhs);
    return *this;
}

#endif // BOOST_INT128_HAS_MSVC_INT128

//=====================================
// And Operator
//=====================================

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr int128_t operator&(const int128_t lhs, const int128_t rhs) noexcept
{
    return {lhs.high & rhs.high, lhs.low & rhs.low};
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t operator&(const int128_t lhs, const SignedInteger rhs) noexcept
{
    return {lhs.high & (rhs < 0 ? -1 : 0), lhs.low & static_cast<std::uint64_t>(rhs)};
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t operator&(const SignedInteger lhs, const int128_t rhs) noexcept
{
    return {rhs.high & (lhs < 0 ? -1 : 0), static_cast<std::uint64_t>(lhs) & rhs.low};
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t operator&(const int128_t lhs, const UnsignedInteger rhs) noexcept
{
    return {0, lhs.low & static_cast<std::uint64_t>(rhs)};
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t operator&(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    return {0, static_cast<std::uint64_t>(lhs) & rhs.low};
}

#if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR int128_t operator&(const int128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs & static_cast<int128_t>(rhs);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR int128_t operator&(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    return static_cast<int128_t>(lhs) & rhs;
}


#endif // BOOST_INT128_HAS_INT128

#ifdef BOOST_INT128_HAS_MSVC_INT128

template <BOOST_INT128_128BIT_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE inline int128_t& int128_t::operator&=(const Integer rhs) noexcept
{
    *this = static_cast<int128_t>(*this & rhs);
    return *this;
}

#endif // BOOST_INT128_HAS_MSVC_INT128

//=====================================
// Compound And Operator
//=====================================

template <BOOST_INT128_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t& int128_t::operator&=(const Integer rhs) noexcept
{
    *this = static_cast<int128_t>(*this & rhs);
    return *this;
}

BOOST_INT128_HOST_DEVICE constexpr int128_t& int128_t::operator&=(const int128_t rhs) noexcept
{
    *this = *this & rhs;
    return *this;
}

//=====================================
// XOR Operator
//=====================================

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr int128_t operator^(const int128_t lhs, const int128_t rhs) noexcept
{
    return {lhs.high ^ rhs.high, lhs.low ^ rhs.low};
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t operator^(const int128_t lhs, const SignedInteger rhs) noexcept
{
    return {lhs.high ^ (rhs < 0 ? -1 : 0), lhs.low ^ static_cast<std::uint64_t>(rhs)};
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t operator^(const SignedInteger lhs, const int128_t rhs) noexcept
{
    return {rhs.high ^ (lhs < 0 ? -1 : 0), static_cast<std::uint64_t>(lhs) ^ rhs.low};
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t operator^(const int128_t lhs, const UnsignedInteger rhs) noexcept
{
    return {lhs.high, lhs.low ^ static_cast<std::uint64_t>(rhs)};
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t operator^(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    return {rhs.high, static_cast<std::uint64_t>(lhs) ^ rhs.low};
}

#if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR int128_t operator^(const int128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs ^ static_cast<int128_t>(rhs);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR int128_t operator^(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    return static_cast<int128_t>(lhs) ^ rhs;
}


#endif // BOOST_INT128_HAS_INT128

//=====================================
// Compound XOR Operator
//=====================================

template <BOOST_INT128_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t& int128_t::operator^=(Integer rhs) noexcept
{
    *this = static_cast<int128_t>(*this ^ rhs);
    return *this;
}

BOOST_INT128_HOST_DEVICE constexpr int128_t& int128_t::operator^=(int128_t rhs) noexcept
{
    *this = *this ^ rhs;
    return *this;
}

#ifdef BOOST_INT128_HAS_MSVC_INT128

template <BOOST_INT128_128BIT_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE inline int128_t& int128_t::operator^=(const Integer rhs) noexcept
{
    *this = static_cast<int128_t>(*this ^ rhs);
    return *this;
}

#endif // BOOST_INT128_HAS_MSVC_INT128

//=====================================
// Left Shift Operator
//=====================================

namespace detail {

template <typename Integer>
BOOST_INT128_HOST_DEVICE constexpr int128_t default_ls_impl(const int128_t lhs, const Integer rhs) noexcept
{
    static_assert(std::is_integral<Integer>::value, "Only builtin types allowed");

    BOOST_INT128_IF_CONSTEXPR (std::numeric_limits<Integer>::is_signed)
    {
        if (rhs < 0 || rhs >= 128)
        {
            return {0, 0};
        }
    }
    else
    {
        if (rhs >= 128)
        {
            return {0, 0};
        }
    }

    if (rhs == 0)
    {
        return lhs;
    }

    if (rhs == 64)
    {
        return {static_cast<std::int64_t>(lhs.low), 0};
    }

    if (rhs > 64)
    {
        return {static_cast<std::int64_t>(lhs.low << (rhs - 64)), 0};
    }

    // For shifts < 64
    std::uint64_t high_part = (static_cast<std::uint64_t>(lhs.high) << rhs) |
                              (lhs.low >> (64 - rhs));

    return {
        static_cast<std::int64_t>(high_part),
        lhs.low << rhs
    };
}

template <typename Integer>
BOOST_INT128_HOST_DEVICE int128_t intrinsic_ls_impl(const int128_t lhs, const Integer rhs) noexcept
{
    BOOST_INT128_IF_CONSTEXPR (std::numeric_limits<Integer>::is_signed)
    {
        if (BOOST_INT128_UNLIKELY(rhs >= 128 || rhs < 0))
        {
            return {0, 0};
        }
    }
    else
    {
        if (BOOST_INT128_UNLIKELY(rhs >= 128))
        {
            return {0, 0};
        }
    }

    #ifdef BOOST_INT128_HAS_INT128

    // Left-shifting a negative builtin_i128 is UB pre-C++20
    #  if defined(__aarch64__)

    #if defined(__GNUC__) && __GNUC__ >= 8
    #  pragma GCC diagnostic push
    #  pragma GCC diagnostic ignored "-Wclass-memaccess"
    #endif

    builtin_u128 value;
    std::memcpy(&value, &lhs, sizeof(builtin_u128));
    const auto res {value << rhs};

    int128_t return_value;
    std::memcpy(&return_value, &res, sizeof(int128_t));
    return return_value;

    #if defined(__GNUC__) && __GNUC__ >= 8
    #  pragma GCC diagnostic pop
    #endif

    #  else

    return int128_t{static_cast<builtin_u128>(lhs) << rhs};

    #  endif

    #elif defined(_M_AMD64)

    if (rhs >= 64)
    {
        return {static_cast<std::int64_t>(lhs.low << (rhs - 64)), 0};
    }
    else
    {
        int128_t res;
        res.high = static_cast<std::int64_t>(__shiftleft128(lhs.low, static_cast<std::uint64_t>(lhs.high), static_cast<unsigned char>(rhs)));
        res.low = lhs.low << rhs;

        return res;
    }

    #else

    if (BOOST_INT128_UNLIKELY(rhs == 0))
    {
        return lhs;
    }
    if (rhs == 64)
    {
        return {static_cast<std::int64_t>(lhs.low), 0};
    }

    if (rhs > 64)
    {
        return {static_cast<std::int64_t>(lhs.low << (rhs - 64)), 0};
    }

    // For shifts < 64
    const auto high_part = (static_cast<std::uint64_t>(lhs.high) << rhs) |
                           (lhs.low >> (64 - rhs));

    return {
        static_cast<std::int64_t>(high_part),
        lhs.low << rhs
    };

    #endif
}

} // namespace detail

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t operator<<(const int128_t lhs, const Integer rhs) noexcept
{
    #ifndef BOOST_INT128_NO_CONSTEVAL_DETECTION

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return detail::default_ls_impl(lhs, rhs); // LCOV_EXCL_LINE
    }
    else
    {
        return detail::intrinsic_ls_impl(lhs, rhs);
    }

    #else

    return detail::default_ls_impl(lhs, rhs);

    #endif
}

BOOST_INT128_HOST_DEVICE constexpr int128_t operator<<(const int128_t lhs, const int128_t rhs) noexcept
{
    if (rhs.high != 0 || rhs.low >= 128)
    {
        return 0;
    }

    return lhs << rhs.low;
}

#if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR detail::builtin_u128 operator<<(const detail::builtin_u128 lhs, const int128_t rhs) noexcept
{
    constexpr auto bit_width {sizeof(detail::builtin_u128) * 8};

    if (rhs.high != 0 || rhs.low >= bit_width)
    {
        return 0;
    }
    
    return lhs << static_cast<detail::builtin_u128>(rhs.low);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR detail::builtin_i128 operator<<(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    constexpr auto bit_width {sizeof(detail::builtin_i128) * 8};

    if (rhs.high != 0 || rhs.low >= bit_width)
    {
        return 0;
    }

    return lhs << static_cast<detail::builtin_u128>(rhs.low);
}

#endif

BOOST_INT128_EXPORT template <typename SignedInteger, std::enable_if_t<detail::is_signed_integer_v<SignedInteger> && (sizeof(SignedInteger) * 8 <= 16), bool> = true>
BOOST_INT128_HOST_DEVICE constexpr int operator<<(const SignedInteger lhs, const int128_t rhs) noexcept
{
    constexpr auto bit_width {sizeof(SignedInteger) * 8};

    if (rhs.high != 0 || rhs.low >= bit_width)
    {
        return 0;
    }

    return static_cast<int>(lhs) << rhs.low;
}

BOOST_INT128_EXPORT template <typename UnsignedInteger, std::enable_if_t<detail::is_unsigned_integer_v<UnsignedInteger> && (sizeof(UnsignedInteger) * 8 <= 16), bool> = true>
BOOST_INT128_HOST_DEVICE constexpr unsigned operator<<(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    constexpr auto bit_width {sizeof(UnsignedInteger) * 8};

    if (rhs.high != 0 || rhs.low >= bit_width)
    {
        return 0;
    }

    return static_cast<unsigned>(lhs) << rhs.low;
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4804) // Unsafe use of type bool in operation
#endif // _MSC_VER

template <BOOST_INT128_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t& int128_t::operator<<=(const Integer rhs) noexcept
{
    *this = static_cast<int128_t>(*this << rhs);
    return *this;
}

BOOST_INT128_HOST_DEVICE constexpr int128_t& int128_t::operator<<=(const int128_t rhs) noexcept
{
    *this = *this << rhs;
    return *this;
}

#ifdef BOOST_INT128_HAS_MSVC_INT128

template <BOOST_INT128_128BIT_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE inline int128_t& int128_t::operator<<=(const Integer rhs) noexcept
{
    *this = static_cast<int128_t>(*this << rhs);
    return *this;
}

#endif // BOOST_INT128_HAS_MSVC_INT128

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

//=====================================
// Right Shift Operator
//=====================================

namespace detail {

template <typename Integer>
BOOST_INT128_HOST_DEVICE constexpr int128_t default_rs_impl(const int128_t lhs, const Integer rhs) noexcept
{
    BOOST_INT128_IF_CONSTEXPR (std::numeric_limits<Integer>::is_signed)
    {
        if (rhs >= 128 || rhs < 0)
        {
            return lhs.high < 0 ? int128_t{-1, UINT64_MAX} : int128_t{0, 0};
        }
    }
    else
    {
        if (rhs >= 128)
        {
            return lhs.high < 0 ? int128_t{-1, UINT64_MAX} : int128_t{0, 0};
        }
    }

    if (rhs == 0)
    {
        return lhs;
    }

    if (rhs >= 64)
    {
        return {lhs.high < 0 ? -1 : 0, static_cast<std::uint64_t>(lhs.high >> (rhs - 64))};
    }

    // For shifts < 64
    const auto high_to_low {static_cast<std::uint64_t>(lhs.high) << (64 - rhs)};
    const auto low_shifted {lhs.low >> rhs};
    const auto low_part {high_to_low | low_shifted};

    return {
        lhs.high >> rhs,
        low_part
    };
}

template <typename Integer>
BOOST_INT128_HOST_DEVICE int128_t intrinsic_rs_impl(const int128_t lhs, const Integer rhs) noexcept
{
    BOOST_INT128_IF_CONSTEXPR (std::numeric_limits<Integer>::is_signed)
    {
        if (rhs >= 128 || rhs < 0)
        {
            return lhs.high < 0 ? int128_t{-1, UINT64_MAX} : int128_t{0, 0};
        }
    }
    else
    {
        if (rhs >= 128)
        {
            return lhs.high < 0 ? int128_t{-1, UINT64_MAX} : int128_t{0, 0};
        }
    }

    #ifdef BOOST_INT128_HAS_INT128

    #  if defined(__aarch64__)

    #if defined(__GNUC__) && __GNUC__ >= 8
    #  pragma GCC diagnostic push
    #  pragma GCC diagnostic ignored "-Wclass-memaccess"
    #endif

    builtin_i128 value;
    std::memcpy(&value, &lhs, sizeof(builtin_i128));
    const auto res {value >> rhs};

    int128_t return_value;
    std::memcpy(&return_value, &res, sizeof(int128_t));
    return return_value;

    #if defined(__GNUC__) && __GNUC__ >= 8
    #  pragma GCC diagnostic pop
    #endif

    #  else

    return static_cast<builtin_i128>(lhs) >> rhs;

    #  endif

    #elif defined(_M_AMD64)

    if (rhs >= 64)
    {
        return {lhs.high < 0 ? -1 : 0, static_cast<std::uint64_t>(lhs.high >> (rhs - 64))};
    }
    else
    {
        int128_t res;
        res.low = __shiftright128(lhs.low, static_cast<std::uint64_t>(lhs.high), static_cast<unsigned char>(rhs));
        res.high = lhs.high >> rhs;

        return res;
    }

    #else

    if (BOOST_INT128_UNLIKELY(rhs == 0))
    {
        return lhs;
    }

    if (rhs >= 64)
    {
        return {lhs.high < 0 ? -1 : 0, static_cast<std::uint64_t>(lhs.high >> (rhs - 64))};
    }

    // For shifts < 64
    const auto high_to_low {static_cast<std::uint64_t>(lhs.high) << (64 - rhs)};
    const auto low_shifted {lhs.low >> rhs};
    const auto low_part {high_to_low | low_shifted};

    return {
        lhs.high >> rhs,
        low_part
    };

    #endif
}

} // namespace detail

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t operator>>(const int128_t lhs, const Integer rhs) noexcept
{
    #ifndef BOOST_INT128_NO_CONSTEVAL_DETECTION

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return detail::default_rs_impl(lhs, rhs); // LCOV_EXCL_LINE
    }
    else
    {
        return detail::intrinsic_rs_impl(lhs, rhs);
    }

    #else

    return detail::default_rs_impl(lhs, rhs);

    #endif
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr int128_t operator>>(const int128_t lhs, const int128_t rhs) noexcept
{
    if (rhs.high != 0 || rhs.low >= 128)
    {
        return 0;
    }

    return lhs >> rhs.low;
}

#if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR detail::builtin_u128 operator>>(const detail::builtin_u128 lhs, const int128_t rhs) noexcept
{
    constexpr auto bit_width {sizeof(detail::builtin_u128) * 8};

    if (rhs.high != 0 || rhs.low >= bit_width)
    {
        return 0;
    }

    return lhs >> static_cast<detail::builtin_u128>(rhs.low);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR detail::builtin_i128 operator>>(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    constexpr auto bit_width {sizeof(detail::builtin_i128) * 8};

    if (rhs.high != 0 || rhs.low >= bit_width)
    {
        return 0;
    }

    return lhs >> static_cast<detail::builtin_u128>(rhs.low);
}

#endif

BOOST_INT128_EXPORT template <typename SignedInteger, std::enable_if_t<detail::is_signed_integer_v<SignedInteger> && (sizeof(SignedInteger) * 8 <= 16), bool> = true>
BOOST_INT128_HOST_DEVICE constexpr int operator>>(const SignedInteger lhs, const int128_t rhs) noexcept
{
    constexpr auto bit_width {sizeof(SignedInteger) * 8};

    if (rhs.high != 0 || rhs.low >= bit_width)
    {
        return 0;
    }

    return static_cast<int>(lhs) >> rhs.low;
}

BOOST_INT128_EXPORT template <typename UnsignedInteger, std::enable_if_t<detail::is_unsigned_integer_v<UnsignedInteger> && (sizeof(UnsignedInteger) * 8 <= 16), bool> = true>
BOOST_INT128_HOST_DEVICE constexpr unsigned operator>>(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    constexpr auto bit_width {sizeof(UnsignedInteger) * 8};

    if (rhs.high != 0 || rhs.low >= bit_width)
    {
        return 0;
    }

    return static_cast<unsigned>(lhs) >> rhs.low;
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4804) // Unsafe use of type bool in operation
#endif // _MSC_VER

template <BOOST_INT128_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t& int128_t::operator>>=(const Integer rhs) noexcept
{
    *this = static_cast<int128_t>(*this >> rhs);
    return *this;
}

BOOST_INT128_HOST_DEVICE constexpr int128_t& int128_t::operator>>=(const int128_t rhs) noexcept
{
    *this = *this >> rhs;
    return *this;
}

#ifdef BOOST_INT128_HAS_MSVC_INT128

template <BOOST_INT128_128BIT_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE inline int128_t& int128_t::operator>>=(const Integer rhs) noexcept
{
    *this = static_cast<int128_t>(*this >> rhs);
    return *this;
}

#endif // BOOST_INT128_HAS_MSVC_INT128

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

//=====================================
// Increment Operators
//=====================================

BOOST_INT128_HOST_DEVICE constexpr int128_t& int128_t::operator++() noexcept
{
    if (++low == UINT64_C(0))
    {
        ++high;
    }

    return *this;
}

BOOST_INT128_HOST_DEVICE constexpr int128_t int128_t::operator++(int) noexcept
{
    const auto temp {*this};
    ++(*this);
    return temp;
}

//=====================================
// Decrement Operators
//=====================================

BOOST_INT128_HOST_DEVICE constexpr int128_t& int128_t::operator--() noexcept
{
    if (low-- == UINT64_C(0))
    {
        --high;
    }

    return *this;
}

BOOST_INT128_HOST_DEVICE constexpr int128_t int128_t::operator--(int) noexcept
{
    const auto temp {*this};
    --(*this);
    return temp;
}

//=====================================
// Addition Operators
//=====================================

namespace detail {

BOOST_INT128_HOST_DEVICE BOOST_INT128_FORCE_INLINE constexpr int128_t library_add(const int128_t lhs, const int128_t rhs) noexcept
{
    const auto new_low {lhs.low + rhs.low};
    const auto new_high {static_cast<std::uint64_t>(lhs.high) +
                                        static_cast<std::uint64_t>(rhs.high) +
                                        static_cast<std::uint64_t>(new_low < lhs.low)};

    return int128_t{static_cast<std::int64_t>(new_high), new_low};
}

BOOST_INT128_HOST_DEVICE BOOST_INT128_FORCE_INLINE constexpr int128_t default_add(const int128_t lhs, const int128_t rhs) noexcept
{
    #if (defined(__x86_64__) || (defined(__aarch64__) && !defined(__APPLE__))) && !defined(_WIN32) && defined(BOOST_INT128_HAS_INT128)

    // Compute in the unsigned domain so that overflow wraps modulo 2^128
    return int128_t{static_cast<detail::builtin_u128>(lhs) + static_cast<detail::builtin_u128>(rhs)};

    #elif defined(BOOST_INT128_HAS_BUILTIN_ADD_OVERFLOW)

    std::uint64_t result_low {};
    std::uint64_t result_high {};

    result_high = static_cast<std::uint64_t>(lhs.high) + static_cast<std::uint64_t>(rhs.high) + __builtin_add_overflow(lhs.low, rhs.low, &result_low);

    return int128_t{static_cast<std::int64_t>(result_high), result_low};

    #elif defined(_M_AMD64) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION)

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return library_add(lhs, rhs); // LCOV_EXCL_LINE
    }
    else
    {
        int128_t result {};
        const auto carry {BOOST_INT128_ADD_CARRY(0, lhs.low, rhs.low, &result.low)};
        BOOST_INT128_ADD_CARRY(carry, static_cast<std::uint64_t>(lhs.high), static_cast<std::uint64_t>(rhs.high), reinterpret_cast<std::uint64_t*>(&result.high));

        return result;
    }

    #else

    return library_add(lhs, rhs);

    #endif
}

template <BOOST_INT128_DEFAULTED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE BOOST_INT128_FORCE_INLINE constexpr int128_t default_add(const int128_t lhs, const Integer rhs) noexcept
{
    const auto new_low {lhs.low + rhs};
    const auto new_high {static_cast<std::uint64_t>(lhs.high) + static_cast<std::uint64_t>(new_low < lhs.low)};

    return int128_t{static_cast<std::int64_t>(new_high), new_low};
}

BOOST_INT128_HOST_DEVICE BOOST_INT128_FORCE_INLINE constexpr int128_t library_sub(const int128_t lhs, const int128_t rhs) noexcept
{
    const auto new_low {lhs.low - rhs.low};
    const auto new_high {static_cast<std::uint64_t>(lhs.high) - static_cast<std::uint64_t>(rhs.high) - static_cast<std::uint64_t>(lhs.low < rhs.low)};

    return int128_t{static_cast<std::int64_t>(new_high), new_low};
}

BOOST_INT128_HOST_DEVICE BOOST_INT128_FORCE_INLINE constexpr int128_t default_sub(const int128_t lhs, const int128_t rhs) noexcept
{
    #if defined(BOOST_INT128_HAS_BUILTIN_SUB_OVERFLOW) && (!defined(__aarch64__) || defined(__APPLE__) || !defined(BOOST_INT128_HAS_INT128)) && !(defined(__CUDACC__) && defined(BOOST_INT128_ENABLE_CUDA))

    // __builtin_sub_overflow is marked constexpr so we don't need if consteval handling
    std::uint64_t result_low {};
    const auto result_high {static_cast<std::uint64_t>(lhs.high) - static_cast<std::uint64_t>(rhs.high) - static_cast<std::uint64_t>(__builtin_sub_overflow(lhs.low, rhs.low, &result_low))};

    return int128_t{static_cast<std::int64_t>(result_high), result_low};

    #elif defined(__aarch64__) && !defined(__APPLE__)

    // Unsigned wrap for consistent two's-complement semantics
    return int128_t{static_cast<detail::builtin_u128>(lhs) - static_cast<detail::builtin_u128>(rhs)};

    #elif defined(_M_AMD64) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION)

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return library_sub(lhs, rhs); // LCOV_EXCL_LINE
    }
    else
    {
        int128_t result {};
        const auto borrow {BOOST_INT128_SUB_BORROW(0, lhs.low, rhs.low, &result.low)};
        BOOST_INT128_SUB_BORROW(borrow, static_cast<std::uint64_t>(lhs.high), static_cast<std::uint64_t>(rhs.high), reinterpret_cast<std::uint64_t*>(&result.high));

        return result;
    }

    #else

    return library_sub(lhs, rhs);

    #endif
}

template <BOOST_INT128_DEFAULTED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE BOOST_INT128_FORCE_INLINE constexpr int128_t default_sub(const int128_t lhs, const Integer rhs) noexcept
{
    const auto new_low {lhs.low - rhs};
    const auto new_high {static_cast<std::uint64_t>(lhs.high) - static_cast<std::uint64_t>(new_low > lhs.low)};
    return int128_t{static_cast<std::int64_t>(new_high), new_low};
}

}

// On s390x with multiple different versions of GCC and language standards
// doing addition via subtraction is >10% faster in the benchmarks
#if defined(__s390__) || defined(__s390x__)

BOOST_INT128_HOST_DEVICE constexpr int128_t operator+(const int128_t lhs, const int128_t rhs) noexcept
{
    return detail::default_sub(lhs, -rhs);
}

#else

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr int128_t operator+(const int128_t lhs, const int128_t rhs) noexcept
{
    return detail::default_add(lhs, rhs);
}

#endif

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t operator+(const int128_t lhs, const UnsignedInteger rhs) noexcept
{
    return detail::default_add(lhs, rhs);
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t operator+(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    return detail::default_add(rhs, lhs);
}

#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable : 4146) // Unary minus applied to unsigned type
#endif

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t operator+(const int128_t lhs, const SignedInteger rhs) noexcept
{
    // Negate in the unsigned domain so INT64_MIN does not overflow (UBSAN)
    return rhs < 0 ? detail::default_sub(lhs, -static_cast<std::uint64_t>(rhs)) :
                     detail::default_add(lhs, static_cast<std::uint64_t>(rhs));
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t operator+(const SignedInteger lhs, const int128_t rhs) noexcept
{
    return lhs < 0 ? detail::default_sub(rhs, -static_cast<std::uint64_t>(lhs)) :
                     detail::default_add(rhs, static_cast<std::uint64_t>(lhs));
}

#ifdef _MSC_VER
#  pragma warning(pop)
#endif

#if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)


BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR int128_t operator+(const int128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return detail::default_add(lhs, static_cast<int128_t>(rhs));
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR int128_t operator+(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    return detail::default_add(rhs, static_cast<int128_t>(lhs));
}

#endif // BOOST_INT128_HAS_INT128

template <BOOST_INT128_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t& int128_t::operator+=(const Integer rhs) noexcept
{
    *this = static_cast<int128_t>(*this + rhs);
    return *this;
}

BOOST_INT128_HOST_DEVICE constexpr int128_t& int128_t::operator+=(const int128_t rhs) noexcept
{
    *this = *this + rhs;
    return *this;
}

#ifdef BOOST_INT128_HAS_MSVC_INT128

template <BOOST_INT128_128BIT_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE inline int128_t& int128_t::operator+=(const Integer rhs) noexcept
{
    *this = static_cast<int128_t>(*this + rhs);
    return *this;
}

#endif // BOOST_INT128_HAS_MSVC_INT128

//=====================================
// Subtraction Operators
//=====================================

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr int128_t operator-(const int128_t lhs, const int128_t rhs) noexcept
{
    return detail::default_sub(lhs, rhs);
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t operator-(const int128_t lhs, const UnsignedInteger rhs) noexcept
{
    return detail::default_sub(lhs, rhs);
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t operator-(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    return detail::default_add(-rhs, lhs);
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t operator-(const int128_t lhs, const SignedInteger rhs) noexcept
{
    return detail::default_sub(lhs, static_cast<int128_t>(rhs));
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t operator-(const SignedInteger lhs, const int128_t rhs) noexcept
{
    return detail::default_sub(static_cast<int128_t>(lhs), rhs);
}

#if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)


BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR int128_t operator-(const int128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs - static_cast<int128_t>(rhs);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR int128_t operator-(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    return static_cast<int128_t>(lhs) - rhs;
}

#endif

template <BOOST_INT128_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t& int128_t::operator-=(const Integer rhs) noexcept
{
    *this = static_cast<int128_t>(*this - rhs);
    return *this;
}

BOOST_INT128_HOST_DEVICE constexpr int128_t& int128_t::operator-=(const int128_t rhs) noexcept
{
    *this = *this - rhs;
    return *this;
}

#ifdef BOOST_INT128_HAS_MSVC_INT128

template <BOOST_INT128_128BIT_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE inline int128_t& int128_t::operator-=(const Integer rhs) noexcept
{
    *this = static_cast<int128_t>(*this - rhs);
    return *this;
}

#endif // BOOST_INT128_HAS_MSVC_INT128

//=====================================
// Multiplication Operators
//=====================================

namespace detail {

BOOST_INT128_HOST_DEVICE BOOST_INT128_FORCE_INLINE constexpr int128_t default_mul(const int128_t lhs, const std::uint64_t rhs) noexcept
{
    return low_word_mul<int128_t>(lhs, rhs);
}

BOOST_INT128_HOST_DEVICE BOOST_INT128_FORCE_INLINE constexpr int128_t default_mul(const int128_t lhs, const std::uint32_t rhs) noexcept
{
    return low_word_mul<int128_t>(lhs, rhs);
}

#if defined(_M_AMD64) && !defined(__GNUC__)

BOOST_INT128_HOST_DEVICE BOOST_INT128_FORCE_INLINE int128_t msvc_amd64_mul(const int128_t lhs, const int128_t rhs) noexcept
{
    int128_t result {};
    result.low = _umul128(lhs.low, rhs.low, reinterpret_cast<std::uint64_t*>(&result.high));
    result.high += lhs.low * rhs.high;
    result.high += lhs.high * rhs.low;

    return result;
}

#endif

BOOST_INT128_HOST_DEVICE BOOST_INT128_FORCE_INLINE constexpr int128_t default_mul(const int128_t lhs, const int128_t rhs) noexcept
{
    #if ((defined(__aarch64__) && defined(__APPLE__)) || defined(__x86_64__) || defined(__PPC__) || defined(__powerpc__)) && defined(__GNUC__) && !defined(__clang__) && defined(BOOST_INT128_HAS_INT128)

    #  if !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION)

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return low_word_mul<int128_t>(lhs, rhs);
    }
    else
    {
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wclass-memaccess"

        detail::builtin_u128 new_lhs {};
        detail::builtin_u128 new_rhs {};

        std::memcpy(&new_lhs, &lhs, sizeof(detail::builtin_u128));
        std::memcpy(&new_rhs, &rhs, sizeof(detail::builtin_u128));

        const auto res {new_lhs * new_rhs};
        int128_t library_res {};

        std::memcpy(&library_res, &res, sizeof(detail::builtin_u128));

        return library_res;

        #pragma GCC diagnostic pop
    }

    #  elif defined(BOOST_INT128_HAS_INT128)

    // Unsigned wrap for consistent two's-complement semantics
    return int128_t{static_cast<detail::builtin_u128>(lhs) * static_cast<detail::builtin_u128>(rhs)};

    #  else

    return low_word_mul<int128_t>(lhs, rhs);

    #  endif

    #elif defined(__aarch64__) && defined(BOOST_INT128_HAS_INT128)

    return int128_t{static_cast<detail::builtin_u128>(lhs) * static_cast<detail::builtin_u128>(rhs)};

    #elif defined(_M_AMD64) && !defined(__GNUC__) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION)

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(rhs))
    {
        return low_word_mul<int128_t>(lhs, rhs); // LCOV_EXCL_LINE
    }
    else
    {
        return msvc_amd64_mul(lhs, rhs);
    }

    #else

    return low_word_mul<int128_t>(lhs, rhs);

    #endif
}

} // namespace detail

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr int128_t operator*(const int128_t lhs, const int128_t rhs) noexcept
{
    return detail::default_mul(lhs, rhs);
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t operator*(const int128_t lhs, const UnsignedInteger rhs) noexcept
{
    using local_eval_type = detail::evaluation_type_t<UnsignedInteger>;
    return detail::default_mul(lhs, static_cast<local_eval_type>(rhs));
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t operator*(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    using local_eval_type = detail::evaluation_type_t<UnsignedInteger>;
    return detail::default_mul(rhs, static_cast<local_eval_type>(lhs));
}

#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable : 4146) // Unary minus applied to unsigned
#endif

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t operator*(const int128_t lhs, const SignedInteger rhs) noexcept
{
    return rhs < 0 ? -detail::default_mul(lhs, -static_cast<std::uint64_t>(rhs)) :
                      detail::default_mul(lhs, static_cast<std::uint64_t>(rhs));
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t operator*(const SignedInteger lhs, const int128_t rhs) noexcept
{
    return lhs < 0 ? -detail::default_mul(rhs, -static_cast<std::uint64_t>(lhs)) :
                      detail::default_mul(rhs, static_cast<std::uint64_t>(lhs));
}

#ifdef _MSC_VER
#  pragma warning(pop)
#endif

#if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR int128_t operator*(const int128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return detail::default_mul(lhs, static_cast<int128_t>(rhs));
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR int128_t operator*(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    return detail::default_mul(rhs, static_cast<int128_t>(lhs));
}

#endif // BOOST_INT128_HAS_INT128

template <BOOST_INT128_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t& int128_t::operator*=(const Integer rhs) noexcept
{
    *this = static_cast<int128_t>(*this * rhs);
    return *this;
}

BOOST_INT128_HOST_DEVICE constexpr int128_t& int128_t::operator*=(const int128_t rhs) noexcept
{
    *this = *this * rhs;
    return *this;
}

#ifdef BOOST_INT128_HAS_MSVC_INT128

template <BOOST_INT128_128BIT_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE inline int128_t& int128_t::operator*=(const Integer rhs) noexcept
{
    *this = static_cast<int128_t>(*this * rhs);
    return *this;
}

#endif // BOOST_INT128_HAS_MSVC_INT128

//=====================================
// Division Operator
//=====================================

#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wassume"
#endif

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr int128_t operator/(const int128_t lhs, const int128_t rhs) noexcept
{
    if (BOOST_INT128_UNLIKELY(rhs == 0))
    {
        return {0, 0};
    }

    constexpr int128_t min_val {INT64_MIN, 0};
    const auto abs_lhs {abs(lhs)};
    const auto abs_rhs {abs(rhs)};

    if (lhs != min_val && abs_lhs < abs_rhs)
    {
        return {0,0};
    }

    const auto negative_res {(lhs.high < 0) != (rhs.high < 0)};

    // Narrow fast path: when the divisor magnitude fits in 64 bits, divide the magnitudes with
    // the hardware-accelerated one_word_div and reapply the sign. This reuses the abs values
    // computed above and beats native signed division (the out-of-line __divti3) for this case.
    if (abs_rhs.high == 0)
    {
        int128_t quotient {};

        if (abs_lhs.high == 0)
        {
            quotient = {0, abs_lhs.low / abs_rhs.low};
        }
        else
        {
            detail::one_word_div(abs_lhs, abs_rhs.low, quotient);
        }

        return negative_res ? -quotient : quotient;
    }

    #if defined(BOOST_INT128_HAS_INT128)

    return static_cast<int128_t>(static_cast<detail::builtin_i128>(lhs) / static_cast<detail::builtin_i128>(rhs));

    #else

    const auto quotient {detail::knuth_div(abs_lhs, abs_rhs)};
    return negative_res ? -quotient : quotient;

    #endif
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t operator/(const int128_t lhs, const UnsignedInteger rhs) noexcept
{
    using eval_type = detail::evaluation_type_t<UnsignedInteger>;

    if (BOOST_INT128_UNLIKELY(rhs == 0))
    {
        return {0, 0};
    }

    const auto abs_lhs {abs(lhs)};

    int128_t quotient {};
    detail::one_word_div(abs_lhs, static_cast<eval_type>(rhs), quotient);
    return lhs < 0 ? -quotient : quotient;
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t operator/(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    if (BOOST_INT128_UNLIKELY(rhs == 0))
    {
        return {0, 0};
    }

    if (rhs.high != 0 && rhs.high != -1)
    {
        return {0,0};
    }
    else
    {
        auto abs_rhs {abs(rhs)};
        // rhs == -2^64 has |rhs| greater than any 64-bit lhs, so the quotient is 0 (also avoids /0)
        if (abs_rhs.high != 0)
        {
            return {0, 0};
        }
        const auto res {static_cast<std::uint64_t>(lhs) / abs_rhs.low};
        const int128_t result {0, res};
        return rhs < 0 ? -result : result;
    }
}

#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable : 4146) // Unary minus applied to unsigned type
#endif

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t operator/(const int128_t lhs, const SignedInteger rhs) noexcept
{
    using eval_type = detail::evaluation_type_t<SignedInteger>;

    if (BOOST_INT128_UNLIKELY(rhs == 0))
    {
        return {0, 0};
    }

    int128_t quotient {};

    constexpr int128_t min_val {INT64_MIN, 0};
    const auto negative_res {static_cast<bool>((lhs.high < 0) ^ (rhs < 0))};
    // Negate in the unsigned domain so INT64_MIN does not overflow (UBSAN)
    const auto abs_rhs {rhs < 0 ? -static_cast<eval_type>(rhs) : static_cast<eval_type>(rhs)};
    const auto abs_lhs {abs(lhs)};

    if (lhs != min_val && abs_lhs < abs_rhs)
    {
        return {0, 0};
    }

    detail::one_word_div(abs_lhs, abs_rhs, quotient);

    return negative_res ? -quotient : quotient;
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t operator/(const SignedInteger lhs, const int128_t rhs) noexcept
{
    if (BOOST_INT128_UNLIKELY(rhs == 0))
    {
        return {0, 0};
    }

    if (rhs.high != 0 && rhs.high != -1)
    {
        return {0,0};
    }
    else
    {
        const auto negative_res {static_cast<bool>((rhs.high < 0) ^ (lhs < 0))};
        const auto abs_rhs {abs(rhs)};
        // rhs == -2^64 has |rhs| greater than any 64-bit lhs, so the quotient is 0 (also avoids /0)
        if (abs_rhs.high != 0)
        {
            return {0, 0};
        }
        // Negate in the unsigned domain so INT64_MIN does not overflow (UBSAN)
        const auto abs_lhs {lhs < 0 ? -static_cast<std::uint64_t>(lhs) : static_cast<std::uint64_t>(lhs)};
        const int128_t res {0, abs_lhs / abs_rhs.low};

        return negative_res ? -res : res;
    }
}

#ifdef _MSC_VER
#  pragma warning(pop)
#endif

#if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)


BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR int128_t operator/(const int128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return static_cast<int128_t>(static_cast<detail::builtin_i128>(lhs) / rhs);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR int128_t operator/(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    return static_cast<int128_t>(lhs / static_cast<detail::builtin_i128>(rhs));
}

#elif defined(BOOST_INT128_HAS_MSVC_INT128)


BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE inline int128_t operator/(const int128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs / static_cast<int128_t>(rhs);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE inline int128_t operator/(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    return static_cast<int128_t>(lhs) / rhs;
}

#endif // BOOST_INT128_HAS_INT128

template <BOOST_INT128_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t& int128_t::operator/=(const Integer rhs) noexcept
{
    *this = static_cast<int128_t>(*this / rhs);
    return *this;
}

BOOST_INT128_HOST_DEVICE constexpr int128_t& int128_t::operator/=(const int128_t rhs) noexcept
{
    *this = *this / rhs;
    return *this;
}

#ifdef BOOST_INT128_HAS_MSVC_INT128

template <BOOST_INT128_128BIT_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE inline int128_t& int128_t::operator/=(const Integer rhs) noexcept
{
    *this = static_cast<int128_t>(*this / rhs);
    return *this;
}

#endif // BOOST_INT128_HAS_MSVC_INT128

#if defined(__clang__)
#  pragma clang diagnostic pop
#elif defined(__GNUC__)
#  pragma GCC diagnostic pop
#endif

//=====================================
// Modulo Operator
//=====================================

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t operator%(int128_t lhs, UnsignedInteger rhs) noexcept;

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t operator%(UnsignedInteger lhs, int128_t rhs) noexcept;

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t operator%(int128_t lhs, SignedInteger rhs) noexcept;

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t operator%(SignedInteger lhs, int128_t rhs) noexcept;

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr int128_t operator%(int128_t lhs, int128_t rhs) noexcept;

template <BOOST_INT128_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t operator%(const int128_t lhs, const UnsignedInteger rhs) noexcept
{
    using eval_type = detail::evaluation_type_t<UnsignedInteger>;

    if (BOOST_INT128_UNLIKELY(rhs == 0))
    {
        return {0, 0};
    }

    int128_t quotient {};
    int128_t remainder {};

    const auto abs_lhs {abs(lhs)};

    detail::one_word_div(abs_lhs, static_cast<eval_type>(rhs), quotient, remainder);

    return lhs < 0 ? -remainder : remainder;
}

template <BOOST_INT128_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t operator%(const UnsignedInteger lhs, const int128_t rhs) noexcept
{
    using eval_type = detail::evaluation_type_t<UnsignedInteger>;

    if (BOOST_INT128_UNLIKELY(rhs == 0))
    {
        return {0, 0};
    }

    const auto abs_rhs {abs(rhs)};

    if (abs_rhs > lhs)
    {
        return lhs;
    }

    const int128_t remainder {0, static_cast<eval_type>(lhs) % abs_rhs.low};

    return remainder;
}

template <BOOST_INT128_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t operator%(const int128_t lhs, const SignedInteger rhs) noexcept
{
    return lhs % static_cast<int128_t>(rhs);
}

template <BOOST_INT128_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t operator%(const SignedInteger lhs, const int128_t rhs) noexcept
{
    return static_cast<int128_t>(lhs) % rhs;
}

BOOST_INT128_HOST_DEVICE constexpr int128_t operator%(const int128_t lhs, const int128_t rhs) noexcept
{
    if (rhs == 0)
    {
        return {0, 0};
    }

    constexpr int128_t min_val {INT64_MIN, 0};
    const auto abs_lhs {abs(lhs)};
    const auto abs_rhs {abs(rhs)};

    if (lhs != min_val && rhs != min_val && abs_rhs > abs_lhs)
    {
        return lhs;
    }

    const auto is_neg {lhs < 0};

    // Narrow fast path: when the divisor magnitude fits in 64 bits, take the remainder of the
    // magnitudes with the hardware-accelerated one_word_div and reapply the dividend's sign.
    if (abs_rhs.high == 0)
    {
        int128_t remainder {};

        if (abs_lhs.high == 0)
        {
            remainder = int128_t{0, abs_lhs.low % abs_rhs.low};
        }
        else
        {
            int128_t quotient {};
            detail::one_word_div(abs_lhs, abs_rhs.low, quotient, remainder);
        }

        return is_neg ? -remainder : remainder;
    }

    #if defined(BOOST_INT128_HAS_INT128)

    return static_cast<int128_t>(static_cast<detail::builtin_i128>(lhs) % static_cast<detail::builtin_i128>(rhs));

    #else

    int128_t remainder {};
    detail::knuth_div(abs_lhs, abs_rhs, remainder);
    return is_neg ? -remainder : remainder;

    #endif
}

#if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR int128_t operator%(const int128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return static_cast<detail::builtin_i128>(lhs) % rhs;
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR int128_t operator%(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    return lhs % static_cast<detail::builtin_i128>(rhs);
}


#elif defined(BOOST_INT128_HAS_MSVC_INT128)

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE inline int128_t operator%(const int128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs % static_cast<int128_t>(rhs);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE inline int128_t operator%(const detail::builtin_i128 lhs, const int128_t rhs) noexcept
{
    return static_cast<int128_t>(lhs) % rhs;
}


#endif // BOOST_INT128_HAS_INT128

template <BOOST_INT128_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr int128_t& int128_t::operator%=(const Integer rhs) noexcept
{
    *this = static_cast<int128_t>(*this % rhs);
    return *this;
}

BOOST_INT128_HOST_DEVICE constexpr int128_t& int128_t::operator%=(const int128_t rhs) noexcept
{
    *this = *this % rhs;
    return *this;
}

#ifdef BOOST_INT128_HAS_MSVC_INT128

template <BOOST_INT128_128BIT_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE inline int128_t& int128_t::operator%=(const Integer rhs) noexcept
{
    *this = static_cast<int128_t>(*this % rhs);
    return *this;
}

#endif // BOOST_INT128_HAS_MSVC_INT128

namespace detail {

template <bool>
class numeric_limits_impl_i128
{
public:

        // Member constants
    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = true;
    static constexpr bool is_integer = true;
    static constexpr bool is_exact = true;
    static constexpr bool has_infinity = false;
    static constexpr bool has_quiet_NaN = false;
    static constexpr bool has_signaling_NaN = false;

    // C++23 deprecated the following two members
    #if defined(__GNUC__) && __cplusplus > 202002L
    #  pragma GCC diagnostic push
    #  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    #elif defined(_MSC_VER)
    #  pragma warning(push)
    #  pragma warning(disable:4996)
    #endif

    static constexpr std::float_denorm_style has_denorm = std::denorm_absent;
    static constexpr bool has_denorm_loss = false;

    #if defined(__GNUC__) && __cplusplus > 202002L
    #  pragma GCC diagnostic pop
    #elif defined(_MSC_VER)
    #  pragma warning(pop)
    #endif

    static constexpr std::float_round_style round_style = std::round_toward_zero;
    static constexpr bool is_iec559 = false;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = true;
    static constexpr int digits = 127;
    static constexpr int digits10 = 38;
    static constexpr int max_digits10 = 0;
    static constexpr int radix = 2;
    static constexpr int min_exponent = 0;
    static constexpr int min_exponent10 = 0;
    static constexpr int max_exponent = 0;
    static constexpr int max_exponent10 = 0;
    static constexpr bool traps = std::numeric_limits<std::uint64_t>::traps;
    static constexpr bool tinyness_before = false;

    // Member functions
    BOOST_INT128_HOST_DEVICE static constexpr auto (min)        () -> boost::int128::int128_t { return {INT64_MIN, 0}; }
    BOOST_INT128_HOST_DEVICE static constexpr auto lowest       () -> boost::int128::int128_t { return {INT64_MIN, 0}; }
    BOOST_INT128_HOST_DEVICE static constexpr auto (max)        () -> boost::int128::int128_t { return {INT64_MAX, UINT64_MAX}; }
    BOOST_INT128_HOST_DEVICE static constexpr auto epsilon      () -> boost::int128::int128_t { return {0, 0}; }
    BOOST_INT128_HOST_DEVICE static constexpr auto round_error  () -> boost::int128::int128_t { return {0, 0}; }
    BOOST_INT128_HOST_DEVICE static constexpr auto infinity     () -> boost::int128::int128_t { return {0, 0}; }
    BOOST_INT128_HOST_DEVICE static constexpr auto quiet_NaN    () -> boost::int128::int128_t { return {0, 0}; }
    BOOST_INT128_HOST_DEVICE static constexpr auto signaling_NaN() -> boost::int128::int128_t { return {0, 0}; }
    BOOST_INT128_HOST_DEVICE static constexpr auto denorm_min   () -> boost::int128::int128_t { return {0, 0}; }
};

#if !defined(__cpp_inline_variables) || __cpp_inline_variables < 201606L

template <bool b> constexpr bool numeric_limits_impl_i128<b>::is_specialized;
template <bool b> constexpr bool numeric_limits_impl_i128<b>::is_signed;
template <bool b> constexpr bool numeric_limits_impl_i128<b>::is_integer;
template <bool b> constexpr bool numeric_limits_impl_i128<b>::is_exact;
template <bool b> constexpr bool numeric_limits_impl_i128<b>::has_infinity;
template <bool b> constexpr bool numeric_limits_impl_i128<b>::has_quiet_NaN;
template <bool b> constexpr bool numeric_limits_impl_i128<b>::has_signaling_NaN;

// These members were deprecated in C++23; suppress the deprecation warning rather
// than dropping the definitions.
#if defined(__GNUC__) && __cplusplus > 202002L
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable:4996)
#endif

template <bool b> constexpr std::float_denorm_style numeric_limits_impl_i128<b>::has_denorm;
template <bool b> constexpr bool numeric_limits_impl_i128<b>::has_denorm_loss;

#if defined(__GNUC__) && __cplusplus > 202002L
#  pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#  pragma warning(pop)
#endif

template <bool b> constexpr std::float_round_style numeric_limits_impl_i128<b>::round_style;
template <bool b> constexpr bool numeric_limits_impl_i128<b>::is_iec559;
template <bool b> constexpr bool numeric_limits_impl_i128<b>::is_bounded;
template <bool b> constexpr bool numeric_limits_impl_i128<b>::is_modulo;
template <bool b> constexpr int numeric_limits_impl_i128<b>::digits;
template <bool b> constexpr int numeric_limits_impl_i128<b>::digits10;
template <bool b> constexpr int numeric_limits_impl_i128<b>::max_digits10;
template <bool b> constexpr int numeric_limits_impl_i128<b>::radix;
template <bool b> constexpr int numeric_limits_impl_i128<b>::min_exponent;
template <bool b> constexpr int numeric_limits_impl_i128<b>::min_exponent10;
template <bool b> constexpr int numeric_limits_impl_i128<b>::max_exponent;
template <bool b> constexpr int numeric_limits_impl_i128<b>::max_exponent10;
template <bool b> constexpr bool numeric_limits_impl_i128<b>::traps;
template <bool b> constexpr bool numeric_limits_impl_i128<b>::tinyness_before;

#endif // !defined(__cpp_inline_variables) || __cpp_inline_variables < 201606L

} // namespace detail

} // namespace int128
} // namespace boost

namespace std {

#ifdef __clang__
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wmismatched-tags"
#endif

template <>
class numeric_limits<boost::int128::int128_t> :
    public boost::int128::detail::numeric_limits_impl_i128<true> {};

#ifdef __clang__
#  pragma clang diagnostic pop
#endif

} // namespace std

#endif // BOOST_INT128_DETAIL_INT128_HPP
// ===== END boost/int128/detail/int128_imp.hpp =====
// ===== BEGIN boost/int128/detail/uint128_imp.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_DETAIL_UINT128_IMP_HPP
#define BOOST_INT128_DETAIL_UINT128_IMP_HPP

// [amalgamate] skipped duplicate include of boost/int128/detail/fwd.hpp
// [amalgamate] skipped duplicate include of boost/int128/detail/config.hpp
// [amalgamate] skipped duplicate include of boost/int128/detail/traits.hpp
// [amalgamate] skipped duplicate include of boost/int128/detail/constants.hpp
// [amalgamate] skipped duplicate include of boost/int128/detail/clz.hpp
// [amalgamate] skipped duplicate include of boost/int128/detail/common_mul.hpp
// [amalgamate] skipped duplicate include of boost/int128/detail/common_div.hpp

#ifndef BOOST_INT128_BUILD_MODULE

#include <cstdint>
#include <cstring>
#include <climits>

#endif

namespace boost {
namespace int128 {

BOOST_INT128_EXPORT struct
    #if (defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)) && !defined(_M_IX86)
    alignas(alignof(detail::builtin_u128))
    #endif
uint128_t
{
    #if BOOST_INT128_ENDIAN_LITTLE_BYTE
    std::uint64_t low {};
    std::uint64_t high {};
    #else

    #ifdef __GNUC__
    #  pragma GCC diagnostic push
    #  pragma GCC diagnostic ignored "-Wreorder"
    #endif

    std::uint64_t high {};
    std::uint64_t low {};

    #ifdef __GNUC__
    #  pragma GCC diagnostic pop
    #endif

    #endif // BOOST_INT128_ENDIAN_LITTLE_BYTE

    // Defaulted basic construction
    constexpr uint128_t() noexcept = default;
    constexpr uint128_t(const uint128_t&) noexcept = default;
    constexpr uint128_t(uint128_t&&) noexcept = default;
    constexpr uint128_t& operator=(const uint128_t&) noexcept = default;
    constexpr uint128_t& operator=(uint128_t&&) noexcept = default;

    // Requires a conversion file to be implemented
    BOOST_INT128_HOST_DEVICE constexpr uint128_t(const int128_t& v) noexcept;

    // Construct from integral types
    #if BOOST_INT128_ENDIAN_LITTLE_BYTE

    BOOST_INT128_HOST_DEVICE constexpr uint128_t(const std::uint64_t hi, const std::uint64_t lo) noexcept : low {lo}, high {hi} {}

    template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE constexpr uint128_t(const SignedInteger v) noexcept : low {static_cast<std::uint64_t>(v)}, high {v < 0 ? UINT64_MAX : UINT64_C(0)} {}

    template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE constexpr uint128_t(const UnsignedInteger v) noexcept : low {static_cast<std::uint64_t>(v)}, high {} {}

    #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)

    BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t(const detail::builtin_i128 v) noexcept :
        low {static_cast<std::uint64_t>(v)},
        high {static_cast<std::uint64_t>(static_cast<detail::builtin_u128>(v) >> static_cast<detail::builtin_u128>(64U))} {}

    BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t(const detail::builtin_u128 v) noexcept :
        low {static_cast<std::uint64_t>(v)},
        high {static_cast<std::uint64_t>(v >> static_cast<detail::builtin_i128>(64U))} {}

    #endif // BOOST_INT128_HAS_INT128

    #else // Big endian

    BOOST_INT128_HOST_DEVICE constexpr uint128_t(const std::uint64_t hi, const std::uint64_t lo) noexcept : high {hi}, low {lo} {}

    template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE constexpr uint128_t(const SignedInteger v) noexcept : high {v < 0 ? UINT64_MAX : UINT64_C(0)}, low {static_cast<std::uint64_t>(v)} {}

    template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE constexpr uint128_t(const UnsignedInteger v) noexcept : high {}, low {static_cast<std::uint64_t>(v)} {}

    #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)

    BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t(const detail::builtin_i128 v) noexcept :
        high {static_cast<std::uint64_t>(static_cast<detail::builtin_u128>(v) >> 64U)},
        low {static_cast<std::uint64_t>(v)} {}

    BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t(const detail::builtin_u128 v) noexcept :
        high {static_cast<std::uint64_t>(v >> 64U)},
        low {static_cast<std::uint64_t>(v)} {}

    #endif // BOOST_INT128_HAS_INT128

    #endif // BOOST_INT128_ENDIAN_LITTLE_BYTE

    // Construct from floating-point types
    template <BOOST_INT128_DEFAULTED_FLOATING_POINT_CONCEPT>
    BOOST_INT128_HOST_DEVICE constexpr uint128_t(Float f) noexcept;

    // Integer conversion operators
    BOOST_INT128_HOST_DEVICE explicit constexpr operator bool() const noexcept {return low || high; }

    template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE constexpr operator SignedInteger() const noexcept { return static_cast<SignedInteger>(low); }

    template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE constexpr operator UnsignedInteger() const noexcept { return static_cast<UnsignedInteger>(low); }

    #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)

    BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR operator detail::builtin_i128() const noexcept { return static_cast<detail::builtin_i128>(static_cast<detail::builtin_u128>(high) << static_cast<detail::builtin_u128>(64)) | static_cast<detail::builtin_i128>(low); }

    BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR operator detail::builtin_u128() const noexcept { return (static_cast<detail::builtin_u128>(high) << static_cast<detail::builtin_u128>(64)) | static_cast<detail::builtin_u128>(low); }

    #endif // BOOST_INT128_HAS_INT128

    // Conversion to float
    // This is basically the same as ldexp(static_cast<T>(high), 64) + static_cast<T>(low),
    // but can be constexpr at C++11 instead of C++26
    BOOST_INT128_HOST_DEVICE constexpr operator float() const noexcept;
    BOOST_INT128_HOST_DEVICE constexpr operator double() const noexcept;

    // long doubles do not exist on the CUDA or SYCL (spir64) device
    #if !defined(BOOST_INT128_HAS_GPU_SUPPORT)
    constexpr operator long double() const noexcept;
    #endif

    // Compound OR
    template <BOOST_INT128_DEFAULTED_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE constexpr uint128_t& operator|=(Integer rhs) noexcept;

    BOOST_INT128_HOST_DEVICE constexpr uint128_t& operator|=(uint128_t rhs) noexcept;

    #ifdef BOOST_INT128_HAS_MSVC_INT128

    template <BOOST_INT128_DEFAULTED_128BIT_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE inline uint128_t& operator|=(Integer rhs) noexcept;

    #endif // BOOST_INT128_HAS_MSVC_INT128

    // Compound AND
    template <BOOST_INT128_DEFAULTED_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE constexpr uint128_t& operator&=(Integer rhs) noexcept;

    BOOST_INT128_HOST_DEVICE constexpr uint128_t& operator&=(uint128_t rhs) noexcept;

    #ifdef BOOST_INT128_HAS_MSVC_INT128

    template <BOOST_INT128_DEFAULTED_128BIT_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE inline uint128_t& operator&=(Integer rhs) noexcept;

    #endif // BOOST_INT128_HAS_MSVC_INT128

    // Compound XOR
    template <BOOST_INT128_DEFAULTED_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE constexpr uint128_t& operator^=(Integer rhs) noexcept;

    BOOST_INT128_HOST_DEVICE constexpr uint128_t& operator^=(uint128_t rhs) noexcept;

    #ifdef BOOST_INT128_HAS_MSVC_INT128

    template <BOOST_INT128_DEFAULTED_128BIT_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE inline uint128_t& operator^=(Integer rhs) noexcept;

    #endif // BOOST_INT128_HAS_MSVC_INT128

    // Compound Left Shift
    template <BOOST_INT128_DEFAULTED_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE constexpr uint128_t& operator<<=(Integer rhs) noexcept;

    BOOST_INT128_HOST_DEVICE constexpr uint128_t& operator<<=(uint128_t rhs) noexcept;

    #ifdef BOOST_INT128_HAS_MSVC_INT128

    template <BOOST_INT128_DEFAULTED_128BIT_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE inline uint128_t& operator<<=(Integer rhs) noexcept;

    #endif // BOOST_INT128_HAS_MSVC_INT128

    // Compound Right Shift
    template <BOOST_INT128_DEFAULTED_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE constexpr uint128_t& operator>>=(Integer rhs) noexcept;

    BOOST_INT128_HOST_DEVICE constexpr uint128_t& operator>>=(uint128_t rhs) noexcept;

    #ifdef BOOST_INT128_HAS_MSVC_INT128

    template <BOOST_INT128_DEFAULTED_128BIT_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE inline uint128_t& operator>>=(Integer rhs) noexcept;

    #endif // BOOST_INT128_HAS_MSVC_INT128

    BOOST_INT128_HOST_DEVICE constexpr uint128_t& operator++() noexcept;
    BOOST_INT128_HOST_DEVICE constexpr uint128_t operator++(int) noexcept;
    BOOST_INT128_HOST_DEVICE constexpr uint128_t& operator--() noexcept;
    BOOST_INT128_HOST_DEVICE constexpr uint128_t operator--(int) noexcept;

    // Compound Addition
    template <BOOST_INT128_DEFAULTED_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE constexpr uint128_t& operator+=(Integer rhs) noexcept;

    BOOST_INT128_HOST_DEVICE constexpr uint128_t& operator+=(uint128_t rhs) noexcept;

    #ifdef BOOST_INT128_HAS_MSVC_INT128

    template <BOOST_INT128_DEFAULTED_128BIT_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE inline uint128_t& operator+=(Integer rhs) noexcept;

    #endif // BOOST_INT128_HAS_MSVC_INT128

    // Compound Subtraction
    template <BOOST_INT128_DEFAULTED_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE constexpr uint128_t& operator-=(Integer rhs) noexcept;

    BOOST_INT128_HOST_DEVICE constexpr uint128_t& operator-=(uint128_t rhs) noexcept;

    #ifdef BOOST_INT128_HAS_MSVC_INT128

    template <BOOST_INT128_DEFAULTED_128BIT_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE inline uint128_t& operator-=(Integer rhs) noexcept;

    #endif // BOOST_INT128_HAS_MSVC_INT128

    // Compound Multiplication
    template <BOOST_INT128_DEFAULTED_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE constexpr uint128_t& operator*=(Integer rhs) noexcept;

    BOOST_INT128_HOST_DEVICE constexpr uint128_t& operator*=(uint128_t rhs) noexcept;

    #ifdef BOOST_INT128_HAS_MSVC_INT128

    template <BOOST_INT128_DEFAULTED_128BIT_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE inline uint128_t& operator*=(Integer rhs) noexcept;

    #endif // BOOST_INT128_HAS_MSVC_INT128

    // Compound Division
    template <BOOST_INT128_DEFAULTED_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE constexpr uint128_t& operator/=(Integer rhs) noexcept;

    BOOST_INT128_HOST_DEVICE constexpr uint128_t& operator/=(uint128_t rhs) noexcept;

    #ifdef BOOST_INT128_HAS_MSVC_INT128

    template <BOOST_INT128_DEFAULTED_128BIT_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE inline uint128_t& operator/=(Integer rhs) noexcept;

    #endif // BOOST_INT128_HAS_MSVC_INT128

    // Compound modulo
    template <BOOST_INT128_DEFAULTED_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE constexpr uint128_t& operator%=(Integer rhs) noexcept;

    BOOST_INT128_HOST_DEVICE constexpr uint128_t& operator%=(uint128_t rhs) noexcept;

    #ifdef BOOST_INT128_HAS_MSVC_INT128

    template <BOOST_INT128_DEFAULTED_128BIT_INTEGER_CONCEPT>
    BOOST_INT128_HOST_DEVICE inline uint128_t& operator%=(Integer rhs) noexcept;

    #endif // BOOST_INT128_HAS_MSVC_INT128
};

//=====================================
// Absolute Value function
//=====================================

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr uint128_t abs(const uint128_t value) noexcept
{
    return value;
}

//=====================================
// Float Conversion Operators
//=====================================

// The most correct way to do this would be std::ldexp(static_cast<T>(high), 64) + static_cast<T>(low);
// Since std::ldexp is not constexpr until C++23 we can work around this by multiplying the high word
// by 0xFFFFFFFF in order to generally replicate what ldexp is doing in the constexpr context.
// We also avoid pulling in <quadmath.h> for the __float128 case where we would need ldexpq

BOOST_INT128_HOST_DEVICE constexpr uint128_t::operator float() const noexcept
{
    return static_cast<float>(high) * detail::offset_value_v<float> + static_cast<float>(low);
}

BOOST_INT128_HOST_DEVICE constexpr uint128_t::operator double() const noexcept
{
    return static_cast<double>(high) * detail::offset_value_v<double> + static_cast<double>(low);
}

#if !defined(BOOST_INT128_HAS_GPU_SUPPORT)

constexpr uint128_t::operator long double() const noexcept
{
    return static_cast<long double>(high) * detail::offset_value_v<long double> + static_cast<long double>(low);
}

#endif // __NVCC__

//=====================================
// Float Construction
//=====================================

// Inverse of operator(Float): decompose f into (high, low) by dividing by 2^64.
// NaN/negative -> 0
// overflow -> UINT128_MAX.
template <BOOST_INT128_FLOATING_POINT_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t::uint128_t(Float f) noexcept
{
    constexpr Float two_32 {static_cast<Float>(UINT64_C(1) << 32)};
    constexpr Float two_64 {two_32 * two_32};

    // !(f >= 0) catches both NaN and negative values without using <cmath>
    if (!(f >= Float{0}))
    {
        return;
    }

    // Overflow test: f >= 2^128 iff f / 2^64 >= 2^64. Comparing scaled values
    // avoids materializing 2^128 as a Float, which overflows to +infinity for
    // `float` and is therefore not constant-evaluable on older compilers.
    const Float scaled {f / two_64};
    if (scaled >= two_64)
    {
        high = UINT64_MAX;
        low = UINT64_MAX;
        return;
    }

    high = static_cast<std::uint64_t>(scaled);
    const Float remainder {f - static_cast<Float>(high) * two_64};
    low = static_cast<std::uint64_t>(remainder);
}

//=====================================
// Unary Operators
//=====================================

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr uint128_t operator+(const uint128_t value) noexcept
{
    return value;
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr uint128_t operator-(const uint128_t value) noexcept
{
    return {~value.high + static_cast<std::uint64_t>(value.low == UINT64_C(0)), ~value.low + UINT64_C(1)};
}

//=====================================
// Equality Operators
//=====================================

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr bool operator==(const uint128_t lhs, const bool rhs) noexcept
{
    return lhs.high == UINT64_C(0) && lhs.low == static_cast<std::uint64_t>(rhs);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr bool operator==(const bool lhs, const uint128_t rhs) noexcept
{
    return rhs.high == UINT64_C(0) && rhs.low == static_cast<std::uint64_t>(lhs);
}

#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wsign-conversion"
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wsign-conversion"
#endif

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr bool operator==(const uint128_t lhs, const SignedInteger rhs) noexcept
{
    const uint128_t rhs_u {rhs};
    return lhs.high == rhs_u.high && lhs.low == rhs_u.low;
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr bool operator==(const SignedInteger lhs, const uint128_t rhs) noexcept
{
    const uint128_t lhs_u {lhs};
    return lhs_u.high == rhs.high && lhs_u.low == rhs.low;
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr bool operator==(const uint128_t lhs, const UnsignedInteger rhs) noexcept
{
    return lhs.high == UINT64_C(0) && lhs.low == static_cast<std::uint64_t>(rhs);
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr bool operator==(const UnsignedInteger lhs, const uint128_t rhs) noexcept
{
    return rhs.high == UINT64_C(0) && rhs.low == static_cast<std::uint64_t>(lhs);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr bool operator==(const uint128_t lhs, const uint128_t rhs) noexcept
{
    #if defined(__aarch64__) || defined(_M_ARM64) || defined(_M_AMD64)

    return lhs.low == rhs.low && lhs.high == rhs.high;

    #elif defined (__x86_64__) && !defined(BOOST_INT128_NO_BUILTIN_INT128)

    return static_cast<detail::builtin_u128>(lhs) == static_cast<detail::builtin_u128>(rhs);

    #elif (defined(__i386__) || defined(_M_IX86) || defined(_M_AMD64)) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION) && defined(__SSE2__)

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return lhs.low == rhs.low && lhs.high == rhs.high;
    }
    else
    {
        __m128i a = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&lhs));
        __m128i b = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&rhs));
        __m128i cmp = _mm_cmpeq_epi32(a, b);

        return _mm_movemask_ps(_mm_castsi128_ps(cmp)) == 0xF;
    }

    #else

    return lhs.high == rhs.high && lhs.low == rhs.low;

    #endif
}

#if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR bool operator==(const uint128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs == static_cast<uint128_t>(rhs);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR bool operator==(const detail::builtin_i128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) == rhs;
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR bool operator==(const uint128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return lhs == static_cast<uint128_t>(rhs);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR bool operator==(const detail::builtin_u128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) == rhs;
}

#endif // BOOST_INT128_HAS_INT128

//=====================================
// Inequality Operators
//=====================================

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr bool operator!=(const uint128_t lhs, const bool rhs) noexcept
{
    return lhs.high != UINT64_C(0) || lhs.low != static_cast<std::uint64_t>(rhs);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr bool operator!=(const bool lhs, const uint128_t rhs) noexcept
{
    return rhs.high != UINT64_C(0) || rhs.low != static_cast<std::uint64_t>(lhs);
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr bool operator!=(const uint128_t lhs, const SignedInteger rhs) noexcept
{
    const uint128_t rhs_u {rhs};
    return lhs.high != rhs_u.high || lhs.low != rhs_u.low;
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr bool operator!=(const SignedInteger lhs, const uint128_t rhs) noexcept
{
    const uint128_t lhs_u {lhs};
    return lhs_u.high != rhs.high || lhs_u.low != rhs.low;
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr bool operator!=(const uint128_t lhs, const UnsignedInteger rhs) noexcept
{
    return lhs.high != UINT64_C(0) || lhs.low != static_cast<std::uint64_t>(rhs);
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr bool operator!=(const UnsignedInteger lhs, const uint128_t rhs) noexcept
{
    return rhs.high != UINT64_C(0) || rhs.low != static_cast<std::uint64_t>(lhs);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr bool operator!=(const uint128_t lhs, const uint128_t rhs) noexcept
{
    #if defined(__aarch64__) || defined(_M_ARM64) || defined(_M_AMD64)

    return lhs.low != rhs.low || lhs.high != rhs.high;

    #elif defined(__x86_64__) && !defined(BOOST_INT128_NO_BUILTIN_INT128)

    return static_cast<detail::builtin_u128>(lhs) != static_cast<detail::builtin_u128>(rhs);

    #elif (defined(__i386__) || defined(_M_IX86)) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION) && defined(__SSE2__)

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return lhs.low != rhs.low || lhs.high != rhs.high;
    }
    else
    {
        __m128i a = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&lhs));
        __m128i b = _mm_loadu_si128(reinterpret_cast<const __m128i*>(&rhs));
        __m128i cmp = _mm_cmpeq_epi32(a, b);

        return _mm_movemask_ps(_mm_castsi128_ps(cmp)) != 0xF;
    }

    #else

    return lhs.high != rhs.high || lhs.low != rhs.low;

    #endif
}

#if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_BUILTIN_CONSTEXPR)


BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR bool operator!=(const uint128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs != static_cast<uint128_t>(rhs);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR bool operator!=(const detail::builtin_i128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) != rhs;
}


BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR bool operator!=(const uint128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return lhs != static_cast<uint128_t>(rhs);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR bool operator!=(const detail::builtin_u128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) != rhs;
}

#endif // BOOST_INT128_HAS_INT128

//=====================================
// Less than Operators
//=====================================

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr bool operator<(const uint128_t lhs, const SignedInteger rhs) noexcept
{
    const uint128_t rhs_u {rhs};
    return lhs.high == rhs_u.high ? lhs.low < rhs_u.low : lhs.high < rhs_u.high;
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr bool operator<(const SignedInteger lhs, const uint128_t rhs) noexcept
{
    const uint128_t lhs_u {lhs};
    return lhs_u.high == rhs.high ? lhs_u.low < rhs.low : lhs_u.high < rhs.high;
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr bool operator<(const uint128_t lhs, const UnsignedInteger rhs) noexcept
{
    return lhs.high == UINT64_C(0) && lhs.low < static_cast<std::uint64_t>(rhs);
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr bool operator<(const UnsignedInteger lhs, const uint128_t rhs) noexcept
{
    return rhs.high > UINT64_C(0) || static_cast<std::uint64_t>(lhs) < rhs.low;
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr bool operator<(const uint128_t lhs, const uint128_t rhs) noexcept
{
    // On ARM macs only with the clang compiler is casting to unsigned __int128 uniformly better (and seemingly cost free)
    #if defined(__clang__) && defined(BOOST_INT128_HAS_INT128)

    return static_cast<detail::builtin_u128>(lhs) < static_cast<detail::builtin_u128>(rhs);

    #elif defined(__x86_64__) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION) && defined(__GNUC__) && !defined(__clang__) && defined(BOOST_INT128_HAS_INT128)

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return lhs.high == rhs.high ? lhs.low < rhs.low : lhs.high < rhs.high; // LCOV_EXCL_LINE
    }
    else
    {
        detail::builtin_u128 builtin_lhs {};
        detail::builtin_u128 builtin_rhs {};

        std::memcpy(&builtin_lhs, &lhs, sizeof(builtin_lhs));
        std::memcpy(&builtin_rhs, &rhs, sizeof(builtin_rhs));

        return builtin_lhs < builtin_rhs;
    }

    #elif (defined(__i386__) || defined(_M_IX86) || defined(__arm__)) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION)

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return lhs.high == rhs.high ? lhs.low < rhs.low : lhs.high < rhs.high; // LCOV_EXCL_LINE
    }
    else
    {
        std::uint32_t l[4] {};
        std::uint32_t r[4] {};
        std::memcpy(l, &lhs, sizeof(lhs));
        std::memcpy(r, &rhs, sizeof(rhs));

        if (l[3] != r[3])
        {
            return l[3] < r[3];
        }
        else if (l[2] != r[2])
        {
            return l[2] < r[2];
        }
        else if (l[1] != r[1])
        {
            return l[1] < r[1];
        }
        else
        {
            return l[0] < r[0];
        }
    }

    #else

    return lhs.high == rhs.high ? lhs.low < rhs.low : lhs.high < rhs.high;

    #endif
}

#if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)


BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR bool operator<(const uint128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs < static_cast<uint128_t>(rhs);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR bool operator<(const detail::builtin_i128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) < rhs;
}


BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR bool operator<(const uint128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return lhs < static_cast<uint128_t>(rhs);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR bool operator<(const detail::builtin_u128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) < rhs;
}

#endif // BOOST_INT128_HAS_INT128

//=====================================
// Less Equal Operators
//=====================================

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr bool operator<=(const uint128_t lhs, const SignedInteger rhs) noexcept
{
    const uint128_t rhs_u {rhs};
    return lhs.high == rhs_u.high ? lhs.low <= rhs_u.low : lhs.high < rhs_u.high;
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr bool operator<=(const SignedInteger lhs, const uint128_t rhs) noexcept
{
    const uint128_t lhs_u {lhs};
    return lhs_u.high == rhs.high ? lhs_u.low <= rhs.low : lhs_u.high < rhs.high;
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr bool operator<=(const uint128_t lhs, const UnsignedInteger rhs) noexcept
{
    return lhs.high == UINT64_C(0) && lhs.low <= static_cast<std::uint64_t>(rhs);
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr bool operator<=(const UnsignedInteger lhs, const uint128_t rhs) noexcept
{
    return rhs.high > UINT64_C(0) || static_cast<std::uint64_t>(lhs) <= rhs.low;
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr bool operator<=(const uint128_t lhs, const uint128_t rhs) noexcept
{
    #if defined(__clang__) && defined(BOOST_INT128_HAS_INT128)

    return static_cast<detail::builtin_u128>(lhs) <= static_cast<detail::builtin_u128>(rhs);

    #elif defined(__x86_64__) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION) && defined(__GNUC__) && !defined(__clang__) && defined(BOOST_INT128_HAS_INT128)

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return lhs.high == rhs.high ? lhs.low <= rhs.low : lhs.high <= rhs.high; // LCOV_EXCL_LINE
    }
    else
    {
        detail::builtin_u128 builtin_lhs {};
        detail::builtin_u128 builtin_rhs {};

        std::memcpy(&builtin_lhs, &lhs, sizeof(builtin_lhs));
        std::memcpy(&builtin_rhs, &rhs, sizeof(builtin_rhs));

        return builtin_lhs <= builtin_rhs;
    }

    #elif (defined(__i386__) || defined(_M_IX86) || defined(__arm__)) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION)

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return lhs.high == rhs.high ? lhs.low <= rhs.low : lhs.high <= rhs.high; // LCOV_EXCL_LINE
    }
    else
    {
        std::uint32_t l[4] {};
        std::uint32_t r[4] {};
        std::memcpy(l, &lhs, sizeof(lhs));
        std::memcpy(r, &rhs, sizeof(rhs));

        if (l[3] != r[3])
        {
            return l[3] < r[3];
        }
        else if (l[2] != r[2])
        {
            return l[2] < r[2];
        }
        else if (l[1] != r[1])
        {
            return l[1] < r[1];
        }
        else
        {
            return l[0] <= r[0];
        }
    }

    #else

    return lhs.high == rhs.high ? lhs.low <= rhs.low : lhs.high <= rhs.high;

    #endif
}

#if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)


BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR bool operator<=(const uint128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs <= static_cast<uint128_t>(rhs);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR bool operator<=(const detail::builtin_i128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) <= rhs;
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR bool operator<=(const uint128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return lhs <= static_cast<uint128_t>(rhs);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR bool operator<=(const detail::builtin_u128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) <= rhs;
}


#endif // BOOST_INT128_HAS_INT128

//=====================================
// Greater Than Operators
//=====================================

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr bool operator>(const uint128_t lhs, const SignedInteger rhs) noexcept
{
    const uint128_t rhs_u {rhs};
    return lhs.high == rhs_u.high ? lhs.low > rhs_u.low : lhs.high > rhs_u.high;
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr bool operator>(const SignedInteger lhs, const uint128_t rhs) noexcept
{
    const uint128_t lhs_u {lhs};
    return lhs_u.high == rhs.high ? lhs_u.low > rhs.low : lhs_u.high > rhs.high;
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr bool operator>(const uint128_t lhs, const UnsignedInteger rhs) noexcept
{
    return lhs.high > UINT64_C(0) || lhs.low > static_cast<std::uint64_t>(rhs);
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr bool operator>(const UnsignedInteger lhs, const uint128_t rhs) noexcept
{
    return rhs.high == UINT64_C(0) && static_cast<std::uint64_t>(lhs) > rhs.low;
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr bool operator>(const uint128_t lhs, const uint128_t rhs) noexcept
{
    #if defined(__clang__) && defined(BOOST_INT128_HAS_INT128)

    return static_cast<detail::builtin_u128>(lhs) > static_cast<detail::builtin_u128>(rhs);

    #elif defined(__x86_64__) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION) && defined(__GNUC__) && !defined(__clang__) && defined(BOOST_INT128_HAS_INT128)

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return lhs.high == rhs.high ? rhs.low < lhs.low : rhs.high < lhs.high; // LCOV_EXCL_LINE
    }
    else
    {
        detail::builtin_u128 builtin_lhs {};
        detail::builtin_u128 builtin_rhs {};

        std::memcpy(&builtin_lhs, &lhs, sizeof(builtin_lhs));
        std::memcpy(&builtin_rhs, &rhs, sizeof(builtin_rhs));

        return builtin_lhs > builtin_rhs;
    }

    #elif (defined(__i386__) || defined(_M_IX86) || defined(__arm__)) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION)

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return lhs.high == rhs.high ? rhs.low < lhs.low : rhs.high < lhs.high; // LCOV_EXCL_LINE
    }
    else
    {
        std::uint32_t l[4] {};
        std::uint32_t r[4] {};
        std::memcpy(l, &lhs, sizeof(lhs));
        std::memcpy(r, &rhs, sizeof(rhs));

        if (l[3] != r[3])
        {
            return l[3] > r[3];
        }
        else if (l[2] != r[2])
        {
            return l[2] > r[2];
        }
        else if (l[1] != r[1])
        {
            return l[1] > r[1];
        }
        else
        {
            return l[0] > r[0];
        }
    }

    #else

    return lhs.high == rhs.high ? rhs.low < lhs.low : rhs.high < lhs.high;

    #endif
}

#if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)


BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR bool operator>(const uint128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs > static_cast<uint128_t>(rhs);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR bool operator>(const detail::builtin_i128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) > rhs;
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR bool operator>(const uint128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return lhs > static_cast<uint128_t>(rhs);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR bool operator>(const detail::builtin_u128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) > rhs;
}


#endif // BOOST_INT128_HAS_INT128

//=====================================
// Greater-equal Operators
//=====================================

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr bool operator>=(const uint128_t lhs, const SignedInteger rhs) noexcept
{
    const uint128_t rhs_u {rhs};
    return lhs.high == rhs_u.high ? lhs.low >= rhs_u.low : lhs.high > rhs_u.high;
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr bool operator>=(const SignedInteger lhs, const uint128_t rhs) noexcept
{
    const uint128_t lhs_u {lhs};
    return lhs_u.high == rhs.high ? lhs_u.low >= rhs.low : lhs_u.high > rhs.high;
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr bool operator>=(const uint128_t lhs, const UnsignedInteger rhs) noexcept
{
    return lhs.high > UINT64_C(0) || lhs.low >= static_cast<std::uint64_t>(rhs);
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr bool operator>=(const UnsignedInteger lhs, const uint128_t rhs) noexcept
{
    return rhs.high == UINT64_C(0) && static_cast<std::uint64_t>(lhs) >= rhs.low;
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr bool operator>=(const uint128_t lhs, const uint128_t rhs) noexcept
{
    #if defined(__clang__) && defined(BOOST_INT128_HAS_INT128)

    return static_cast<detail::builtin_u128>(lhs) >= static_cast<detail::builtin_u128>(rhs);

    #elif defined(__x86_64__) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION) && defined(__GNUC__) && !defined(__clang__) && defined(BOOST_INT128_HAS_INT128)

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return lhs.high == rhs.high ? rhs.low <= lhs.low : rhs.high <= lhs.high; // LCOV_EXCL_LINE
    }
    else
    {
        detail::builtin_u128 builtin_lhs {};
        detail::builtin_u128 builtin_rhs {};

        std::memcpy(&builtin_lhs, &lhs, sizeof(builtin_lhs));
        std::memcpy(&builtin_rhs, &rhs, sizeof(builtin_rhs));

        return builtin_lhs >= builtin_rhs;
    }

    #elif (defined(__i386__) || defined(_M_IX86) || defined(__arm__)) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION)

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return lhs.high == rhs.high ? rhs.low <= lhs.low : rhs.high <= lhs.high; // LCOV_EXCL_LINE
    }
    else
    {
        std::uint32_t l[4] {};
        std::uint32_t r[4] {};
        std::memcpy(l, &lhs, sizeof(lhs));
        std::memcpy(r, &rhs, sizeof(rhs));

        if (l[3] != r[3])
        {
            return l[3] > r[3];
        }
        else if (l[2] != r[2])
        {
            return l[2] > r[2];
        }
        else if (l[1] != r[1])
        {
            return l[1] > r[1];
        }
        else
        {
            return l[0] >= r[0];
        }
    }

    #else

    return lhs.high == rhs.high ? rhs.low <= lhs.low : rhs.high <= lhs.high;

    #endif
}

#if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR bool operator>=(const uint128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs >= static_cast<uint128_t>(rhs);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR bool operator>=(const detail::builtin_i128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) >= rhs;
}


BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR bool operator>=(const uint128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return lhs >= static_cast<uint128_t>(rhs);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR bool operator>=(const detail::builtin_u128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) >= rhs;
}


#endif // BOOST_INT128_HAS_INT128

//=====================================
// Spaceship Operator
//=====================================

#ifdef BOOST_INT128_HAS_SPACESHIP_OPERATOR

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr std::strong_ordering operator<=>(const uint128_t lhs, const uint128_t rhs) noexcept
{
    if (lhs < rhs)
    {
        return std::strong_ordering::less;
    }
    else if (lhs == rhs)
    {
        return std::strong_ordering::equivalent;
    }
    else
    {
        return std::strong_ordering::greater;
    }
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr std::strong_ordering operator<=>(const uint128_t lhs, const UnsignedInteger rhs) noexcept
{
    if (lhs < rhs)
    {
        return std::strong_ordering::less;
    }
    else if (lhs == rhs)
    {
        return std::strong_ordering::equivalent;
    }
    else
    {
        return std::strong_ordering::greater;
    }
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr std::strong_ordering operator<=>(const UnsignedInteger lhs, const uint128_t rhs) noexcept
{
    if (lhs < rhs)
    {
        return std::strong_ordering::less;
    }
    else if (lhs == rhs)
    {
        return std::strong_ordering::equivalent;
    }
    else
    {
        return std::strong_ordering::greater;
    }
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr std::strong_ordering operator<=>(const SignedInteger lhs, const uint128_t rhs) noexcept
{
    if (lhs < rhs)
    {
        return std::strong_ordering::less;
    }
    else if (lhs == rhs)
    {
        return std::strong_ordering::equivalent;
    }
    else
    {
        return std::strong_ordering::greater;
    }
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr std::strong_ordering operator<=>(const uint128_t lhs, const SignedInteger rhs) noexcept
{
    if (lhs < rhs)
    {
        return std::strong_ordering::less;
    }
    else if (lhs == rhs)
    {
        return std::strong_ordering::equivalent;
    }
    else
    {
        return std::strong_ordering::greater;
    }
}

#endif

//=====================================
// Not Operator
//=====================================

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr uint128_t operator~(const uint128_t rhs) noexcept
{
    return {~rhs.high, ~rhs.low};
}

//=====================================
// OR Operator
//=====================================

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator|(const uint128_t lhs, const SignedInteger rhs) noexcept
{
    return {lhs.high | (rhs < 0 ? ~UINT64_C(0) : UINT64_C(0)), lhs.low | static_cast<std::uint64_t>(rhs)};
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator|(const SignedInteger lhs, const uint128_t rhs) noexcept
{
    return {rhs.high | (lhs < 0 ? ~UINT64_C(0) : UINT64_C(0)), rhs.low | static_cast<std::uint64_t>(lhs)};
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator|(const uint128_t lhs, const UnsignedInteger rhs) noexcept
{
    return {lhs.high, lhs.low | static_cast<std::uint64_t>(rhs)};
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator|(const UnsignedInteger lhs, const uint128_t rhs) noexcept
{
    return {rhs.high, rhs.low | static_cast<std::uint64_t>(lhs)};
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr uint128_t operator|(const uint128_t lhs, const uint128_t rhs) noexcept
{
    return {lhs.high | rhs.high, lhs.low | rhs.low};
}

#if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)


BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t operator|(const uint128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs | static_cast<uint128_t>(rhs);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t operator|(const detail::builtin_i128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) | rhs;
}


BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t operator|(const uint128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return lhs | static_cast<uint128_t>(rhs);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t operator|(const detail::builtin_u128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) | rhs;
}

#endif // BOOST_INT128_HAS_INT128

template <BOOST_INT128_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t& uint128_t::operator|=(const Integer rhs) noexcept
{
    *this = *this | rhs;
    return *this;
}
BOOST_INT128_HOST_DEVICE constexpr uint128_t& uint128_t::operator|=(const uint128_t rhs) noexcept
{
    *this = *this | rhs;
    return *this;
}

#ifdef BOOST_INT128_HAS_MSVC_INT128

template <BOOST_INT128_128BIT_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE inline uint128_t& uint128_t::operator|=(const Integer rhs) noexcept
{
    *this = *this | rhs;
    return *this;
}

#endif

//=====================================
// AND Operator
//=====================================

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator&(const uint128_t lhs, const SignedInteger rhs) noexcept
{
    return {lhs.high & (rhs < 0 ? ~UINT64_C(0) : UINT64_C(0)), lhs.low & static_cast<std::uint64_t>(rhs)};
}

template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator&(const SignedInteger lhs, const uint128_t rhs) noexcept
{
    return {rhs.high & (lhs < 0 ? ~UINT64_C(0) : UINT64_C(0)), rhs.low & static_cast<std::uint64_t>(lhs)};
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator&(const uint128_t lhs, const UnsignedInteger rhs) noexcept
{
    return {UINT64_C(0), lhs.low & static_cast<std::uint64_t>(rhs)};
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator&(const UnsignedInteger lhs, const uint128_t rhs) noexcept
{
    return {UINT64_C(0), rhs.low & static_cast<std::uint64_t>(lhs)};
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr uint128_t operator&(const uint128_t lhs, const uint128_t rhs) noexcept
{
    return {lhs.high & rhs.high, lhs.low & rhs.low};
}

#if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)


BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t operator&(const uint128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs & static_cast<uint128_t>(rhs);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t operator&(const detail::builtin_i128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) & rhs;
}


BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t operator&(const uint128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return lhs & static_cast<uint128_t>(rhs);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t operator&(const detail::builtin_u128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) & rhs;
}

#endif // BOOST_INT128_HAS_INT128

template <BOOST_INT128_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t& uint128_t::operator&=(const Integer rhs) noexcept
{
    *this = *this & rhs;
    return *this;
}

BOOST_INT128_HOST_DEVICE constexpr uint128_t& uint128_t::operator&=(const uint128_t rhs) noexcept
{
    *this = *this & rhs;
    return *this;
}

#ifdef BOOST_INT128_HAS_MSVC_INT128

template <BOOST_INT128_128BIT_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE inline uint128_t& uint128_t::operator&=(Integer rhs) noexcept
{
    *this = *this & rhs;
    return *this;
}

#endif // BOOST_INT128_HAS_MSVC_INT128


//=====================================
// XOR Operator
//=====================================

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator^(const uint128_t lhs, const SignedInteger rhs) noexcept
{
    return {lhs.high ^ (rhs < 0 ? ~UINT64_C(0) : UINT64_C(0)), lhs.low ^ static_cast<std::uint64_t>(rhs)};
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator^(const SignedInteger lhs, const uint128_t rhs) noexcept
{
    return {rhs.high ^ (lhs < 0 ? ~UINT64_C(0) : UINT64_C(0)), rhs.low ^ static_cast<std::uint64_t>(lhs)};
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator^(const uint128_t lhs, const UnsignedInteger rhs) noexcept
{
    return {lhs.high, lhs.low ^ static_cast<std::uint64_t>(rhs)};
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator^(const UnsignedInteger lhs, const uint128_t rhs) noexcept
{
    return {rhs.high, rhs.low ^ static_cast<std::uint64_t>(lhs)};
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr uint128_t operator^(const uint128_t lhs, const uint128_t rhs) noexcept
{
    return {lhs.high ^ rhs.high, lhs.low ^ rhs.low};
}

#if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)


BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t operator^(const uint128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs ^ static_cast<uint128_t>(rhs);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t operator^(const detail::builtin_i128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) ^ rhs;
}


BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t operator^(const uint128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return lhs ^ static_cast<uint128_t>(rhs);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t operator^(const detail::builtin_u128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) ^ rhs;
}

#endif // BOOST_INT128_HAS_INT128

template <BOOST_INT128_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t& uint128_t::operator^=(const Integer rhs) noexcept
{
    *this = *this ^ rhs;
    return *this;
}

BOOST_INT128_HOST_DEVICE constexpr uint128_t& uint128_t::operator^=(const uint128_t rhs) noexcept
{
    *this = *this ^ rhs;
    return *this;
}

#ifdef BOOST_INT128_HAS_MSVC_INT128

template <BOOST_INT128_128BIT_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE inline uint128_t& uint128_t::operator^=(Integer rhs) noexcept
{
    *this = *this ^ rhs;
    return *this;
}

#endif // BOOST_INT128_HAS_MSVC_INT128

//=====================================
// Left Shift Operator
//=====================================

namespace detail {

template <typename Integer>
BOOST_INT128_HOST_DEVICE constexpr uint128_t default_ls_impl(const uint128_t lhs, const Integer rhs) noexcept
{
    static_assert(std::is_integral<Integer>::value, "Needs to be a builtin type");

    BOOST_INT128_IF_CONSTEXPR (std::numeric_limits<Integer>::is_signed)
    {
        if (rhs < 0 || rhs >= 128)
        {
            return {0, 0};
        }
    }
    else
    {
        if (rhs >= 128)
        {
            return {0, 0};
        }
    }

    if (rhs == 0)
    {
        return lhs;
    }

    if (rhs == 64)
    {
        return {lhs.low, 0};
    }

    if (rhs > 64)
    {
        return {lhs.low << (rhs - 64), 0};
    }

    return {
        (lhs.high << rhs) | (lhs.low >> (64 - rhs)),
        lhs.low << rhs
    };
}

template <typename T>
BOOST_INT128_HOST_DEVICE uint128_t intrinsic_ls_impl(const uint128_t lhs, const T rhs) noexcept
{
    BOOST_INT128_IF_CONSTEXPR (std::numeric_limits<T>::is_signed)
    {
        if (BOOST_INT128_UNLIKELY(rhs >= 128 || rhs < 0))
        {
            return {0, 0};
        }
    }
    else
    {
        if (BOOST_INT128_UNLIKELY(rhs >= 128))
        {
            return {0, 0};
        }
    }

    if (BOOST_INT128_UNLIKELY(rhs == 0))
    {
        return lhs;
    }

    #ifdef BOOST_INT128_HAS_INT128

    #  if defined(__aarch64__)

        #if defined(__GNUC__) && __GNUC__ >= 8
        #  pragma GCC diagnostic push
        #  pragma GCC diagnostic ignored "-Wclass-memaccess"
        #endif

        builtin_u128 value;
        std::memcpy(&value, &lhs, sizeof(builtin_u128));
        const auto res {value << rhs};

        uint128_t return_value;
        std::memcpy(&return_value, &res, sizeof(uint128_t));
        return return_value;

        #if defined(__GNUC__) && __GNUC__ >= 8
        #  pragma GCC diagnostic pop
        #endif

    #  else

        return static_cast<builtin_u128>(lhs) << rhs;

    #  endif

    #else

    if (rhs == 64)
    {
        return {lhs.low, 0};
    }

    if (rhs > 64)
    {
        return {lhs.low << (rhs - 64), 0};
    }

    return {
        (lhs.high << rhs) | (lhs.low >> (64 - rhs)),
        lhs.low << rhs
    };

    #endif
}

} // namespace detail

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator<<(const uint128_t lhs, const Integer rhs) noexcept
{
    #ifndef BOOST_INT128_NO_CONSTEVAL_DETECTION

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return detail::default_ls_impl(lhs, rhs); // LCOV_EXCL_LINE
    }
    else
    {
        return detail::intrinsic_ls_impl(lhs, rhs);
    }

    #else

    return detail::default_ls_impl(lhs, rhs);

    #endif
}

// A number of different overloads to ensure that we return the same type as the builtins would

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr uint128_t operator<<(const uint128_t lhs, const uint128_t rhs) noexcept
{
    if (rhs.high > UINT64_C(0) || rhs.low >= UINT64_C(128))
    {
        return uint128_t{0};
    }

    return lhs << rhs.low;
}

#if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR detail::builtin_u128 operator<<(const detail::builtin_u128 lhs, const uint128_t rhs) noexcept
{
    constexpr auto bit_width {sizeof(detail::builtin_u128 ) * 8};

    if (rhs.high > UINT64_C(0) || rhs.low >= bit_width)
    {
        return 0;
    }
    return lhs << static_cast<detail::builtin_u128>(rhs.low);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR detail::builtin_i128 operator<<(const detail::builtin_i128 lhs, const uint128_t rhs) noexcept
{
    constexpr auto bit_width {sizeof(detail::builtin_u128) * 8};

    if (rhs.high > UINT64_C(0) || rhs.low >= bit_width)
    {
        return 0;
    }

    return lhs << static_cast<detail::builtin_u128>(rhs.low);
}

#endif

BOOST_INT128_EXPORT template <typename SignedInteger, std::enable_if_t<detail::is_signed_integer_v<SignedInteger> && (sizeof(SignedInteger) * 8 <= 16), bool> = true>
BOOST_INT128_HOST_DEVICE constexpr int operator<<(const SignedInteger lhs, const uint128_t rhs) noexcept
{
    constexpr auto bit_width {sizeof(SignedInteger) * 8};

    if (rhs.high > UINT64_C(0) || rhs.low >= bit_width)
    {
        return 0;
    }

    return static_cast<int>(lhs) << rhs.low;
}

BOOST_INT128_EXPORT template <typename UnsignedInteger, std::enable_if_t<detail::is_unsigned_integer_v<UnsignedInteger> && (sizeof(UnsignedInteger) * 8 <= 16), bool> = true>
BOOST_INT128_HOST_DEVICE constexpr unsigned int operator<<(const UnsignedInteger lhs, const uint128_t rhs) noexcept
{
    constexpr auto bit_width {sizeof(UnsignedInteger) * 8};

    if (rhs.high > UINT64_C(0) || rhs.low >= bit_width)
    {
        return 0;
    }

    return static_cast<unsigned int>(lhs) << rhs.low;
}

template <BOOST_INT128_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t& uint128_t::operator<<=(const Integer rhs) noexcept
{
    *this = *this << rhs;
    return *this;
}

BOOST_INT128_HOST_DEVICE constexpr uint128_t& uint128_t::operator<<=(const uint128_t rhs) noexcept
{
    *this = *this << rhs;
    return *this;
}

#ifdef BOOST_INT128_HAS_MSVC_INT128

template <BOOST_INT128_128BIT_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE inline uint128_t& uint128_t::operator<<=(Integer rhs) noexcept
{
    *this = *this << rhs;
    return *this;
}

#endif

//=====================================
// Right Shift Operator
//=====================================

namespace detail {

template <typename Integer>
BOOST_INT128_HOST_DEVICE constexpr uint128_t default_rs_impl(const uint128_t lhs, const Integer rhs) noexcept
{
    BOOST_INT128_IF_CONSTEXPR (std::numeric_limits<Integer>::is_signed)
    {
        if (rhs < 0 || rhs >= 128)
        {
            return {0, 0};
        }
    }
    else
    {
        if (rhs >= 128)
        {
            return {0, 0};
        }
    }

    if (rhs == 0)
    {
        return lhs;
    }

    if (rhs == 64)
    {
        return {0, lhs.high};
    }

    if (rhs > 64)
    {
        return {0, lhs.high >> (rhs - 64)};
    }

    return {
        lhs.high >> rhs,
        (lhs.low >> rhs) | (lhs.high << (64 - rhs))
    };
}

template <typename Integer>
BOOST_INT128_HOST_DEVICE uint128_t intrinsic_rs_impl(const uint128_t lhs, const Integer rhs) noexcept
{
    BOOST_INT128_IF_CONSTEXPR (std::numeric_limits<Integer>::is_signed)
    {
        if (BOOST_INT128_UNLIKELY(rhs >= 128 || rhs < 0))
        {
            return {0, 0};
        }
    }
    else
    {
        if (BOOST_INT128_UNLIKELY(rhs >= 128))
        {
            return {0, 0};
        }
    }

    if (BOOST_INT128_UNLIKELY(rhs == 0))
    {
        return lhs;
    }

    #ifdef BOOST_INT128_HAS_INT128

    #  ifdef __aarch64__

        #if defined(__GNUC__) && __GNUC__ >= 8
        #  pragma GCC diagnostic push
        #  pragma GCC diagnostic ignored "-Wclass-memaccess"
        #endif

        builtin_u128 value;
        std::memcpy(&value, &lhs, sizeof(builtin_u128));
        const auto res {value >> rhs};

        uint128_t return_value;
        std::memcpy(&return_value, &res, sizeof(uint128_t));
        return return_value;

        #if defined(__GNUC__) && __GNUC__ >= 8
        #  pragma GCC diagnostic pop
        #endif

    #  else
        return static_cast<builtin_u128>(lhs) >> rhs;
    #  endif

    #else

    if (rhs == 64)
    {
        return {0, lhs.high};
    }

    if (rhs < 64)
    {
        const auto result_low {(lhs.low >> rhs) | (lhs.high << (64 - rhs))};
        const auto result_high {lhs.high >> rhs};
        return {result_high, result_low};
    }

    return {0, lhs.high >> (rhs - 64)};

    #endif
}

} // namespace detail

BOOST_INT128_EXPORT template <typename Integer, std::enable_if_t<std::is_integral<Integer>::value, bool> = true>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator>>(const uint128_t lhs, const Integer rhs) noexcept
{
    #ifndef BOOST_INT128_NO_CONSTEVAL_DETECTION

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return detail::default_rs_impl(lhs, rhs); // LCOV_EXCL_LINE
    }
    else
    {
        return detail::intrinsic_rs_impl(lhs, rhs);
    }

    #else

    return detail::default_rs_impl(lhs, rhs);

    #endif
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr uint128_t operator>>(const uint128_t lhs, const uint128_t rhs) noexcept
{
    if (rhs.high > UINT64_C(0) || rhs.low >= UINT64_C(128))
    {
        return uint128_t{0};
    }

    return lhs >> rhs.low;
}

#if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR detail::builtin_u128 operator>>(const detail::builtin_u128 lhs, const uint128_t rhs) noexcept
{
    constexpr auto bit_width = sizeof(detail::builtin_u128) * 8;

    if (rhs.high > UINT64_C(0) || rhs.low >= bit_width)
    {
        return 0;
    }

    return lhs >> static_cast<detail::builtin_u128>(rhs.low);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR detail::builtin_i128 operator>>(const detail::builtin_i128 lhs, const uint128_t rhs) noexcept
{
    constexpr auto bit_width = sizeof(detail::builtin_i128) * 8;

    if (rhs.high > UINT64_C(0) || rhs.low >= bit_width)
    {
        return 0;
    }

    return lhs >> static_cast<detail::builtin_u128>(rhs.low);
}

#endif

BOOST_INT128_EXPORT template <typename SignedInteger, std::enable_if_t<detail::is_signed_integer_v<SignedInteger> && (sizeof(SignedInteger) * 8 <= 16), bool> = true>
BOOST_INT128_HOST_DEVICE constexpr int operator>>(const SignedInteger lhs, const uint128_t rhs) noexcept
{
    constexpr auto bit_width = sizeof(SignedInteger) * 8;

    if (rhs.high > UINT64_C(0) || rhs.low >= bit_width)
    {
        return 0;
    }

    return static_cast<int>(lhs) >> rhs.low;
}

BOOST_INT128_EXPORT template <typename UnsignedInteger, std::enable_if_t<detail::is_unsigned_integer_v<UnsignedInteger> && (sizeof(UnsignedInteger) * 8 <= 16), bool> = true>
BOOST_INT128_HOST_DEVICE constexpr unsigned operator>>(UnsignedInteger lhs, const uint128_t rhs) noexcept
{
    constexpr auto bit_width = sizeof(UnsignedInteger) * 8;

    if (rhs.high > UINT64_C(0) || rhs.low >= bit_width)
    {
        return 0;
    }

    return static_cast<unsigned>(lhs) >> rhs.low;
}

template <BOOST_INT128_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t& uint128_t::operator>>=(const Integer rhs) noexcept
{
    *this = *this >> rhs;
    return *this;
}

BOOST_INT128_HOST_DEVICE constexpr uint128_t& uint128_t::operator>>=(const uint128_t rhs) noexcept
{
    *this = *this >> rhs;
    return *this;
}

#ifdef BOOST_INT128_HAS_MSVC_INT128

template <BOOST_INT128_128BIT_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE inline uint128_t& uint128_t::operator>>=(Integer rhs) noexcept
{
    *this = *this >> rhs;
    return *this;
}

#endif

//=====================================
// Increment Operator
//=====================================

BOOST_INT128_HOST_DEVICE constexpr uint128_t& uint128_t::operator++() noexcept
{
    if (++low == UINT64_C(0))
    {
        ++high;
    }

    return *this;
}

BOOST_INT128_HOST_DEVICE constexpr uint128_t uint128_t::operator++(int) noexcept
{
    const auto temp {*this};
    ++(*this);
    return temp;
}

//=====================================
// Decrement Operator
//=====================================

BOOST_INT128_HOST_DEVICE constexpr uint128_t& uint128_t::operator--() noexcept
{
    if (--low == UINT64_MAX)
    {
        --high;
    }

    return *this;
}

BOOST_INT128_HOST_DEVICE constexpr uint128_t uint128_t::operator--(int) noexcept
{
    const auto temp {*this};
    --(*this);
    return temp;
}

//=====================================
// Addition Operator
//=====================================

namespace impl {

BOOST_INT128_HOST_DEVICE BOOST_INT128_FORCE_INLINE constexpr uint128_t default_add(const uint128_t lhs, const uint128_t rhs) noexcept
{
    #if defined(BOOST_INT128_HAS_BUILTIN_ADD_OVERFLOW) && (defined(__i386__) || (defined(__aarch64__) && !defined(__APPLE__)) || defined(__arm__) || (defined(__s390__) || defined(__s390x__)))

    uint128_t res {};
    res.high = lhs.high + rhs.high + __builtin_add_overflow(lhs.low, rhs.low, &res.low);

    return res;

    #else

    uint128_t temp {lhs.high + rhs.high, lhs.low + rhs.low};

    if (temp.low < lhs.low)
    {
        ++temp.high;
    }

    return temp;

    #endif
}

BOOST_INT128_HOST_DEVICE BOOST_INT128_FORCE_INLINE constexpr uint128_t default_add(const uint128_t lhs, const std::uint64_t rhs) noexcept
{
    #if defined(BOOST_INT128_HAS_BUILTIN_ADD_OVERFLOW) && (defined(__i386__) || (defined(__aarch64__) && !defined(__APPLE__)) || defined(__arm__) || (defined(__s390__) || defined(__s390x__)))

    uint128_t res {};
    res.high = lhs.high + __builtin_add_overflow(lhs.low, rhs, &res.low);

    return res;

    #else

    uint128_t temp {lhs.high, lhs.low + rhs};

    if (temp.low < lhs.low)
    {
        ++temp.high;
    }

    return temp;

    #endif
}

BOOST_INT128_HOST_DEVICE BOOST_INT128_FORCE_INLINE constexpr uint128_t default_sub(const uint128_t lhs, const uint128_t rhs) noexcept
{
    #if defined(BOOST_INT128_HAS_BUILTIN_SUB_OVERFLOW) && (defined(__i386__) || defined(__arm__) || (defined(__s390__) || defined(__s390x__)))

    uint128_t res {};
    res.high = lhs.high - rhs.high - __builtin_sub_overflow(lhs.low, rhs.low, &res.low);

    return res;

    #elif (defined(__x86_64__) || (defined(__aarch64__) && !defined(__APPLE__))) && !defined(_MSC_VER) && defined(BOOST_INT128_HAS_INT128)

    return static_cast<uint128_t>(static_cast<detail::builtin_u128>(lhs) - static_cast<detail::builtin_u128>(rhs));

    #else

    uint128_t temp {lhs.high - rhs.high, lhs.low - rhs.low};

    // Check for carry
    if (lhs.low < rhs.low)
    {
        --temp.high;
    }

    return temp;

    #endif
}

BOOST_INT128_HOST_DEVICE BOOST_INT128_FORCE_INLINE constexpr uint128_t default_sub(const uint128_t lhs, const std::uint64_t rhs) noexcept
{
    #if defined(BOOST_INT128_HAS_BUILTIN_SUB_OVERFLOW) && (defined(__i386__) || (defined(__aarch64__) && !defined(__APPLE__)) || defined(__arm__) || (defined(__s390__) || defined(__s390x__)))

    uint128_t res {};
    res.high = lhs.high - __builtin_sub_overflow(lhs.low, rhs, &res.low);

    return res;

    #else

    uint128_t temp {lhs.high, lhs.low - rhs};

    // Check for carry
    if (lhs.low < rhs)
    {
        --temp.high;
    }

    return temp;

    #endif
}

} // namespace impl

#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable : 4146)
#endif

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator+(const uint128_t lhs, const SignedInteger rhs) noexcept
{
    return rhs < 0 ? impl::default_sub(lhs, -static_cast<std::uint64_t>(rhs)) :
                     impl::default_add(lhs, static_cast<std::uint64_t>(rhs));
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator+(const SignedInteger lhs, const uint128_t rhs) noexcept
{
    return lhs < 0 ? impl::default_sub(rhs, -static_cast<std::uint64_t>(lhs)) :
                     impl::default_add(rhs, static_cast<std::uint64_t>(lhs));
}

#ifdef _MSC_VER
#  pragma warning(pop)
#endif

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator+(const uint128_t lhs, const UnsignedInteger rhs) noexcept
{
    return impl::default_add(lhs, static_cast<std::uint64_t>(rhs));
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator+(const UnsignedInteger lhs, const uint128_t rhs) noexcept
{
    return impl::default_add(rhs, static_cast<std::uint64_t>(lhs));
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr uint128_t operator+(const uint128_t lhs, const uint128_t rhs) noexcept
{
    return impl::default_add(lhs, rhs);
}

#if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)


BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t operator+(const uint128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return impl::default_add(lhs, static_cast<uint128_t>(rhs));
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t operator+(const detail::builtin_i128 lhs, const uint128_t rhs) noexcept
{
    return impl::default_add(static_cast<uint128_t>(lhs), rhs);
}


BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t operator+(const uint128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return impl::default_add(lhs, static_cast<uint128_t>(rhs));
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t operator+(const detail::builtin_u128 lhs, const uint128_t rhs) noexcept
{
    return impl::default_add(static_cast<uint128_t>(lhs), rhs);
}

#endif // BOOST_INT128_HAS_INT128

template <BOOST_INT128_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t& uint128_t::operator+=(const Integer rhs) noexcept
{
    *this = *this + rhs;
    return *this;
}

BOOST_INT128_HOST_DEVICE constexpr uint128_t& uint128_t::operator+=(const uint128_t rhs) noexcept
{
    *this = *this + rhs;
    return *this;
}

#ifdef BOOST_INT128_HAS_MSVC_INT128

template <BOOST_INT128_128BIT_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE inline uint128_t& uint128_t::operator+=(const Integer rhs) noexcept
{
    *this = *this + rhs;
    return *this;
}

#endif // BOOST_INT128_HAS_MSVC_INT128


//=====================================
// Subtraction Operator
//=====================================

#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable : 4146)
#endif

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator-(const uint128_t lhs, const SignedInteger rhs) noexcept
{
    return rhs < 0 ? impl::default_add(lhs, -static_cast<std::uint64_t>(rhs)) :
                     impl::default_sub(lhs, static_cast<std::uint64_t>(rhs));
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator-(const SignedInteger lhs, const uint128_t rhs) noexcept
{
    return lhs < 0 ? impl::default_sub(-rhs, -static_cast<std::uint64_t>(lhs)) :
                     impl::default_add(-rhs, static_cast<std::uint64_t>(lhs));
}

#ifdef _MSC_VER
#  pragma warning(pop)
#endif

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator-(const uint128_t lhs, const UnsignedInteger rhs) noexcept
{
    return impl::default_sub(lhs, static_cast<std::uint64_t>(rhs));
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator-(const UnsignedInteger lhs, const uint128_t rhs) noexcept
{
    return impl::default_add(-rhs, static_cast<std::uint64_t>(lhs));
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr uint128_t operator-(const uint128_t lhs, const uint128_t rhs) noexcept
{
    return impl::default_sub(lhs, rhs);
}

#if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)


BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t operator-(const uint128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs - static_cast<uint128_t>(rhs);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t operator-(const detail::builtin_i128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) - rhs;
}


BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t operator-(const uint128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return lhs - static_cast<uint128_t>(rhs);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t operator-(const detail::builtin_u128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) - rhs;
}

#endif // BOOST_INT128_HAS_INT128

template <BOOST_INT128_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t& uint128_t::operator-=(const Integer rhs) noexcept
{
    *this = *this - rhs;
    return *this;
}

BOOST_INT128_HOST_DEVICE constexpr uint128_t& uint128_t::operator-=(const uint128_t rhs) noexcept
{
    *this = *this - rhs;
    return *this;
}

#ifdef BOOST_INT128_HAS_MSVC_INT128

template <BOOST_INT128_128BIT_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE inline uint128_t& uint128_t::operator-=(const Integer rhs) noexcept
{
    *this = *this - rhs;
    return *this;
}

#endif // BOOST_INT128_HAS_MSVC_INT128

//=====================================
// Multiplication Operator
//=====================================

#if defined(__GNUC__) && __GNUC__ >= 8
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wclass-memaccess"
#endif

namespace detail {

#if defined(_M_AMD64) && !defined(__GNUC__)

BOOST_INT128_HOST_DEVICE BOOST_INT128_FORCE_INLINE uint128_t msvc_mul(const uint128_t lhs, const uint128_t rhs) noexcept
{
    uint128_t result {};
    result.low = _umul128(lhs.low, rhs.low, &result.high);
    result.high += lhs.low * rhs.high;
    result.high += lhs.high * rhs.low;

    return result;
}

BOOST_INT128_HOST_DEVICE BOOST_INT128_FORCE_INLINE uint128_t msvc_mul(const uint128_t lhs, const std::uint64_t rhs) noexcept
{
    uint128_t result {};
    result.low = _umul128(lhs.low, rhs, &result.high);
    result.high += lhs.high * rhs;

    return result;
}

BOOST_INT128_HOST_DEVICE BOOST_INT128_FORCE_INLINE uint128_t msvc_mul(const uint128_t lhs, const std::uint32_t rhs) noexcept
{
    uint128_t result {};
    result.low = _umul128(lhs.low, static_cast<std::uint64_t>(rhs), &result.high);
    result.high += lhs.high * static_cast<std::uint64_t>(rhs);

    return result;
}

#elif defined(_M_ARM64)

BOOST_INT128_HOST_DEVICE BOOST_INT128_FORCE_INLINE uint128_t msvc_mul(const uint128_t lhs, const uint128_t rhs) noexcept
{
    const auto low_low{lhs.low * rhs.low};
    const auto high_low_low{__umulh(lhs.low, rhs.low)};

    const auto low_high{lhs.low * rhs.high};
    const auto high_low{lhs.high * rhs.low};

    const auto high{high_low + low_high + high_low_low};

    return {high, low_low};
}

BOOST_INT128_HOST_DEVICE BOOST_INT128_FORCE_INLINE uint128_t msvc_mul(const uint128_t lhs, const std::uint64_t rhs) noexcept
{
    const auto low{lhs.low * rhs};
    const auto high{__umulh(lhs.low, rhs) + (lhs.high * rhs)};

    return {high, low};
}

BOOST_INT128_HOST_DEVICE BOOST_INT128_FORCE_INLINE uint128_t msvc_mul(const uint128_t lhs, const std::uint32_t rhs) noexcept
{
    const auto low{lhs.low * rhs};
    const auto high{__umulh(lhs.low, static_cast<std::uint64_t>(rhs)) + (lhs.high * rhs)};

    return {high, low};
}

#endif // MSVC implementations

template <typename UnsignedInteger>
BOOST_INT128_HOST_DEVICE BOOST_INT128_FORCE_INLINE constexpr uint128_t default_mul(const uint128_t lhs, const UnsignedInteger rhs) noexcept
{
    #if (defined(__aarch64__) || defined(__x86_64__) || defined(__PPC__) || defined(__powerpc__)) && defined(__GNUC__) && defined(BOOST_INT128_HAS_INT128)

    #  if !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION)

    if (!BOOST_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        detail::builtin_u128 new_lhs {};
        detail::builtin_u128 new_rhs {};

        std::memcpy(&new_lhs, &lhs, sizeof(uint128_t));
        std::memcpy(&new_rhs, &rhs, sizeof(UnsignedInteger));

        const auto res {new_lhs * new_rhs};

        uint128_t library_res {};

        std::memcpy(&library_res, &res, sizeof(uint128_t));

        return library_res;
    }

    #  elif defined(BOOST_INT128_HAS_INT128)
    #    define BOOST_INT128_HIDE_MUL

        return static_cast<uint128_t>(static_cast<detail::builtin_u128>(lhs) * static_cast<detail::builtin_u128>(rhs));

    #  endif

    // s390x intentionally falls through to the synthetic low_word_mul below. Casting to builtin_u128
    // makes GCC reconstruct the value through a vector-unit stack round-trip that is several times
    // slower, and the memcpy path is unsafe for the narrow (scalar rhs) overloads on big-endian.
    #elif ((defined(_M_AMD64) && !defined(__GNUC__)) || defined(_M_ARM64)) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION)

    if (!BOOST_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        return msvc_mul(lhs, rhs);
    }

    #endif

    // We need to hide this if we use a non-const eval method above to avoid a litany of cross-platform warnings
    #ifndef BOOST_INT128_HIDE_MUL

    return low_word_mul<uint128_t>(lhs, rhs);

    #else
    #undef BOOST_INT128_HIDE_MUL
    #endif //BOOST_INT128_HIDE_MUL
}

} // namespace detail

#if defined(__GNUC__) && __GNUC__ >= 8
#  pragma GCC diagnostic pop
#endif

#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable : 4146)
#endif

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator*(const uint128_t lhs, const SignedInteger rhs) noexcept
{
    using eval_type = detail::evaluation_type_t<SignedInteger>;

    const auto abs_rhs {rhs < 0 ? -static_cast<eval_type>(rhs) : static_cast<eval_type>(rhs)};
    const auto res {detail::default_mul(lhs, abs_rhs)};

    return rhs < 0 ? -res : res;
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator*(const SignedInteger lhs, const uint128_t rhs) noexcept
{
    using eval_type = detail::evaluation_type_t<SignedInteger>;

    const auto abs_lhs {lhs < 0 ? -static_cast<eval_type>(lhs) : static_cast<eval_type>(lhs)};
    const auto res {detail::default_mul(rhs, abs_lhs)};

    return lhs < 0 ? -res : res;
}

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator*(const uint128_t lhs, const UnsignedInteger rhs) noexcept
{
    return detail::default_mul(lhs, static_cast<std::uint64_t>(rhs));
}

#ifdef _MSC_VER
#  pragma warning(pop)
#endif

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator*(const UnsignedInteger lhs, const uint128_t rhs) noexcept
{
    return detail::default_mul(rhs, static_cast<std::uint64_t>(lhs));
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr uint128_t operator*(const uint128_t lhs, const uint128_t rhs) noexcept
{
    return detail::default_mul(lhs, rhs);
}

#if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)


BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t operator*(const uint128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    const detail::builtin_u128 rhs_bits {static_cast<detail::builtin_u128>(rhs)};
    const bool rhs_negative {static_cast<std::int64_t>(static_cast<std::uint64_t>(rhs_bits >> static_cast<detail::builtin_u128>(64U))) < 0};
    const uint128_t rhs_u {rhs_bits};
    const uint128_t abs_rhs {rhs_negative ? -rhs_u : rhs_u};
    const uint128_t res {lhs * abs_rhs};

    return rhs_negative ? -res : res;
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t operator*(const detail::builtin_i128 lhs, const uint128_t rhs) noexcept
{
    const detail::builtin_u128 lhs_bits {static_cast<detail::builtin_u128>(lhs)};
    const bool lhs_negative {static_cast<std::int64_t>(static_cast<std::uint64_t>(lhs_bits >> static_cast<detail::builtin_u128>(64U))) < 0};
    const uint128_t lhs_u {lhs_bits};
    const uint128_t abs_lhs {lhs_negative ? -lhs_u : lhs_u};
    const uint128_t res {abs_lhs * rhs};

    return lhs_negative ? -res : res;
}


BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t operator*(const uint128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return lhs * static_cast<uint128_t>(rhs);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t operator*(const detail::builtin_u128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) * rhs;
}

#endif // BOOST_INT128_HAS_INT128

template <BOOST_INT128_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t& uint128_t::operator*=(const Integer rhs) noexcept
{
    *this = *this * rhs;
    return *this;
}

BOOST_INT128_HOST_DEVICE constexpr uint128_t& uint128_t::operator*=(const uint128_t rhs) noexcept
{
    *this = *this * rhs;
    return *this;
}

#ifdef BOOST_INT128_HAS_MSVC_INT128

template <BOOST_INT128_128BIT_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE inline uint128_t& uint128_t::operator*=(const Integer rhs) noexcept
{
    *this = *this * rhs;
    return *this;
}

#endif // BOOST_INT128_HAS_MSVC_INT128

//=====================================
// Division Operator
//=====================================

// For div we need forward declarations since we mix and match the arguments
BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator/(uint128_t lhs, SignedInteger rhs) noexcept;

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator/(SignedInteger lhs, uint128_t rhs) noexcept;

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator/(uint128_t lhs, UnsignedInteger rhs) noexcept;

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator/(UnsignedInteger lhs, uint128_t rhs) noexcept;

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr uint128_t operator/(uint128_t lhs, uint128_t rhs) noexcept;

template <BOOST_INT128_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator/(const uint128_t lhs, const SignedInteger rhs) noexcept
{
    using eval_type = detail::evaluation_type_t<SignedInteger>;
    return rhs < 0 ? lhs / static_cast<uint128_t>(rhs) : lhs / static_cast<eval_type>(rhs);
}

template <BOOST_INT128_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator/(const SignedInteger lhs, const uint128_t rhs) noexcept
{
    using eval_type = detail::evaluation_type_t<SignedInteger>;
    return lhs < 0 ? static_cast<uint128_t>(lhs) / rhs : static_cast<eval_type>(lhs) / rhs;
}

template <BOOST_INT128_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator/(const uint128_t lhs, const UnsignedInteger rhs) noexcept
{
    using eval_type = detail::evaluation_type_t<UnsignedInteger>;

    if (BOOST_INT128_UNLIKELY(rhs == 0U))
    {
        return {0, 0};
    }

    if (lhs < rhs)
    {
        return {0, 0};
    }

    uint128_t quotient {};

    detail::one_word_div(lhs, static_cast<eval_type>(rhs), quotient);

    return quotient;
}

template <BOOST_INT128_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator/(const UnsignedInteger lhs, const uint128_t rhs) noexcept
{
    using eval_type = detail::evaluation_type_t<UnsignedInteger>;

    if (BOOST_INT128_UNLIKELY(rhs == 0U))
    {
        return {0, 0};
    }

    if (lhs < rhs)
    {
        return {0, 0};
    }

    return {0, static_cast<eval_type>(lhs) / rhs.low};
}

BOOST_INT128_HOST_DEVICE constexpr uint128_t operator/(const uint128_t lhs, const uint128_t rhs) noexcept
{
    if (BOOST_INT128_UNLIKELY(rhs == 0U))
    {
        return {0, 0};
    }

    if (lhs < rhs)
    {
        return {0, 0};
    }

    // A divisor that fits in 64 bits is handled by the hardware-accelerated narrow path. This
    // beats the native 128/128 divide for this common case on every platform (it avoids the
    // out-of-line __udivti3 call on GCC/Clang and uses divq / _udiv128 directly where present).
    if (rhs.high == 0U)
    {
        if (lhs.high == 0U)
        {
            return {0, lhs.low / rhs.low};
        }

        uint128_t quotient {};
        detail::one_word_div(lhs, rhs.low, quotient);
        return quotient;
    }

    #if defined(BOOST_INT128_HAS_INT128) && !defined(__s390__) && !defined(__s390x__)

    return static_cast<uint128_t>(static_cast<detail::builtin_u128>(lhs) / static_cast<detail::builtin_u128>(rhs));

    #else

    return detail::knuth_div(lhs, rhs);

    #endif
}

#if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t operator/(const uint128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return lhs / static_cast<uint128_t>(rhs);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t operator/(const detail::builtin_u128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) / rhs;
}


BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t operator/(const uint128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs / static_cast<uint128_t>(rhs);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t operator/(const detail::builtin_i128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) / rhs;
}


#endif // BOOST_INT128_HAS_INT128

template <BOOST_INT128_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t& uint128_t::operator/=(const Integer rhs) noexcept
{
    *this = *this / rhs;
    return *this;
}

BOOST_INT128_HOST_DEVICE constexpr uint128_t& uint128_t::operator/=(const uint128_t rhs) noexcept
{
    *this = *this / rhs;
    return *this;
}

#ifdef BOOST_INT128_HAS_MSVC_INT128

template <BOOST_INT128_128BIT_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE inline uint128_t& uint128_t::operator/=(const Integer rhs) noexcept
{
    *this = *this / rhs;
    return *this;
}

#endif // BOOST_INT128_HAS_MSVC_INT128

//=====================================
// Modulo Operator
//=====================================

// For div we need forward declarations since we mix and match the arguments
BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator%(uint128_t lhs, SignedInteger rhs) noexcept;

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator%(SignedInteger lhs, uint128_t rhs) noexcept;

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator%(uint128_t lhs, UnsignedInteger rhs) noexcept;

BOOST_INT128_EXPORT template <BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator%(UnsignedInteger lhs, uint128_t rhs) noexcept;

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr uint128_t operator%(uint128_t lhs, uint128_t rhs) noexcept;

template <BOOST_INT128_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator%(const uint128_t lhs, const SignedInteger rhs) noexcept
{
    using eval_type = detail::evaluation_type_t<SignedInteger>;
    return rhs < 0 ? lhs % static_cast<uint128_t>(rhs) : lhs % static_cast<eval_type>(rhs);
}

template <BOOST_INT128_SIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator%(const SignedInteger lhs, const uint128_t rhs) noexcept
{
    using eval_type = detail::evaluation_type_t<SignedInteger>;
    return lhs < 0 ? static_cast<uint128_t>(lhs) % rhs : static_cast<eval_type>(lhs) % rhs;
}

template <BOOST_INT128_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator%(const uint128_t lhs, const UnsignedInteger rhs) noexcept
{
    using eval_type = detail::evaluation_type_t<UnsignedInteger>;

    if (BOOST_INT128_UNLIKELY(rhs == 0U))
    {
        return {0, 0};
    }

    if (lhs.high != 0)
    {
        uint128_t quotient {};
        uint128_t remainder {};

        detail::one_word_div(lhs, static_cast<eval_type>(rhs), quotient, remainder);

        return remainder;
    }
    else
    {
        return {0, lhs.low % rhs};
    }
}

template <BOOST_INT128_UNSIGNED_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator%(const UnsignedInteger lhs, const uint128_t rhs) noexcept
{
    using eval_type = detail::evaluation_type_t<UnsignedInteger>;

    if (BOOST_INT128_UNLIKELY(rhs == 0U))
    {
        return {0, 0};
    }
    else if (rhs > lhs)
    {
        return lhs;
    }

    return {0, static_cast<eval_type>(lhs) % rhs.low};
}

BOOST_INT128_HOST_DEVICE constexpr uint128_t operator%(const uint128_t lhs, const uint128_t rhs) noexcept
{
    if (BOOST_INT128_UNLIKELY(rhs == 0U))
    {
        return {0, 0};
    }
    if (rhs > lhs)
    {
        return lhs;
    }

    // A divisor that fits in 64 bits is handled by the hardware-accelerated narrow path, which
    // beats the native 128/128 divide for this common case on every platform.
    if (rhs.high == 0U)
    {
        if (lhs.high == 0U)
        {
            return {0, lhs.low % rhs.low};
        }

        uint128_t quotient {};
        uint128_t remainder {};
        detail::one_word_div(lhs, rhs.low, quotient, remainder);
        return remainder;
    }

    #if defined(BOOST_INT128_HAS_INT128) && !defined(__s390__) && !defined(__s390x__)

    return static_cast<uint128_t>(static_cast<detail::builtin_u128>(lhs) % static_cast<detail::builtin_u128>(rhs));

    #else

    uint128_t remainder {};
    detail::knuth_div(lhs, rhs, remainder);
    return remainder;

    #endif
}

#if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t operator%(const uint128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return lhs % static_cast<uint128_t>(rhs);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t operator%(const detail::builtin_u128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) % rhs;
}


BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t operator%(const uint128_t lhs, const detail::builtin_i128 rhs) noexcept
{
    return lhs % static_cast<uint128_t>(rhs);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t operator%(const detail::builtin_i128 lhs, const uint128_t rhs) noexcept
{
    return static_cast<uint128_t>(lhs) % rhs;
}


#endif // BOOST_INT128_HAS_INT128

template <BOOST_INT128_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE constexpr uint128_t& uint128_t::operator%=(const Integer rhs) noexcept
{
    *this = *this % rhs;
    return *this;
}

BOOST_INT128_HOST_DEVICE constexpr uint128_t& uint128_t::operator%=(const uint128_t rhs) noexcept
{
    *this = *this % rhs;
    return *this;
}

#ifdef BOOST_INT128_HAS_MSVC_INT128

template <BOOST_INT128_128BIT_INTEGER_CONCEPT>
BOOST_INT128_HOST_DEVICE inline uint128_t& uint128_t::operator%=(const Integer rhs) noexcept
{
    * this = *this % rhs;
    return *this;
}

#endif // BOOST_INT128_HAS_MSVC_INT128

namespace detail {

template <bool>
class numeric_limits_impl_u128
{
public:

        // Member constants
    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = false;
    static constexpr bool is_integer = true;
    static constexpr bool is_exact = true;
    static constexpr bool has_infinity = false;
    static constexpr bool has_quiet_NaN = false;
    static constexpr bool has_signaling_NaN = false;

    // C++23 deprecated the following two members
    #if defined(__GNUC__) && __cplusplus > 202002L
    #  pragma GCC diagnostic push
    #  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    #elif defined(_MSC_VER)
    #  pragma warning(push)
    #  pragma warning(disable:4996)
    #endif

    static constexpr std::float_denorm_style has_denorm = std::denorm_absent;
    static constexpr bool has_denorm_loss = false;

    #if defined(__GNUC__) && __cplusplus > 202002L
    #  pragma GCC diagnostic pop
    #elif defined(_MSC_VER)
    #  pragma warning(pop)
    #endif

    static constexpr std::float_round_style round_style = std::round_toward_zero;
    static constexpr bool is_iec559 = false;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = true;
    static constexpr int digits = 128;
    static constexpr int digits10 = 38;
    static constexpr int max_digits10 = 0;
    static constexpr int radix = 2;
    static constexpr int min_exponent = 0;
    static constexpr int min_exponent10 = 0;
    static constexpr int max_exponent = 0;
    static constexpr int max_exponent10 = 0;
    static constexpr bool traps = std::numeric_limits<std::uint64_t>::traps;
    static constexpr bool tinyness_before = false;

    // Member functions
    BOOST_INT128_HOST_DEVICE static constexpr auto (min)        () -> boost::int128::uint128_t { return {0, 0}; }
    BOOST_INT128_HOST_DEVICE static constexpr auto lowest       () -> boost::int128::uint128_t { return {0, 0}; }
    BOOST_INT128_HOST_DEVICE static constexpr auto (max)        () -> boost::int128::uint128_t { return {UINT64_MAX, UINT64_MAX}; }
    BOOST_INT128_HOST_DEVICE static constexpr auto epsilon      () -> boost::int128::uint128_t { return {0, 0}; }
    BOOST_INT128_HOST_DEVICE static constexpr auto round_error  () -> boost::int128::uint128_t { return {0, 0}; }
    BOOST_INT128_HOST_DEVICE static constexpr auto infinity     () -> boost::int128::uint128_t { return {0, 0}; }
    BOOST_INT128_HOST_DEVICE static constexpr auto quiet_NaN    () -> boost::int128::uint128_t { return {0, 0}; }
    BOOST_INT128_HOST_DEVICE static constexpr auto signaling_NaN() -> boost::int128::uint128_t { return {0, 0}; }
    BOOST_INT128_HOST_DEVICE static constexpr auto denorm_min   () -> boost::int128::uint128_t { return {0, 0}; }
};

#if !defined(__cpp_inline_variables) || __cpp_inline_variables < 201606L

template <bool b> constexpr bool numeric_limits_impl_u128<b>::is_specialized;
template <bool b> constexpr bool numeric_limits_impl_u128<b>::is_signed;
template <bool b> constexpr bool numeric_limits_impl_u128<b>::is_integer;
template <bool b> constexpr bool numeric_limits_impl_u128<b>::is_exact;
template <bool b> constexpr bool numeric_limits_impl_u128<b>::has_infinity;
template <bool b> constexpr bool numeric_limits_impl_u128<b>::has_quiet_NaN;
template <bool b> constexpr bool numeric_limits_impl_u128<b>::has_signaling_NaN;

// These members were deprecated in C++23; suppress the deprecation warning rather
// than dropping the definitions.
#if defined(__GNUC__) && __cplusplus > 202002L
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable:4996)
#endif

template <bool b> constexpr std::float_denorm_style numeric_limits_impl_u128<b>::has_denorm;
template <bool b> constexpr bool numeric_limits_impl_u128<b>::has_denorm_loss;

#if defined(__GNUC__) && __cplusplus > 202002L
#  pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#  pragma warning(pop)
#endif

template <bool b> constexpr std::float_round_style numeric_limits_impl_u128<b>::round_style;
template <bool b> constexpr bool numeric_limits_impl_u128<b>::is_iec559;
template <bool b> constexpr bool numeric_limits_impl_u128<b>::is_bounded;
template <bool b> constexpr bool numeric_limits_impl_u128<b>::is_modulo;
template <bool b> constexpr int numeric_limits_impl_u128<b>::digits;
template <bool b> constexpr int numeric_limits_impl_u128<b>::digits10;
template <bool b> constexpr int numeric_limits_impl_u128<b>::max_digits10;
template <bool b> constexpr int numeric_limits_impl_u128<b>::radix;
template <bool b> constexpr int numeric_limits_impl_u128<b>::min_exponent;
template <bool b> constexpr int numeric_limits_impl_u128<b>::min_exponent10;
template <bool b> constexpr int numeric_limits_impl_u128<b>::max_exponent;
template <bool b> constexpr int numeric_limits_impl_u128<b>::max_exponent10;
template <bool b> constexpr bool numeric_limits_impl_u128<b>::traps;
template <bool b> constexpr bool numeric_limits_impl_u128<b>::tinyness_before;

#endif // !defined(__cpp_inline_variables) || __cpp_inline_variables < 201606L


} // namespace detail

} // namespace int128
} // namespace boost

namespace std {

#ifdef __clang__
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wmismatched-tags"
#endif

template <>
class numeric_limits<boost::int128::uint128_t> :
    public boost::int128::detail::numeric_limits_impl_u128<true> {};

#ifdef __clang__
#  pragma clang diagnostic pop
#endif

} // namespace std

#endif //BOOST_INT128_DETAIL_UINT128_IMP_HPP
// ===== END boost/int128/detail/uint128_imp.hpp =====
// ===== BEGIN boost/int128/detail/conversions.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_CONVERSIONS_HPP
#define BOOST_INT128_CONVERSIONS_HPP

// [amalgamate] skipped duplicate include of boost/int128/detail/int128_imp.hpp
// [amalgamate] skipped duplicate include of boost/int128/detail/uint128_imp.hpp

namespace boost {
namespace int128 {

namespace detail {

template <typename T>
struct valid_overload
{
    static constexpr bool value = std::is_same<T, uint128_t>::value || std::is_same<T, int128_t>::value;
};

template <typename T>
BOOST_INT128_INLINE_CONSTEXPR bool is_valid_overload_v = valid_overload<T>::value;

} // namespace detail

#if BOOST_INT128_ENDIAN_LITTLE_BYTE

BOOST_INT128_HOST_DEVICE constexpr int128_t::int128_t(const uint128_t& v) noexcept : low {v.low}, high {static_cast<std::int64_t>(v.high)} {}

BOOST_INT128_HOST_DEVICE constexpr uint128_t::uint128_t(const int128_t& v) noexcept : low {v.low}, high {static_cast<std::uint64_t>(v.high)} {}

#else

BOOST_INT128_HOST_DEVICE constexpr int128_t::int128_t(const uint128_t& v) noexcept : high {static_cast<std::int64_t>(v.high)}, low {v.low} {}

BOOST_INT128_HOST_DEVICE constexpr uint128_t::uint128_t(const int128_t& v) noexcept : high {static_cast<std::uint64_t>(v.high)}, low {v.low} {}

#endif // BOOST_INT128_ENDIAN_LITTLE_BYTE

//=====================================
// Comparison Operators
//=====================================

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4127)
#endif

template <typename T, typename U, std::enable_if_t<detail::is_valid_overload_v<T> && detail::is_valid_overload_v<U> && !std::is_same<T, U>::value, bool> = true>
BOOST_INT128_HOST_DEVICE constexpr bool operator==(const T lhs, const U rhs) noexcept
{
    return static_cast<uint128_t>(lhs) == static_cast<uint128_t>(rhs);
}

template <typename T, typename U, std::enable_if_t<detail::is_valid_overload_v<T> && detail::is_valid_overload_v<U> && !std::is_same<T, U>::value, bool> = true>
BOOST_INT128_HOST_DEVICE constexpr bool operator!=(const T lhs, const U rhs) noexcept
{
    return static_cast<uint128_t>(lhs) != static_cast<uint128_t>(rhs);
}

template <typename T, typename U, std::enable_if_t<detail::is_valid_overload_v<T> && detail::is_valid_overload_v<U> && !std::is_same<T, U>::value, bool> = true>
BOOST_INT128_HOST_DEVICE constexpr bool operator<(const T lhs, const U rhs) noexcept
{
    return static_cast<uint128_t>(lhs) < static_cast<uint128_t>(rhs);
}

template <typename T, typename U, std::enable_if_t<detail::is_valid_overload_v<T> && detail::is_valid_overload_v<U> && !std::is_same<T, U>::value, bool> = true>
BOOST_INT128_HOST_DEVICE constexpr bool operator<=(const T lhs, const U rhs) noexcept
{
    return static_cast<uint128_t>(lhs) <= static_cast<uint128_t>(rhs);
}

template <typename T, typename U, std::enable_if_t<detail::is_valid_overload_v<T> && detail::is_valid_overload_v<U> && !std::is_same<T, U>::value, bool> = true>
BOOST_INT128_HOST_DEVICE constexpr bool operator>(const T lhs, const U rhs) noexcept
{
    return static_cast<uint128_t>(lhs) > static_cast<uint128_t>(rhs);
}

template <typename T, typename U, std::enable_if_t<detail::is_valid_overload_v<T> && detail::is_valid_overload_v<U> && !std::is_same<T, U>::value, bool> = true>
BOOST_INT128_HOST_DEVICE constexpr bool operator>=(const T lhs, const U rhs) noexcept
{
    return static_cast<uint128_t>(lhs) >= static_cast<uint128_t>(rhs);
}

//=====================================
// Arithmetic Operators
//=====================================

template <typename T, typename U, std::enable_if_t<detail::is_valid_overload_v<T> && detail::is_valid_overload_v<U> && !std::is_same<T, U>::value, bool> = true>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator+(const T lhs, const U rhs) noexcept
{
    return static_cast<uint128_t>(lhs) + static_cast<uint128_t>(rhs);
}

template <typename T, typename U, std::enable_if_t<detail::is_valid_overload_v<T> && detail::is_valid_overload_v<U> && !std::is_same<T, U>::value, bool> = true>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator-(const T lhs, const U rhs) noexcept
{
    return static_cast<uint128_t>(lhs) - static_cast<uint128_t>(rhs);
}

template <typename T, typename U, std::enable_if_t<detail::is_valid_overload_v<T> && detail::is_valid_overload_v<U> && !std::is_same<T, U>::value, bool> = true>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator*(const T lhs, const U rhs) noexcept
{
    return static_cast<uint128_t>(lhs) * static_cast<uint128_t>(rhs);
}

template <typename T, typename U, std::enable_if_t<detail::is_valid_overload_v<T> && detail::is_valid_overload_v<U> && !std::is_same<T, U>::value, bool> = true>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator/(const T lhs, const U rhs) noexcept
{
    return static_cast<uint128_t>(lhs) / static_cast<uint128_t>(rhs);
}

template <typename T, typename U, std::enable_if_t<detail::is_valid_overload_v<T> && detail::is_valid_overload_v<U> && !std::is_same<T, U>::value, bool> = true>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator%(const T lhs, const U rhs) noexcept
{
    return static_cast<uint128_t>(lhs) % static_cast<uint128_t>(rhs);
}

//=====================================
// Cross-type Bitwise Operators
//=====================================

template <typename T, typename U, std::enable_if_t<detail::is_valid_overload_v<T> && detail::is_valid_overload_v<U> && !std::is_same<T, U>::value, bool> = true>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator|(const T lhs, const U rhs) noexcept
{
    return static_cast<uint128_t>(lhs) | static_cast<uint128_t>(rhs);
}

template <typename T, typename U, std::enable_if_t<detail::is_valid_overload_v<T> && detail::is_valid_overload_v<U> && !std::is_same<T, U>::value, bool> = true>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator&(const T lhs, const U rhs) noexcept
{
    return static_cast<uint128_t>(lhs) & static_cast<uint128_t>(rhs);
}

template <typename T, typename U, std::enable_if_t<detail::is_valid_overload_v<T> && detail::is_valid_overload_v<U> && !std::is_same<T, U>::value, bool> = true>
BOOST_INT128_HOST_DEVICE constexpr uint128_t operator^(const T lhs, const U rhs) noexcept
{
    return static_cast<uint128_t>(lhs) ^ static_cast<uint128_t>(rhs);
}

//=====================================
// Cross-type Shift Operators
//=====================================

BOOST_INT128_HOST_DEVICE constexpr int128_t operator<<(const int128_t lhs, const uint128_t rhs) noexcept
{
    return lhs << static_cast<int128_t>(rhs);
}

BOOST_INT128_HOST_DEVICE constexpr uint128_t operator<<(const uint128_t lhs, const int128_t rhs) noexcept
{
    return lhs << static_cast<uint128_t>(rhs);
}

BOOST_INT128_HOST_DEVICE constexpr int128_t operator>>(const int128_t lhs, const uint128_t rhs) noexcept
{
    return lhs >> static_cast<int128_t>(rhs);
}

BOOST_INT128_HOST_DEVICE constexpr uint128_t operator>>(const uint128_t lhs, const int128_t rhs) noexcept
{
    return lhs >> static_cast<uint128_t>(rhs);
}

//=====================================
// int128_t with builtin unsigned __int128 comparison operators
//
// These live here (not in int128_imp.hpp) 
// to avoid C++20 rewritten-candidate ambiguity on MSVC
//=====================================

#if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)

BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR bool operator==(const int128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return static_cast<uint128_t>(lhs) == rhs;
}

BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR bool operator==(const detail::builtin_u128 lhs, const int128_t rhs) noexcept
{
    return lhs == static_cast<uint128_t>(rhs);
}

BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR bool operator!=(const int128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return static_cast<uint128_t>(lhs) != rhs;
}

BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR bool operator!=(const detail::builtin_u128 lhs, const int128_t rhs) noexcept
{
    return lhs != static_cast<uint128_t>(rhs);
}

BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR bool operator<(const int128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return static_cast<uint128_t>(lhs) < rhs;
}

BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR bool operator<(const detail::builtin_u128 lhs, const int128_t rhs) noexcept
{
    return lhs < static_cast<uint128_t>(rhs);
}

BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR bool operator<=(const int128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return static_cast<uint128_t>(lhs) <= rhs;
}

BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR bool operator<=(const detail::builtin_u128 lhs, const int128_t rhs) noexcept
{
    return lhs <= static_cast<uint128_t>(rhs);
}

BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR bool operator>(const int128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return static_cast<uint128_t>(lhs) > rhs;
}

BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR bool operator>(const detail::builtin_u128 lhs, const int128_t rhs) noexcept
{
    return lhs > static_cast<uint128_t>(rhs);
}

BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR bool operator>=(const int128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return static_cast<uint128_t>(lhs) >= rhs;
}

BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR bool operator>=(const detail::builtin_u128 lhs, const int128_t rhs) noexcept
{
    return lhs >= static_cast<uint128_t>(rhs);
}

#endif // BOOST_INT128_HAS_INT128

//=====================================
// int128_t with builtin unsigned __int128 binary operators
//=====================================

#if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)

BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t operator|(const int128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return static_cast<uint128_t>(lhs) | rhs;
}

BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t operator|(const detail::builtin_u128 lhs, const int128_t rhs) noexcept
{
    return lhs | static_cast<uint128_t>(rhs);
}

BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t operator&(const int128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return static_cast<uint128_t>(lhs) & rhs;
}

BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t operator&(const detail::builtin_u128 lhs, const int128_t rhs) noexcept
{
    return lhs & static_cast<uint128_t>(rhs);
}

BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t operator^(const int128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return static_cast<uint128_t>(lhs) ^ rhs;
}

BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t operator^(const detail::builtin_u128 lhs, const int128_t rhs) noexcept
{
    return lhs ^ static_cast<uint128_t>(rhs);
}

BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t operator+(const int128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return static_cast<uint128_t>(lhs) + rhs;
}

BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t operator+(const detail::builtin_u128 lhs, const int128_t rhs) noexcept
{
    return lhs + static_cast<uint128_t>(rhs);
}

BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t operator-(const int128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return static_cast<uint128_t>(lhs) - rhs;
}

BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t operator-(const detail::builtin_u128 lhs, const int128_t rhs) noexcept
{
    return lhs - static_cast<uint128_t>(rhs);
}

BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t operator*(const int128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return static_cast<uint128_t>(lhs) * rhs;
}

BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t operator*(const detail::builtin_u128 lhs, const int128_t rhs) noexcept
{
    return lhs * static_cast<uint128_t>(rhs);
}

BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t operator/(const int128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return static_cast<uint128_t>(lhs) / rhs;
}

BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t operator/(const detail::builtin_u128 lhs, const int128_t rhs) noexcept
{
    return lhs / static_cast<uint128_t>(rhs);
}

BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t operator%(const int128_t lhs, const detail::builtin_u128 rhs) noexcept
{
    return static_cast<uint128_t>(lhs) % rhs;
}

BOOST_INT128_HOST_DEVICE BOOST_INT128_BUILTIN_CONSTEXPR uint128_t operator%(const detail::builtin_u128 lhs, const int128_t rhs) noexcept
{
    return lhs % static_cast<uint128_t>(rhs);
}

#endif // BOOST_INT128_HAS_INT128

#ifdef _MSC_VER
#pragma warning(pop)
#endif

} // namespace int128
} // namespace boost

#endif // BOOST_INT128_CONVERSIONS_HPP
// ===== END boost/int128/detail/conversions.hpp =====

#endif // BOOST_INT128_INT128_HPP
// ===== END boost/int128/int128.hpp =====
// ===== BEGIN boost/int128/bit.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_BIT_HPP
#define BOOST_INT128_BIT_HPP

// [amalgamate] skipped duplicate include of boost/int128/int128.hpp
// [amalgamate] skipped duplicate include of boost/int128/detail/config.hpp
// [amalgamate] skipped duplicate include of boost/int128/detail/clz.hpp
// ===== BEGIN boost/int128/detail/ctz.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_DETAIL_CTZ_HPP
#define BOOST_INT128_DETAIL_CTZ_HPP

// [amalgamate] skipped duplicate include of boost/int128/detail/config.hpp

#ifndef BOOST_INT128_BUILD_MODULE

#include <limits>
#include <cstdint>

#endif

namespace boost {
namespace int128 {
namespace detail {

namespace impl {

#if BOOST_INT128_HAS_BUILTIN(__builtin_ctz) && !(defined(__CUDACC__) && defined(BOOST_INT128_ENABLE_CUDA))

constexpr int countr_impl(unsigned int x) noexcept
{
    return x ? __builtin_ctz(x) : std::numeric_limits<unsigned int>::digits;
}

constexpr int countr_impl(unsigned long x) noexcept
{
    return x ? __builtin_ctzl(x) : std::numeric_limits<unsigned long>::digits;
}

constexpr int countr_impl(unsigned long long x) noexcept
{
    return x ? __builtin_ctzll(x) : std::numeric_limits<unsigned long long>::digits;
}

#endif

#if !(defined(__CUDACC__) && defined(BOOST_INT128_ENABLE_CUDA))

BOOST_INT128_INLINE_CONSTEXPR int countr_mod37[37] = {
    32, 0, 1, 26, 2, 23, 27, 0,
    3, 16, 24, 30, 28, 11, 0, 13,
    4, 7, 17, 0, 25, 22, 31, 15,
    29, 10, 12, 6, 0, 21, 14, 9,
    5, 20, 8, 19, 18
};

#endif

#if defined(_MSC_VER) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION) && !BOOST_INT128_HAS_BUILTIN(__builtin_ctz)

#pragma warning(push)
#pragma warning(disable : 4146) // unary minus operator applied to unsigned type, result still unsigned

constexpr int countr_impl(std::uint32_t x) noexcept
{
    if (BOOST_INT128_IS_CONSTANT_EVALUATED(x))
    {
        return countr_mod37[(-x & x) % 37]; // LCOV_EXCL_LINE
    }
    else
    {
        unsigned long r {};

        if(_BitScanForward(&r, x))
        {
            return static_cast<int>(r);
        }
        else
        {
            return 32;
        }
    }
}

#pragma warning(pop)

#elif !BOOST_INT128_HAS_BUILTIN(__builtin_ctz) || (defined(__CUDACC__) && defined(BOOST_INT128_ENABLE_CUDA))

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4146) // unary minus operator applied to unsigned type, result still unsigned
#endif

BOOST_INT128_HOST_DEVICE constexpr int countr_impl(std::uint32_t x) noexcept
{
    #if defined(__CUDACC__) && defined(BOOST_INT128_ENABLE_CUDA)

    constexpr int countr_mod37[37] = {
        32, 0, 1, 26, 2, 23, 27, 0,
        3, 16, 24, 30, 28, 11, 0, 13,
        4, 7, 17, 0, 25, 22, 31, 15,
        29, 10, 12, 6, 0, 21, 14, 9,
        5, 20, 8, 19, 18
    };

    #endif

    return countr_mod37[(-x & x) % 37];
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif

#if (defined(_M_AMD64) || defined(_M_ARM64)) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION) && !BOOST_INT128_HAS_BUILTIN(__builtin_ctz) && !(defined(__CUDACC__) && defined(BOOST_INT128_ENABLE_CUDA))

constexpr int countr_impl(std::uint64_t x) noexcept
{
    if (BOOST_INT128_IS_CONSTANT_EVALUATED(x))
    {
        return static_cast<std::uint32_t>(x) != 0 ? countr_impl(static_cast<std::uint32_t>(x)) : countr_impl(static_cast<std::uint32_t>(x >> 32)) + 32; // LCOV_EXCL_LINE
    }
    else
    {
        unsigned long r {};

        if(_BitScanForward64(&r, x))
        {
            return static_cast<int>(r);
        }
        else
        {
            return 64;
        }
    }
}

#elif !BOOST_INT128_HAS_BUILTIN(__builtin_ctz) || (defined(__CUDACC__) && defined(BOOST_INT128_ENABLE_CUDA))

BOOST_INT128_HOST_DEVICE constexpr int countr_impl(std::uint64_t x) noexcept
{
    return static_cast<std::uint32_t>(x) != 0 ? countr_impl(static_cast<std::uint32_t>(x)) :
                                                countr_impl(static_cast<std::uint32_t>(x >> 32)) + 32;
}

#endif

} // namespace impl

template <typename T>
BOOST_INT128_HOST_DEVICE constexpr int countr_zero(T x) noexcept
{
    static_assert(std::numeric_limits<T>::is_integer && !std::numeric_limits<T>::is_signed,
                  "Can only count with unsigned integers");

    return impl::countr_impl(x);
}

} // namespace detail
} // namespace int128
} // namespace boost

#endif // BOOST_INT128_DETAIL_CTZ_HPP
// ===== END boost/int128/detail/ctz.hpp =====

namespace boost {
namespace int128 {

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr bool has_single_bit(const uint128_t x) noexcept
{
    return x && !(x & (x - 1U));
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr int countl_zero(const uint128_t x) noexcept
{
    return x.high == 0 ? 64 + detail::countl_zero(x.low) : detail::countl_zero(x.high);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr int countl_one(const uint128_t x) noexcept
{
    return countl_zero(~x);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr int bit_width(const uint128_t x) noexcept
{
    return x ? 128 - countl_zero(x) : 0;
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr uint128_t bit_ceil(const uint128_t x) noexcept
{
    return x <= 1U ? static_cast<uint128_t>(1) : static_cast<uint128_t>(1) << bit_width(x - 1U);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr uint128_t bit_floor(const uint128_t x) noexcept
{
    return x > 0U ? static_cast<uint128_t>(1) << (bit_width(x) - 1U) : static_cast<uint128_t>(0);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr int countr_zero(const uint128_t x) noexcept
{
    return x.low == 0 ? 64 + detail::countr_zero(x.high) : detail::countr_zero(x.low);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr int countr_one(const uint128_t x) noexcept
{
    return countr_zero(~x);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr uint128_t rotl(const uint128_t x, const int s) noexcept
{
    constexpr auto mask {127U};
    return x << (static_cast<unsigned>(s) & mask) | x >> (static_cast<unsigned>(-s) & mask);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr uint128_t rotr(const uint128_t x, const int s) noexcept
{
    constexpr auto mask {127U};
    return x >> (static_cast<unsigned>(s) & mask) | x << (static_cast<unsigned>(-s) & mask);
}

#if BOOST_INT128_HAS_BUILTIN(__builtin_popcountll) && !(defined(__CUDACC__) && defined(BOOST_INT128_ENABLE_CUDA))

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr int popcount(const uint128_t x) noexcept
{
    return __builtin_popcountll(x.high) + __builtin_popcountll(x.low);
}

#endif

namespace impl {

BOOST_INT128_TEST_EXPORT BOOST_INT128_HOST_DEVICE constexpr int popcount_impl(std::uint64_t x) noexcept
{
    x = x - ((x >> 1U) & UINT64_C(0x5555555555555555));
    x = (x & UINT64_C(0x3333333333333333)) + ((x >> 2U) & UINT64_C(0x3333333333333333));
    x = (x + (x >> 4U)) & UINT64_C(0x0F0F0F0F0F0F0F0F);

    return static_cast<int>((x * UINT64_C(0x0101010101010101)) >> 56U);
}

} // namespace impl

#if defined(_M_AMD64) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION) && !BOOST_INT128_HAS_BUILTIN(__builtin_popcountll)

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr int popcount(const uint128_t x) noexcept
{
    if (BOOST_INT128_IS_CONSTANT_EVALUATED(x))
    {
        return impl::popcount_impl(x.high) + impl::popcount_impl(x.low); // LCOV_EXCL_LINE
    }
    else
    {
        #ifdef __AVX__

        return static_cast<int>(_mm_popcnt_u64(x.high) +  _mm_popcnt_u64(x.low));

        #else

        return static_cast<int>(__popcnt64(x.high) + __popcnt64(x.low));

        #endif
    }
}

#elif defined(_M_IX86) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION) && !BOOST_INT128_HAS_BUILTIN(__builtin_popcountll)

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr int popcount(const uint128_t x) noexcept
{
    if (BOOST_INT128_IS_CONSTANT_EVALUATED(x))
    {
        return impl::popcount_impl(x.high) + impl::popcount_impl(x.low); // LCOV_EXCL_LINE
    }
    else
    {
        #ifdef __AVX__

        return static_cast<int>(
               _mm_popcnt_u32(static_cast<unsigned>(x.high)) +
               _mm_popcnt_u32(static_cast<unsigned>(x.high >> 32U)) +
               _mm_popcnt_u32(static_cast<unsigned>(x.low)) +
               _mm_popcnt_u32(static_cast<unsigned>(x.low >> 32U)));

        #else

        return static_cast<int>(
               __popcnt(static_cast<unsigned>(x.high)) +
               __popcnt(static_cast<unsigned>(x.high >> 32U)) +
               __popcnt(static_cast<unsigned>(x.low)) +
               __popcnt(static_cast<unsigned>(x.low >> 32U)));

        #endif
    }
}

#elif !BOOST_INT128_HAS_BUILTIN(__builtin_popcountll) || (defined(__CUDACC__) && defined(BOOST_INT128_ENABLE_CUDA))

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr int popcount(const uint128_t x) noexcept
{
    return impl::popcount_impl(x.high) + impl::popcount_impl(x.low);
}

#endif

#if BOOST_INT128_HAS_BUILTIN(__builtin_bswap64) && !(defined(__CUDACC__) && defined(BOOST_INT128_ENABLE_CUDA))

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr uint128_t byteswap(const uint128_t x) noexcept
{
    return {__builtin_bswap64(x.low), __builtin_bswap64(x.high)};
}

#endif

namespace impl {

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr std::uint64_t byteswap_impl(const std::uint64_t x) noexcept
{
    const auto step32 {x << 32U | x >> 32U};
    const auto step16 {(step32 & UINT64_C(0x0000FFFF0000FFFF)) << 16U | (step32 & UINT64_C(0xFFFF0000FFFF0000)) >> 16U};
    return (step16 & UINT64_C(0x00FF00FF00FF00FF)) << 8U | (step16 & UINT64_C(0xFF00FF00FF00FF00)) >> 8U;
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr uint128_t byteswap_impl(const uint128_t x) noexcept
{
    return {byteswap_impl(x.low), byteswap_impl(x.high)};
}

} // namespace impl

#if defined(_MSC_VER) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION) && !BOOST_INT128_HAS_BUILTIN(__builtin_bswap64)

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr uint128_t byteswap(const uint128_t x) noexcept
{
    if (BOOST_INT128_IS_CONSTANT_EVALUATED(x))
    {
        return impl::byteswap_impl(x); // LCOV_EXCL_LINE
    }
    else
    {
        return {_byteswap_uint64(x.low), _byteswap_uint64(x.high)};
    }
}

#elif !BOOST_INT128_HAS_BUILTIN(__builtin_bswap64) || (defined(__CUDACC__) && defined(BOOST_INT128_ENABLE_CUDA))

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr uint128_t byteswap(const uint128_t x) noexcept
{
    return impl::byteswap_impl(x);
}

#endif

} // namespace int128
} // namespace boost

#endif // BOOST_INT128_BIT_HPP
// ===== END boost/int128/bit.hpp =====
// ===== BEGIN boost/int128/iostream.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_IOSTREAM_HPP
#define BOOST_INT128_IOSTREAM_HPP

// [amalgamate] skipped duplicate include of boost/int128/int128.hpp
// ===== BEGIN boost/int128/detail/mini_from_chars.hpp =====
// Copyright 2022 Peter Dimov
// Copyright 2023 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef MINI_FROM_CHARS_HPP
#define MINI_FROM_CHARS_HPP

// [amalgamate] skipped duplicate include of boost/int128/detail/uint128_imp.hpp
// [amalgamate] skipped duplicate include of boost/int128/detail/int128_imp.hpp

#ifndef BOOST_INT128_BUILD_MODULE

#include <cerrno>
#include <limits>
#include <cstddef>

#if !(defined(BOOST_INT128_HAS_GPU_SUPPORT) || defined(BOOST_INT128_DISABLE_EXCEPTIONS))
#include <stdexcept>
#endif

#endif

namespace boost {
namespace int128 {
namespace detail {

namespace impl {

#if !defined(BOOST_INT128_HAS_GPU_SUPPORT)

BOOST_INT128_INLINE_CONSTEXPR unsigned char uchar_values[] =
     {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 254, 255, 255, 255, 255, 255, 255, 255, 255,
        0,   1,   2,   3,   4,   5,   6,   7,   8,   9, 255, 255, 255, 255, 255, 255,
      255,  10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
       25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35, 255, 255, 255, 255, 255,
      255,  10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
       25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};

static_assert(sizeof(uchar_values) == 256, "uchar_values should represent all 256 values of unsigned char");

#endif // __NVCC__

// Convert characters for 0-9, A-Z, a-z to 0-35. The digit separator ' is 254. Anything else is 255
BOOST_INT128_HOST_DEVICE BOOST_INT128_FORCE_INLINE constexpr auto digit_from_char(char val) noexcept -> unsigned char
{
    #if defined(BOOST_INT128_HAS_GPU_SUPPORT)

    constexpr unsigned char uchar_values[] =
    {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 254, 255, 255, 255, 255, 255, 255, 255, 255,
       0,   1,   2,   3,   4,   5,   6,   7,   8,   9, 255, 255, 255, 255, 255, 255,
     255,  10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
      25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35, 255, 255, 255, 255, 255,
     255,  10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
      25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};

    static_assert(sizeof(uchar_values) == 256, "uchar_values should represent all 256 values of unsigned char");

    #endif // __NVCC__

    return uchar_values[static_cast<unsigned char>(val)];
}

template <typename Integer, typename Unsigned_Integer, bool is_literal_parse = false>
BOOST_INT128_HOST_DEVICE constexpr int from_chars_integer_impl(const char* first, const char* last, Integer& value, int base) noexcept
{
    if (last - first <= 0)
    {
        return EINVAL;
    }

    Unsigned_Integer result {};
    Unsigned_Integer overflow_value {};
    Unsigned_Integer max_digit {};

    const auto unsigned_base = static_cast<Unsigned_Integer>(base);

    // Strip sign if the type is signed
    // Negative sign will be appended at the end of parsing
    bool is_negative = false;
    static_cast<void>(is_negative);
    auto next = first;

    BOOST_INT128_IF_CONSTEXPR (std::numeric_limits<Integer>::is_signed)
    {
        if (*next == '-')
        {
            is_negative = true;
            ++next;
        }

        overflow_value = static_cast<Unsigned_Integer>((std::numeric_limits<Integer>::max)());
        max_digit = static_cast<Unsigned_Integer>((std::numeric_limits<Integer>::max)());

        if (is_negative)
        {
            ++overflow_value;
            ++max_digit;
        }
    }
    else
    {
        if (*next == '-' || *next == '+')
        {
            return EINVAL;
        }

        overflow_value = (std::numeric_limits<Unsigned_Integer>::max)();
        max_digit = (std::numeric_limits<Unsigned_Integer>::max)();
    }


    overflow_value /= unsigned_base;
    max_digit %= unsigned_base;

    // If the only character was a sign abort now
    if (next == last)
    {
        return EINVAL;
    }

    bool overflowed = false;

    const std::ptrdiff_t nc = last - next;

    // For bases 2..10 the first digits10 characters always fit in the unsigned
    // For bases above 10, the safe window is shorter, so we must check with each iteration
    const std::ptrdiff_t nd {
        base <= 10
            ? static_cast<std::ptrdiff_t>(std::numeric_limits<Integer>::digits10)
            : std::ptrdiff_t{0}
    };

    const std::ptrdiff_t fast_limit {nd < nc ? nd : nc};
    std::ptrdiff_t i = 0;

    for (; i < fast_limit; ++i)
    {
        const auto raw_digit = digit_from_char(*next);

        // When parsing a user-defined literal skip the digit separator ' (marked as 254)
        BOOST_INT128_IF_CONSTEXPR (is_literal_parse)
        {
            if (raw_digit == 254)
            {
                ++next;
                continue;
            }
        }

        const auto current_digit = static_cast<Unsigned_Integer>(raw_digit);

        if (current_digit >= unsigned_base)
        {
            break;
        }

        result = static_cast<Unsigned_Integer>(result * unsigned_base + current_digit);
        ++next;
    }

    for (; i < nc; ++i)
    {
        const auto raw_digit = digit_from_char(*next);

        // When parsing a user-defined literal skip the digit separator ' (marked as 254)
        BOOST_INT128_IF_CONSTEXPR (is_literal_parse)
        {
            if (raw_digit == 254)
            {
                ++next;
                continue;
            }
        }

        const auto current_digit = static_cast<Unsigned_Integer>(raw_digit);

        if (current_digit >= unsigned_base)
        {
            break;
        }

        if (result < overflow_value || (result == overflow_value && current_digit <= max_digit))
        {
            result = static_cast<Unsigned_Integer>(result * unsigned_base + current_digit);
        }
        else
        {
            overflowed = true;
            break;
        }

        ++next;
    }

    // Return the parsed value, adding the sign back if applicable
    // If we have overflowed, then we do not return the result
    if (overflowed)
    {
        return EDOM;
    }

    value = static_cast<Integer>(result);

    BOOST_INT128_IF_CONSTEXPR (std::numeric_limits<Integer>::is_signed)
    {
        if (is_negative)
        {
            value = static_cast<Integer>(-(static_cast<Unsigned_Integer>(value)));
        }
    }

    // This value will be negative to differentiate from errno values
    // since they are in the range of acceptable distances

    // This cast is useless on 32-bit platforms
    #if defined(__GNUC__) && !defined(__clang__)
    #  pragma GCC diagnostic push
    #  pragma GCC diagnostic ignored "-Wuseless-cast"
    #endif

    return static_cast<int>(first - next);

    #if defined(__GNUC__) && !defined(__clang__)
    #  pragma GCC diagnostic pop
    #endif
}
} // namespace impl

BOOST_INT128_TEST_EXPORT BOOST_INT128_HOST_DEVICE constexpr int from_chars(const char* first, const char* last, uint128_t& value, int base = 10) noexcept
{
    return impl::from_chars_integer_impl<uint128_t, uint128_t>(first, last, value, base);
}

BOOST_INT128_TEST_EXPORT BOOST_INT128_HOST_DEVICE constexpr int from_chars(const char* first, const char* last, int128_t& value, int base = 10) noexcept
{
    return impl::from_chars_integer_impl<int128_t, uint128_t>(first, last, value, base);
}

// Parsing entry points for the user-defined literals. Unlike from_chars these skip the
// C++ digit separator ' so that literals such as 1'234'567_u128 are accepted.
BOOST_INT128_TEST_EXPORT BOOST_INT128_HOST_DEVICE constexpr int from_chars_literal(const char* first, const char* last, uint128_t& value, int base = 10) noexcept
{
    return impl::from_chars_integer_impl<uint128_t, uint128_t, true>(first, last, value, base);
}

BOOST_INT128_TEST_EXPORT BOOST_INT128_HOST_DEVICE constexpr int from_chars_literal(const char* first, const char* last, int128_t& value, int base = 10) noexcept
{
    return impl::from_chars_integer_impl<int128_t, uint128_t, true>(first, last, value, base);
}

// Rejects an out of range literal
[[noreturn]] BOOST_INT128_HOST_DEVICE inline void parse_literal_out_of_range()
{
    #if defined(BOOST_INT128_HAS_GPU_SUPPORT) || defined(BOOST_INT128_DISABLE_EXCEPTIONS)
    BOOST_INT128_UNREACHABLE;
    #else
    BOOST_INT128_THROW_EXCEPTION(std::out_of_range("Literal is out of range of the target type"));
    #endif
}

// Rejects an invalid literal
[[noreturn]] BOOST_INT128_HOST_DEVICE inline void parse_invalid_literal()
{
    #if defined(BOOST_INT128_HAS_GPU_SUPPORT) || defined(BOOST_INT128_DISABLE_EXCEPTIONS)
    BOOST_INT128_UNREACHABLE;
    #else
    BOOST_INT128_THROW_EXCEPTION(std::invalid_argument("Literal is not a valid integer"));
    #endif
}

// GCC before 6 rejects a constexpr function that contains a throw-expression or a
// call to a non-constexpr function anywhere in its body so we need to use unreachable in that case
#if defined(__GNUC__) && !defined(__clang__) && __GNUC__ < 6
#  define BOOST_INT128_REJECT_LITERAL(reporter) BOOST_INT128_UNREACHABLE
#else
#  define BOOST_INT128_REJECT_LITERAL(reporter) reporter()
#endif

// Parse a user-defined literal, hard-failing on any malformed or out-of-range input.
// A C++ base prefix (0x/0X hex, 0b/0B binary, or a leading 0 for octal) is stripped and
// the digits parsed in that base, otherwise handled as base 10
template <typename Integer>
BOOST_INT128_HOST_DEVICE constexpr Integer parse_literal(const char* first, const char* last) noexcept
{
    Integer value {};

    // A leading sign stays with the digits; a base prefix, if present, follows it.
    auto next = first;
    const bool negative {next != last && *next == '-'};
    if (negative)
    {
        ++next;
    }

    int base {10};
    bool prefixed {false};

    if (last - next >= 2 && *next == '0')
    {
        const char marker {next[1]};
        if (marker == 'x' || marker == 'X')
        {
            base = 16;
            next += 2;
            prefixed = true;
        }
        else if (marker == 'b' || marker == 'B')
        {
            base = 2;
            next += 2;
            prefixed = true;
        }
        else
        {
            base = 8;
            next += 1;
            prefixed = true;
        }
    }

    // With no prefix, from_chars_literal handles the sign and the full decimal range.
    // Overflow is reported as EDOM; anything else short of full consumption is malformed.
    if (!prefixed)
    {
        const auto status = from_chars_literal(first, last, value);
        if (status == EDOM)
        {
            BOOST_INT128_REJECT_LITERAL(parse_literal_out_of_range);
        }
        else if (status != first - last)
        {
            BOOST_INT128_REJECT_LITERAL(parse_invalid_literal);
        }

        return value;
    }

    // Prefixed: parse the magnitude in the detected base, then reapply the sign.
    const auto status = from_chars_literal(next, last, value, base);
    if (status == EDOM)
    {
        BOOST_INT128_REJECT_LITERAL(parse_literal_out_of_range);
    }
    else if (status != next - last)
    {
        BOOST_INT128_REJECT_LITERAL(parse_invalid_literal);
    }

    if (negative)
    {
        BOOST_INT128_IF_CONSTEXPR (std::numeric_limits<Integer>::is_signed)
        {
            value = static_cast<Integer>(-value);
        }
        else
        {
            BOOST_INT128_UNREACHABLE;
        }
    }

    return value;
}

} // namespace detail
} // namespace int128
} // namespace boost

#undef BOOST_INT128_REJECT_LITERAL

#endif //MINI_FROM_CHARS_HPP
// ===== END boost/int128/detail/mini_from_chars.hpp =====
// ===== BEGIN boost/int128/detail/mini_to_chars.hpp =====
// Copyright 2022 Peter Dimov
// Copyright 2023 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_DETAIL_MINI_TO_CHARS_HPP
#define BOOST_INT128_DETAIL_MINI_TO_CHARS_HPP

// [amalgamate] skipped duplicate include of boost/int128/int128.hpp

namespace boost {
namespace int128 {
namespace detail {

// A 128-bit integer needs up to 128 binary digits (base 2); allow for a leading sign and a
// null terminator so mini_to_chars is safe for every supported base (2, 8, 10, 16).
constexpr std::size_t mini_to_chars_buffer_size = 130;

#if !defined(BOOST_INT128_HAS_GPU_SUPPORT)

BOOST_INT128_INLINE_CONSTEXPR char lower_case_digit_table[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'a', 'b', 'c', 'd', 'e', 'f'
};

static_assert(sizeof(lower_case_digit_table) == sizeof(char) * 16, "10 numbers, and 6 letters");

BOOST_INT128_INLINE_CONSTEXPR char upper_case_digit_table[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'A', 'B', 'C', 'D', 'E', 'F'
};

static_assert(sizeof(upper_case_digit_table) == sizeof(char) * 16, "10 numbers, and 6 letters");

#endif // !__NVCC__

BOOST_INT128_HOST_DEVICE constexpr char* mini_to_chars(char (&buffer)[mini_to_chars_buffer_size], uint128_t v, const int base, const bool uppercase) noexcept
{
    #if defined(BOOST_INT128_HAS_GPU_SUPPORT)
    constexpr char lower_case_digit_table[] = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        'a', 'b', 'c', 'd', 'e', 'f'
    };

    constexpr char upper_case_digit_table[] = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        'A', 'B', 'C', 'D', 'E', 'F'
    };
    #endif

    char* last {buffer + sizeof(buffer)};
    *--last = '\0';

    if (v == 0U)
    {
        *--last = '0';
        return last;
    }

    const auto digit_table {uppercase ? upper_case_digit_table : lower_case_digit_table};

    switch (base)
    {
        case 2:
            while (v != 0U)
            {
                *--last = v.low & 1U ? '1' : '0';
                v >>= 1U;
            }
            break;

        case 8:
            while (v != 0U)
            {
                constexpr unsigned zero {48U};
                *--last = static_cast<char>(zero + (v & 7U));
                v >>= 3U;
            }
            break;

        case 10:
            while (v != 0U)
            {
                *--last = digit_table[static_cast<std::size_t>(v % 10U)];
                v /= 10U;
            }
            break;

        case 16:
            while (v != 0U)
            {
                *--last = digit_table[static_cast<std::size_t>(v & 15U)];
                v >>= 4U;
            }
            break;

        default:                        // LCOV_EXCL_LINE
            BOOST_INT128_UNREACHABLE;   // LCOV_EXCL_LINE
    }

    return last;
}

BOOST_INT128_HOST_DEVICE constexpr char* mini_to_chars(char (&buffer)[mini_to_chars_buffer_size], const int128_t v, const int base, const bool uppercase) noexcept
{
    char* p {nullptr};

    if (v < 0)
    {
        // We cant negate the min value inside the signed type, but we know what the result will be
        if (v == (std::numeric_limits<int128_t>::min)())
        {
            p = mini_to_chars(buffer, uint128_t{UINT64_C(0x8000000000000000), 0}, base, uppercase);
        }
        else
        {
            const auto neg_v {-v};
            p = mini_to_chars(buffer, static_cast<uint128_t>(neg_v), base, uppercase);
        }

        *--p = '-';
    }
    else
    {
        p = mini_to_chars(buffer, static_cast<uint128_t>(v), base, uppercase);
    }

    return p;
}

} // namespace detail
} // namespace int128
} // namespace boost

#endif // BOOST_INT128_DETAIL_MINI_TO_CHARS_HPP
// ===== END boost/int128/detail/mini_to_chars.hpp =====
// ===== BEGIN boost/int128/detail/utilities.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_DETAIL_UTILITIES_HPP
#define BOOST_INT128_DETAIL_UTILITIES_HPP

// [amalgamate] skipped duplicate include of boost/int128/detail/config.hpp

#ifndef BOOST_INT128_BUILD_MODULE

#include <cstddef>

#endif

namespace boost {
namespace int128 {
namespace detail {

template <typename T>
BOOST_INT128_HOST_DEVICE constexpr std::size_t strlen(const T* str) noexcept
{
    std::size_t i {};
    while (*str != '\0')
    {
        ++str;
        ++i;
    }

    return i;
}

} // namespace detail
} // namespace int128
} // namespace boost

#endif // BOOST_INT128_DETAIL_UTILITIES_HPP
// ===== END boost/int128/detail/utilities.hpp =====
// [amalgamate] skipped duplicate include of boost/int128/detail/config.hpp

#ifndef BOOST_INT128_BUILD_MODULE

#include <type_traits>
#include <iostream>
#include <iomanip>
#include <cstring>

#endif

namespace boost {
namespace int128 {

namespace detail {

template <typename T>
struct streamable_overload
{
    static constexpr bool value = std::is_same<T, uint128_t>::value || std::is_same<T, int128_t>::value;
};

template <typename T>
BOOST_INT128_INLINE_CONSTEXPR bool is_streamable_overload_v = streamable_overload<T>::value;

} // namespace detail

BOOST_INT128_EXPORT template <typename charT, typename traits, typename LibIntegerType>
auto operator>>(std::basic_istream<charT, traits>& is, LibIntegerType& v)
    -> std::enable_if_t<detail::is_streamable_overload_v<LibIntegerType>, std::basic_istream<charT, traits>&>
{
    charT t_buffer[64] {};
    is >> std::ws >> std::setw(63) >> t_buffer;

    const auto t_buffer_len {std::char_traits<charT>::length(t_buffer)};

    char buffer[64] {};
    auto buffer_start {buffer};

    BOOST_INT128_IF_CONSTEXPR (!std::is_same<charT, char>::value)
    {
        auto first {buffer};
        auto t_first {t_buffer};
        const auto t_buffer_end {t_buffer + detail::strlen(t_buffer)};

        while (t_first != t_buffer_end)
        {
            *first++ = static_cast<char>(*t_first++);
        }
    }
    else
    {
        std::memcpy(buffer, t_buffer, sizeof(t_buffer));
    }

    const auto flags {is.flags()};
    int base {10};
    if (flags & std::ios_base::oct)
    {
        base = 8;
        if (*buffer_start == '0')
        {
            ++buffer_start;
        }
    }
    else if (flags & std::ios_base::hex)
    {
        base = 16;
        if (*buffer_start == '0')
        {
            buffer_start += 2;
        }
    }

    const auto r {detail::from_chars(buffer_start, buffer + detail::strlen(buffer), v, base)};

    // Put back unconsumed characters
    // If r is greater than 0 then an errno values has been hit
    const auto consumed {static_cast<std::size_t>(r > 0 ? 0 : -r)};
    BOOST_INT128_ASSERT(t_buffer_len >= consumed);
    const auto return_chars {static_cast<std::size_t>(t_buffer_len - consumed)};

    for (std::size_t i {}; i < return_chars; ++i)
    {
        is.putback(t_buffer[t_buffer_len - i - 1]);
    }

    return is;
}

BOOST_INT128_EXPORT template <typename charT, typename traits, typename LibIntegerType>
auto operator<<(std::basic_ostream<charT, traits>& os, const LibIntegerType& v)
    -> std::enable_if_t<detail::is_streamable_overload_v<LibIntegerType>, std::basic_ostream<charT, traits>&>
{
    char buffer[detail::mini_to_chars_buffer_size] {};

    const auto flags {os.flags()};
    int base {10};
    bool uppercase {false};
    if (flags & std::ios_base::oct)
    {
        base = 8;
    }
    else if (flags & std::ios_base::hex)
    {
        base = 16;
    }

    if (flags & std::ios_base::uppercase)
    {
        uppercase = true;
    }

    auto first {detail::mini_to_chars(buffer, v, base, uppercase)};

    if (flags & std::ios_base::showbase)
    {
        if (base == 8)
        {
            *--first = '0';
        }
        else if (base == 16)
        {
            *--first = uppercase ? 'X' : 'x';
            *--first = '0';
        }
    }

    BOOST_INT128_IF_CONSTEXPR (!std::is_same<charT, char>::value)
    {
        charT t_buffer[64U] {};

        auto t_first {t_buffer};
        while (*first != '\0')
        {
            *t_first++ = static_cast<charT>(*first++);
        }

        os << t_buffer;
    }
    else
    {
        os << first;
    }

    return os;
}

} // namespace int128
} // namespace boost

#endif // BOOST_INT128_IOSTREAM_HPP
// ===== END boost/int128/iostream.hpp =====
// ===== BEGIN boost/int128/literals.hpp =====
// Copyright 2022 Peter Dimov
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_LITERALS_HPP
#define BOOST_INT128_LITERALS_HPP

// [amalgamate] skipped duplicate include of boost/int128/int128.hpp
// [amalgamate] skipped duplicate include of boost/int128/detail/mini_from_chars.hpp
// [amalgamate] skipped duplicate include of boost/int128/detail/utilities.hpp
// [amalgamate] skipped duplicate include of boost/int128/detail/config.hpp

namespace boost {
namespace int128 {
namespace literals {

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr uint128_t operator ""_u128(const char* str) noexcept
{
    return detail::parse_literal<uint128_t>(str, str + detail::strlen(str));
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr uint128_t operator ""_U128(const char* str) noexcept
{
    return detail::parse_literal<uint128_t>(str, str + detail::strlen(str));
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr uint128_t operator ""_u128(const char* str, std::size_t len) noexcept
{
    return detail::parse_literal<uint128_t>(str, str + len);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr uint128_t operator ""_U128(const char* str, std::size_t len) noexcept
{
    return detail::parse_literal<uint128_t>(str, str + len);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr int128_t operator ""_i128(const char* str) noexcept
{
    return detail::parse_literal<int128_t>(str, str + detail::strlen(str));
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr int128_t operator ""_I128(const char* str) noexcept
{
    return detail::parse_literal<int128_t>(str, str + detail::strlen(str));
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr int128_t operator ""_i128(const char* str, std::size_t len) noexcept
{
    return detail::parse_literal<int128_t>(str, str + len);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr int128_t operator ""_I128(const char* str, std::size_t len) noexcept
{
    return detail::parse_literal<int128_t>(str, str + len);
}

} // namespace literals
} // namespace int128
} // namespace boost

// ===== BEGIN boost/int128/detail/literal_macros.hpp =====
// Copyright 2022 Peter Dimov
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_DETAIL_LITERAL_MACROS_HPP
#define BOOST_INT128_DETAIL_LITERAL_MACROS_HPP

// Convenience macros for the user-defined literals. This header intentionally has
// no includes and declares nothing, so module consumers can pull it in on its own
// to obtain the macros (macros are never part of a module's exported interface).

#define BOOST_INT128_STRINGIFY(x) #x
#define BOOST_INT128_UINT128_C(x) boost::int128::literals::operator""_u128(BOOST_INT128_STRINGIFY(x))
#define BOOST_INT128_INT128_C(x) boost::int128::literals::operator""_i128(BOOST_INT128_STRINGIFY(x))

#endif // BOOST_INT128_DETAIL_LITERAL_MACROS_HPP
// ===== END boost/int128/detail/literal_macros.hpp =====

#endif // BOOST_INT128_LITERALS_HPP
// ===== END boost/int128/literals.hpp =====
// ===== BEGIN boost/int128/numeric.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_NUMERIC_HPP
#define BOOST_INT128_NUMERIC_HPP

// [amalgamate] skipped duplicate include of boost/int128/bit.hpp
// [amalgamate] skipped duplicate include of boost/int128/detail/traits.hpp

#ifndef BOOST_INT128_BUILD_MODULE

#include <limits>
#include <utility>

#endif

namespace boost {
namespace int128 {

namespace detail {

template <typename IntegerType>
struct reduced_integers
{
    static constexpr bool value {std::is_same<IntegerType, signed char>::value ||
                                 std::is_same<IntegerType, unsigned char>::value ||
                                 std::is_same<IntegerType, signed short>::value ||
                                 std::is_same<IntegerType, unsigned short>::value ||
                                 std::is_same<IntegerType, signed int>::value ||
                                 std::is_same<IntegerType, unsigned int>::value ||
                                 std::is_same<IntegerType, signed long>::value ||
                                 std::is_same<IntegerType, unsigned long>::value ||
                                 std::is_same<IntegerType, signed long long>::value ||
                                 std::is_same<IntegerType, unsigned long long>::value ||
                                 std::is_same<IntegerType, int128_t>::value ||
                                 std::is_same<IntegerType, uint128_t>::value};
};

#if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)

template <typename IntegerType>
BOOST_INT128_INLINE_CONSTEXPR bool is_reduced_integer_v {reduced_integers<IntegerType>::value ||
                                              std::is_same<IntegerType, detail::builtin_i128>::value ||
                                              std::is_same<IntegerType, detail::builtin_u128>::value};

#else

template <typename IntegerType>
BOOST_INT128_INLINE_CONSTEXPR bool is_reduced_integer_v {reduced_integers<IntegerType>::value};

#endif // 128-bit

} // namespace detail

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr uint128_t add_sat(const uint128_t x, const uint128_t y) noexcept
{
    const auto z {x + y};

    if (z < x)
    {
        return (std::numeric_limits<uint128_t>::max)();
    }

    return z;
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr uint128_t sub_sat(const uint128_t x, const uint128_t y) noexcept
{
    const auto z {x - y};

    if (z > x)
    {
        return (std::numeric_limits<uint128_t>::min)();
    }

    return z;
}

#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable : 4307) // Addition Overflow
#  pragma warning(disable : 4146) // Unary minus applied to unsigned type
#endif

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr int128_t add_sat(const int128_t x, const int128_t y) noexcept
{
    // Detect overflow BEFORE the addition to avoid signed overflow UB.
    // When both are non-negative: overflow iff x > max - y (subtraction safe: max - non_negative >= 0)
    // When both are negative: overflow iff x < min - y (subtraction safe: min - negative > min)
    // Mixed signs: overflow is impossible.

    if (x.high >= 0 && y.high >= 0)
    {
        if (x > (std::numeric_limits<int128_t>::max)() - y)
        {
            return (std::numeric_limits<int128_t>::max)();
        }
    }
    else if (x.high < 0 && y.high < 0)
    {
        if (x < (std::numeric_limits<int128_t>::min)() - y)
        {
            return (std::numeric_limits<int128_t>::min)();
        }
    }

    return x + y;
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr int128_t sub_sat(const int128_t x, const int128_t y) noexcept
{
    // Detect overflow BEFORE the subtraction to avoid signed overflow UB.
    // Positive overflow: x >= 0 and y < 0 and x > max + y (safe: max + negative < max)
    // Negative overflow: x < 0 and y >= 0 and x < min + y (safe: min + non_negative > min)
    // Same signs: overflow is impossible.

    if (x.high >= 0 && y.high < 0)
    {
        if (x > (std::numeric_limits<int128_t>::max)() + y)
        {
            return (std::numeric_limits<int128_t>::max)();
        }
    }
    else if (x.high < 0 && y.high >= 0)
    {
        if (x < (std::numeric_limits<int128_t>::min)() + y)
        {
            return (std::numeric_limits<int128_t>::min)();
        }
    }

    return x - y;
}

#ifdef _MSC_VER
#  pragma warning(pop)
#endif

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr uint128_t mul_sat(const uint128_t x, const uint128_t y) noexcept
{
    const auto x_bits {bit_width(x)};
    const auto y_bits {bit_width(y)};

    if ((x_bits + y_bits) > std::numeric_limits<uint128_t>::digits)
    {
        return (std::numeric_limits<uint128_t>::max)();
    }

    return x * y;
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr int128_t mul_sat(const int128_t x, const int128_t y) noexcept
{
    const auto x_bits {bit_width(static_cast<uint128_t>(abs(x)))};
    const auto y_bits {bit_width(static_cast<uint128_t>(abs(y)))};

    if ((x_bits + y_bits) > std::numeric_limits<int128_t>::digits)
    {
        if ((x < 0) != (y < 0))
        {
            return (std::numeric_limits<int128_t>::min)();
        }
        else
        {
            return (std::numeric_limits<int128_t>::max)();
        }
    }

    const int128_t res {x * y};
    return res;
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr uint128_t div_sat(const uint128_t x, const uint128_t y) noexcept
{
    return x / y;
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr int128_t div_sat(const int128_t x, const int128_t y) noexcept
{
    if (BOOST_INT128_UNLIKELY(x == (std::numeric_limits<int128_t>::min)() && y == -1))
    {
        // This is the only possible case of overflow
        return (std::numeric_limits<int128_t>::max)();
    }

    return x / y;
}

#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable: 4267)
#endif

BOOST_INT128_EXPORT template <typename TargetType, std::enable_if_t<detail::is_reduced_integer_v<TargetType>, bool> = true>
BOOST_INT128_HOST_DEVICE constexpr TargetType saturate_cast(const uint128_t value) noexcept
{
    BOOST_INT128_IF_CONSTEXPR (std::is_same<uint128_t, TargetType>::value)
    {
        return static_cast<TargetType>(value);
    }
    else
    {
        if (value > static_cast<uint128_t>((std::numeric_limits<TargetType>::max)()))
        {
            return (std::numeric_limits<TargetType>::max)();
        }

        return static_cast<TargetType>(value);
    }
}

#ifdef _MSC_VER
#  pragma warning(pop)
#endif

BOOST_INT128_EXPORT template <typename TargetType, std::enable_if_t<detail::is_reduced_integer_v<TargetType>, bool> = true>
BOOST_INT128_HOST_DEVICE constexpr TargetType saturate_cast(const int128_t value) noexcept
{
    BOOST_INT128_IF_CONSTEXPR (std::is_same<int128_t, TargetType>::value)
    {
        return static_cast<TargetType>(value);
    }
    #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)
    else BOOST_INT128_IF_CONSTEXPR (std::is_same<uint128_t, TargetType>::value || std::is_same<detail::builtin_u128, TargetType>::value)
    #else
    else BOOST_INT128_IF_CONSTEXPR (std::is_same<uint128_t, TargetType>::value)
    #endif
    {
        // We can't possibly have overflow in this case
        return value < 0 ? static_cast<TargetType>(0) : static_cast<TargetType>(value);
    }
    else
    {
        if (value > static_cast<int128_t>((std::numeric_limits<TargetType>::max)()))
        {
            return (std::numeric_limits<TargetType>::max)();
        }
        else if (value < static_cast<int128_t>((std::numeric_limits<TargetType>::min)()))
        {
            return (std::numeric_limits<TargetType>::min)();
        }

        return static_cast<TargetType>(value);
    }
}

namespace detail {

BOOST_INT128_TEST_EXPORT BOOST_INT128_HOST_DEVICE constexpr std::uint64_t gcd64(std::uint64_t x, std::uint64_t y) noexcept
{
    if (x == 0)
    {
        return y;
    }
    if (y == 0)
    {
        return x;
    }

    const auto s {impl::countr_impl(x | y)};
    x >>= impl::countr_impl(x);

    do
    {
        y >>= impl::countr_impl(y);
        if (x > y)
        {
            const auto temp {x};
            x = y;
            y = temp;
        }

        y -= x;
    } while (y);

    return x << s;
}

} // namespace detail

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr uint128_t gcd(uint128_t a, uint128_t b) noexcept
{
    // Base case
    if (a == 0U)
    {
        return b;
    }
    if (b == 0U)
    {
        return a;
    }

    const auto a_zero {countr_zero(a)};
    const auto b_zero {countr_zero(b)};
    const auto shift {b_zero < a_zero ? b_zero : a_zero};
    a >>= shift;
    b >>= shift;

    do
    {
        b >>= countr_zero(b);

        if (a > b)
        {
            const uint128_t temp {a};
            a = b;
            b = temp;
        }

        b -= a;
    } while (b != 0U && (a.high | b.high) > 0U);

    // Stop doing 128-bit math as soon as we can
    const auto g {detail::gcd64(a.low, b.low)};
    return uint128_t{0, g} << shift;
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr int128_t gcd(const int128_t a, const int128_t b) noexcept
{
    return static_cast<int128_t>(gcd(static_cast<uint128_t>(abs(a)), static_cast<uint128_t>(abs(b))));
}

// For unknown reasons this implementation fails for MSVC x86 only in release mode
// Directly calculating leads to the same failures, so unfortunately we have a viable,
// but very slow impl that we know works.
#if !(defined(_M_IX86) && !defined(_NDEBUG))

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr uint128_t lcm(const uint128_t a, const uint128_t b) noexcept
{
    if (a == 0U || b == 0U)
    {
        return static_cast<uint128_t>(0);
    }

    // Calculate GCD first
    const auto g {gcd(a, b)};

    // Compute LCM avoiding overflow: (a/gcd) * b
    return (a / g) * b;
}

#else

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr uint128_t lcm(uint128_t a, uint128_t b) noexcept
{
    if (a == 0U || b == 0U)
    {
        return uint128_t{0};
    }


    unsigned shift{};
    while ((a & 1U) == 0U && (b & 1U) == 0U) 
    {
        a >>= 1U;
        b >>= 1U;
        shift++;
    }

    // Ensure a >= b
    if (a < b)
    {
        std::swap(a, b);
    }

    uint128_t lcm{a};

    while (lcm % b != 0U) 
    {
        lcm += a;
    }

    return lcm << shift;
}

#endif

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr int128_t lcm(const int128_t a, const int128_t b) noexcept
{
    return static_cast<int128_t>(lcm(static_cast<uint128_t>(abs(a)), static_cast<uint128_t>(abs(b))));
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr uint128_t midpoint(const uint128_t a, const uint128_t b) noexcept
{
    // Bit manipulation formula works for unsigned integers
    auto mid {(a & b) + ((a ^ b) >> 1)};

    // std::midpoint rounds towards the first parameter
    if ((a ^ b) & 1U && a > b)
    {
        ++mid;
    }

    return mid;
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr int128_t midpoint(const int128_t a, const int128_t b) noexcept
{
    // For signed integers, we use a + (b - a) / 2 or a - (a - b) / 2
    // The subtraction is done in unsigned arithmetic to handle overflow correctly
    // Integer division automatically rounds toward the first argument
    //
    // Use direct field access for both the uint128 construction and the
    // comparison to avoid NVCC host compiler issues with operator<= and
    // static_cast on int128_t for large-magnitude values

    const uint128_t ua {static_cast<std::uint64_t>(a.high), a.low};
    const uint128_t ub {static_cast<std::uint64_t>(b.high), b.low};

    const bool a_le_b {a.high == b.high ? a.low <= b.low : a.high < b.high};

    if (a_le_b)
    {
        // diff = b - a (computed in unsigned, handles wrap-around correctly)
        const auto diff {ub - ua};
        return a + static_cast<int128_t>(diff / 2U);
    }
    else
    {
        // diff = a - b (computed in unsigned, handles wrap-around correctly)
        const auto diff {ua - ub};
        return a - static_cast<int128_t>(diff / 2U);
    }
}

} // namespace int128
} // namespace boost

#endif // BOOST_INT128_NUMERIC_HPP
// ===== END boost/int128/numeric.hpp =====
// ===== BEGIN boost/int128/format.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_FORMAT_HPP
#define BOOST_INT128_FORMAT_HPP

#if __has_include(<format>) && defined(__cpp_lib_format) && __cpp_lib_format >= 201907L && !defined(BOOST_DECIMAL_DISABLE_CLIB)

// [amalgamate] skipped duplicate include of boost/int128/detail/mini_to_chars.hpp
// [amalgamate] skipped duplicate include of boost/int128/detail/config.hpp
// [amalgamate] skipped duplicate include of boost/int128/int128.hpp

#ifndef BOOST_INT128_BUILD_MODULE
#include <string>
#include <format>
#include <tuple>
#endif

#define BOOST_INT128_HAS_FORMAT

namespace boost::int128::detail {

enum class sign_option
{
    plus,
    negative,
    space
};

enum class alignment
{
    none,
    left,   // <
    right,  // >
    center  // ^
};

template <typename ParseContext>
constexpr auto parse_impl(ParseContext& ctx)
{
    auto it {ctx.begin()};
    int base = 10;
    bool is_upper = false;
    int padding_digits = 0;
    auto sign = sign_option::negative;
    bool prefix = false;
    bool zero_pad = false;
    char fill_char = ' ';
    auto align = alignment::none;

    // Parse fill and alignment: [[fill]align]
    // Alignment characters are: < (left), > (right), ^ (center)
    if (it != ctx.end())
    {
        // Check if we have [fill]align (fill char followed by alignment)
        auto next = it;
        ++next;
        if (next != ctx.end() && (*next == '<' || *next == '>' || *next == '^'))
        {
            fill_char = *it;
            it = next;
            switch (*it)
            {
                case '<':
                    align = alignment::left;
                    break;
                case '>':
                    align = alignment::right;
                    break;
                case '^':
                    align = alignment::center;
                    break;
                default:                        // LCOV_EXCL_LINE
                    BOOST_INT128_UNREACHABLE;   // LCOV_EXCL_LINE
            }
            ++it;
        }
        // Check if we just have align (no fill char)
        else if (*it == '<' || *it == '>' || *it == '^')
        {
            switch (*it)
            {
                case '<':
                    align = alignment::left;
                    break;
                case '>':
                    align = alignment::right;
                    break;
                case '^':
                    align = alignment::center;
                    break;
                default:                        // LCOV_EXCL_LINE
                    BOOST_INT128_UNREACHABLE;   // LCOV_EXCL_LINE
            }
            ++it;
        }
    }

    // Handle sign or space
    if (it != ctx.end())
    {
        switch (*it) {
            case ' ':
                sign = sign_option::space;
                ++it;
                break;
            case '+':
                sign = sign_option::plus;
                ++it;
                break;
            case '-':
                sign = sign_option::negative;
                ++it;
                break;
            default:
                break;
        }
    }

    // Alternate form option
    if (it != ctx.end() && *it == '#')
    {
        prefix = true;
        ++it;
    }

    // Zero-pad flag (std::format places '0' before the width)
    if (it != ctx.end() && *it == '0')
    {
        zero_pad = true;
        ++it;
    }

    // Parse the width
    while (it != ctx.end() && *it >= '0' && *it <= '9')
    {
        padding_digits = padding_digits * 10 + (*it - '0');
        ++it;
    }

    // Integer presentation
    if (it != ctx.end() && *it != '}')
    {
        switch (*it++)
        {
            case 'b':
                base = 2;
                break;
            case 'B':
                base = 2;
                is_upper = true;
                break;

            case 'o':
                base = 8;
                break;

            case 'd':
                base = 10;
                break;

            case 'x':
                base = 16;
                break;
            case 'X':
                base = 16;
                is_upper = true;
                break;
            default:                                                                                // LCOV_EXCL_LINE
                BOOST_INT128_THROW_EXCEPTION(std::format_error("Unsupported format specifier"));    // LCOV_EXCL_LINE
        }
    }

    // Verify we're at the closing brace
    if (it != ctx.end() && *it != '}')
    {
        BOOST_INT128_THROW_EXCEPTION(std::format_error("Expected '}' in format string")); // LCOV_EXCL_LINE
    }

    return std::make_tuple(base, padding_digits, sign, is_upper, prefix, zero_pad, fill_char, align, it);
}

template <typename T>
struct is_library_type_impl
{
    static constexpr bool value {std::is_same_v<T, boost::int128::uint128_t> || std::is_same_v<T, boost::int128::int128_t>};
};

template <typename T>
BOOST_INT128_INLINE_CONSTEXPR bool is_library_type_v = is_library_type_impl<T>::value;

template <typename T>
concept is_library_type = is_library_type_v<T>;

} // namespace boost::int128::detail

namespace std {

template <boost::int128::detail::is_library_type T>
struct formatter<T>
{
    int base;
    int padding_digits;
    boost::int128::detail::sign_option sign;
    bool is_upper;
    bool prefix;
    bool zero_pad;
    char fill_char;
    boost::int128::detail::alignment align;

    constexpr formatter() : base {10},
                            padding_digits {0},
                            sign {boost::int128::detail::sign_option::negative},
                            is_upper {false},
                            prefix {false},
                            zero_pad {false},
                            fill_char {' '},
                            align {boost::int128::detail::alignment::none}
    {}

    constexpr auto parse(format_parse_context& ctx)
    {
        const auto res {boost::int128::detail::parse_impl(ctx)};

        base = std::get<0>(res);
        padding_digits = std::get<1>(res);
        sign = std::get<2>(res);
        is_upper = std::get<3>(res);
        prefix = std::get<4>(res);
        zero_pad = std::get<5>(res);
        fill_char = std::get<6>(res);
        align = std::get<7>(res);

        return std::get<8>(res);
    }

    template <typename FormatContext>
    auto format(T v, FormatContext& ctx) const
    {
        char buffer[boost::int128::detail::mini_to_chars_buffer_size];
        bool isneg {false};
        boost::int128::uint128_t abs_v {};

        if constexpr (std::is_same_v<T, boost::int128::int128_t>)
        {
            if (v < 0)
            {
                isneg = true;
                // Can't negate int128_t::min(), handle specially
                if (v == (std::numeric_limits<T>::min)())
                {
                    abs_v = boost::int128::uint128_t{UINT64_C(0x8000000000000000), 0};
                }
                else
                {
                    abs_v = static_cast<boost::int128::uint128_t>(-v);
                }
            }
            else
            {
                abs_v = static_cast<boost::int128::uint128_t>(v);
            }
        }
        else
        {
            abs_v = static_cast<boost::int128::uint128_t>(v);
        }

        const auto end = boost::int128::detail::mini_to_chars(buffer, abs_v, base, is_upper);
        std::string s(end, buffer + sizeof(buffer));

        // Calculate prefix length that will be added later
        std::size_t prefix_len {0};
        if (prefix)
        {
            switch (base)
            {
                case 2:
                case 16:
                    prefix_len = 2;  // "0b", "0B", "0x", or "0X"
                    break;
                case 8:
                    prefix_len = 1;  // "0"
                    break;
                default:
                    break;
            }
        }

        // Calculate sign length that will be added later
        std::size_t sign_len {0};
        if (sign == boost::int128::detail::sign_option::plus || sign == boost::int128::detail::sign_option::space || isneg)
        {
            sign_len = 1;
        }

        // Zero-padding applies only with the '0' flag and no explicit alignment.
        // Account for prefix and sign in the padding calculation.
        if (zero_pad && align == boost::int128::detail::alignment::none && padding_digits > 0)
        {
            auto target_digit_width {static_cast<std::size_t>(padding_digits)};
            if (target_digit_width > prefix_len + sign_len)
            {
                target_digit_width -= prefix_len + sign_len;
            }
            else
            {
                target_digit_width = 0;
            }

            if (s.size() - 1u < target_digit_width)
            {
                s.insert(s.begin(), target_digit_width - s.size() + 1u, '0');
            }
        }

        if (prefix)
        {
            switch (base)
            {
                case 2:
                    if (is_upper)
                    {
                        s.insert(s.begin(), 'B');
                    }
                    else
                    {
                        s.insert(s.begin(), 'b');
                    }
                    s.insert(s.begin(), '0');
                    break;
                case 8:
                    s.insert(s.begin(), '0');
                    break;
                case 16:
                    if (is_upper)
                    {
                        s.insert(s.begin(), 'X');
                    }
                    else
                    {
                        s.insert(s.begin(), 'x');
                    }
                    s.insert(s.begin(), '0');
                    break;
                default:
                    // Nothing to do
                    break;
            }
        }

        // Insert our sign
        switch (sign)
        {
            case boost::int128::detail::sign_option::plus:
                if (isneg)
                {
                    s.insert(s.begin(), '-');
                }
                else
                {
                    s.insert(s.begin(), '+');
                }
                break;
            case boost::int128::detail::sign_option::space:
                if (!isneg)
                {
                    s.insert(s.begin(), ' ');
                }
                if constexpr (std::is_same_v<T, boost::int128::int128_t>)
                {
                    if (isneg)
                    {
                        s.insert(s.begin(), '-');
                    }
                }
                break;
            case boost::int128::detail::sign_option::negative:
                if constexpr (std::is_same_v<T, boost::int128::int128_t>)
                {
                    if (isneg)
                    {
                        s.insert(s.begin(), '-');
                    }
                }
                break;
            // LCOV_EXCL_START
            default:
                BOOST_INT128_UNREACHABLE;
            // LCOV_EXCL_STOP
        }

        s.erase(0, s.find_first_not_of('\0'));
        s.erase(s.find_last_not_of('\0') + 1);

        // Apply alignment. An explicit alignment uses fill_char; with no explicit
        // alignment and no zero-padding, integer types default to right alignment
        // with the fill character (matching std::format).
        if (s.size() < static_cast<std::size_t>(padding_digits) &&
            (align != boost::int128::detail::alignment::none || !zero_pad))
        {
            auto fill_count = static_cast<std::size_t>(padding_digits) - s.size();
            const auto effective_align = (align == boost::int128::detail::alignment::none)
                                             ? boost::int128::detail::alignment::right
                                             : align;
            switch (effective_align)
            {
                case boost::int128::detail::alignment::left:
                    s.append(fill_count, fill_char);
                    break;
                case boost::int128::detail::alignment::right:
                    s.insert(s.begin(), fill_count, fill_char);
                    break;
                case boost::int128::detail::alignment::center:
                {
                    auto left_fill = fill_count / 2;
                    auto right_fill = fill_count - left_fill;
                    s.insert(s.begin(), left_fill, fill_char);
                    s.append(right_fill, fill_char);
                    break;
                }
                    // LCOV_EXCL_START
                default:
                    break;
                    // LCOV_EXCL_STOP
            }
        }

        return std::format_to(ctx.out(), "{}", s);
    }
};

} // namespace std

#endif

#endif // BOOST_INT128_FORMAT_HPP
// ===== END boost/int128/format.hpp =====
// ===== BEGIN boost/int128/limits.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_LIMITS_HPP
#define BOOST_INT128_LIMITS_HPP

// [amalgamate] skipped duplicate include of boost/int128/detail/int128_imp.hpp
// [amalgamate] skipped duplicate include of boost/int128/detail/uint128_imp.hpp

#endif // BOOST_INT128_LIMITS_HPP
// ===== END boost/int128/limits.hpp =====
// ===== BEGIN boost/int128/climits.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_CLIMITS_HPP
#define BOOST_INT128_CLIMITS_HPP

// [amalgamate] skipped duplicate include of boost/int128/detail/int128_imp.hpp
// [amalgamate] skipped duplicate include of boost/int128/detail/uint128_imp.hpp
#include <climits>

#define BOOST_INT128_UINT128_MAX boost::int128::uint128_t{UINT64_MAX, UINT64_MAX}

#define BOOST_INT128_INT128_MIN  boost::int128::int128_t{INT64_MIN, 0}
#define BOOST_INT128_INT128_MAX  boost::int128::int128_t{INT64_MAX, UINT64_MAX}

#endif // BOOST_INT128_CLIMITS_HPP
// ===== END boost/int128/climits.hpp =====
// ===== BEGIN boost/int128/cstdlib.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_CSTDLIB_HPP
#define BOOST_INT128_CSTDLIB_HPP

// [amalgamate] skipped duplicate include of boost/int128/int128.hpp

namespace boost {
namespace int128 {

BOOST_INT128_EXPORT struct u128div_t
{
    uint128_t quot;
    uint128_t rem;
};

BOOST_INT128_EXPORT struct i128div_t
{
    int128_t quot;
    int128_t rem;
};

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr u128div_t div(const uint128_t x, const uint128_t y) noexcept
{
    if (BOOST_INT128_UNLIKELY(x == 0U || y == 0U))
    {
        return u128div_t{0U, 0U};
    }

    if (x < y)
    {
        return u128div_t{0U, x};
    }
    else if (y.high != 0U)
    {
        u128div_t res {};
        res.quot = detail::knuth_div(x, y, res.rem);
        return res;
    }
    else
    {
        if (x.high == 0U)
        {
            return u128div_t{x.low / y.low, x.low % y.low};
        }
        else
        {
            u128div_t res {};
            detail::one_word_div(x, y.low, res.quot, res.rem);
            return res;
        }
    }
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr i128div_t div(const int128_t x, const int128_t y) noexcept
{
    if (BOOST_INT128_UNLIKELY(x == 0 || y == 0))
    {
        return i128div_t{0, 0};
    }

    const auto abs_lhs {static_cast<uint128_t>(abs(x))};
    const auto abs_rhs {static_cast<uint128_t>(abs(y))};

    if (abs_rhs > abs_lhs)
    {
        return {0, x};
    }

    const auto negative_quot {(x.high < 0) != (y.high < 0)};
    const auto negative_rem {x.high < 0};

    #if defined(BOOST_INT128_HAS_INT128)

    if (abs_rhs.high != 0)
    {
        const auto builtin_x {static_cast<detail::builtin_i128>(x)};
        const auto builtin_y {static_cast<detail::builtin_i128>(y)};
        return i128div_t{static_cast<int128_t>(builtin_x / builtin_y),
                         static_cast<int128_t>(builtin_x % builtin_y)};
    }

    #endif

    const auto unsigned_res {div(abs_lhs, abs_rhs)};

    i128div_t res {static_cast<int128_t>(unsigned_res.quot), static_cast<int128_t>(unsigned_res.rem)};

    res.quot = negative_quot ? -res.quot : res.quot;
    res.rem = negative_rem ? -res.rem : res.rem;

    return res;
}

} // namespace int128
} // namespace boost

#endif // BOOST_INT128_CSTDLIB_HPP
// ===== END boost/int128/cstdlib.hpp =====
// ===== BEGIN boost/int128/string.hpp =====
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_STRING_HPP
#define BOOST_INT128_STRING_HPP

// [amalgamate] skipped duplicate include of boost/int128/int128.hpp
// [amalgamate] skipped duplicate include of boost/int128/detail/mini_to_chars.hpp

#ifndef BOOST_INT128_BUILD_MODULE

#include <string>

#endif

namespace boost {
namespace int128 {

template <typename T>
auto to_string(const T& value) -> std::enable_if_t<(std::is_same<T, int128_t>::value || std::is_same<T, uint128_t>::value), std::string>
{
    char buffer[detail::mini_to_chars_buffer_size];
    const auto last {detail::mini_to_chars(buffer, value, 10, false)};
    return std::string{last, buffer + sizeof(buffer) - 1};
}

} // namespace int128
} // namespace boost

#endif // BOOST_INT128_STRING_HPP
// ===== END boost/int128/string.hpp =====
// ===== BEGIN boost/int128/utilities.hpp =====
// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_UTILITIES_HPP
#define BOOST_INT128_UTILITIES_HPP

// [amalgamate] skipped duplicate include of boost/int128/int128.hpp
// [amalgamate] skipped duplicate include of boost/int128/bit.hpp
// [amalgamate] skipped duplicate include of boost/int128/detail/config.hpp

#ifndef BOOST_INT128_BUILD_MODULE

#include <cstdint>
#include <limits>
#include <type_traits>

#endif

namespace boost {
namespace int128 {

namespace detail {

// Modular addition for 128-bit operands assuming 0 <= a, b < m
BOOST_INT128_HOST_DEVICE constexpr uint128_t addmod(const uint128_t a, const uint128_t b, const uint128_t m) noexcept
{
    const uint128_t s {a + b};

    if (s < a || s >= m)
    {
        return s - m;
    }

    return s;
}

// Modular multiplication via shift-and-add for the full 128-bit modulus case
BOOST_INT128_TEST_EXPORT BOOST_INT128_HOST_DEVICE constexpr uint128_t mulmod_shift(uint128_t a, uint128_t b, const uint128_t m) noexcept
{
    uint128_t result {0};

    while (b != 0U)
    {
        if (static_cast<bool>(b.low & 1U))
        {
            result = addmod(result, a, m);
        }

        a = addmod(a, a, m);
        b >>= 1;
    }

    return result;
}

// Modular multiplication when the modulus fits in 64 bits
BOOST_INT128_HOST_DEVICE constexpr std::uint64_t mulmod_word(const std::uint64_t a, const std::uint64_t b, const std::uint64_t m) noexcept
{
    return ((uint128_t{a} * uint128_t{b}) % uint128_t{m}).low;
}

} // namespace detail

// Computes (base ^ exp) mod m using fast modular exponentiation with
// optimizations specific to the boost::int128 library types
BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr uint128_t powm(uint128_t base, uint128_t exp, const uint128_t m) noexcept
{
    if (BOOST_INT128_UNLIKELY(m == 0U))
    {
        return uint128_t{0};
    }

    if (m == 1U)
    {
        return uint128_t{0};
    }

    if (exp == 0U)
    {
        return uint128_t{1};
    }

    base %= m;

    if (base == 0U)
    {
        return uint128_t{0};
    }

    // Power-of-two modulus: reduction is just a bitmask.
    if (has_single_bit(m))
    {
        const uint128_t mask {m - 1U};
        uint128_t result {1};

        while (exp != 0U)
        {
            if (static_cast<bool>(exp.low & 1U))
            {
                result = (result * base) & mask;
            }

            base = (base * base) & mask;
            exp >>= 1;
        }

        return result;
    }

    // Modulus fits in 64 bits: stay in 64-bit lanes.
    if (m.high == 0U)
    {
        const auto mm {m.low};
        std::uint64_t result {1};
        auto b {base.low};

        while (exp != 0U)
        {
            if (static_cast<bool>(exp.low & 1U))
            {
                result = detail::mulmod_word(result, b, mm);
            }

            b = detail::mulmod_word(b, b, mm);
            exp >>= 1;
        }

        return uint128_t{result};
    }

    // General 128-bit modulus: shift-and-add for each squaring keeps every
    // intermediate strictly below m without needing a 256-bit product.
    uint128_t result {1};

    while (exp != 0U)
    {
        if (static_cast<bool>(exp.low & 1U))
        {
            result = detail::mulmod_shift(result, base, m);
        }

        base = detail::mulmod_shift(base, base, m);
        exp >>= 1;
    }

    return result;
}

// Signed overload. Returns the non-negative residue in [0, m)
BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr int128_t powm(const int128_t base, const int128_t exp, const int128_t m) noexcept
{
    if (BOOST_INT128_UNLIKELY(m <= 0 || exp < 0))
    {
        return int128_t{0};
    }

    const uint128_t um {static_cast<uint128_t>(m)};

    uint128_t ub {};

    if (base.high < 0)
    {
        const uint128_t magnitude {static_cast<uint128_t>(abs(base))};
        const uint128_t r {magnitude % um};
        ub = r == 0U ? uint128_t{0} : static_cast<uint128_t>(um - r);
    }
    else
    {
        ub = static_cast<uint128_t>(base) % um;
    }

    return static_cast<int128_t>(powm(ub, static_cast<uint128_t>(exp), um));
}

// Computes base^exp using exponentiation by squaring. The result is reduced
// modulo 2^128, mirroring the wrap-around behavior of operator*.
BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr uint128_t ipow(uint128_t base, std::uint64_t exp) noexcept
{
    uint128_t result {1};

    while (exp != 0U)
    {
        if (static_cast<bool>(exp & 1U))
        {
            result *= base;
        }

        exp >>= 1;

        if (exp != 0U)
        {
            base *= base;
        }
    }

    return result;
}

// Signed overload. Wraps modulo 2^128 on overflow, matching operator*.
BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr int128_t ipow(int128_t base, std::uint64_t exp) noexcept
{
    int128_t result {1};

    while (exp != 0U)
    {
        if (static_cast<bool>(exp & 1U))
        {
            result *= base;
        }

        exp >>= 1;

        if (exp != 0U)
        {
            base *= base;
        }
    }

    return result;
}

// Integer square root: returns floor(sqrt(n)).
BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr uint128_t isqrt(const uint128_t n) noexcept
{
    if (n < 2U)
    {
        return n;
    }

    // 2^ceil(bit_width(n)/2) is the smallest power of two whose square exceeds n.
    uint128_t x {uint128_t{1} << ((bit_width(n) + 1) / 2)};

    while (true)
    {
        const uint128_t y {(x + n / x) >> 1};

        if (y >= x)
        {
            return x;
        }

        x = y;
    }
}

// Signed overload. Negative inputs are documented to return 0.
BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr int128_t isqrt(const int128_t n) noexcept
{
    if (BOOST_INT128_UNLIKELY(n < 0))
    {
        return int128_t{0};
    }

    return static_cast<int128_t>(isqrt(static_cast<uint128_t>(n)));
}

namespace detail {

// The C23 checked integer macros accept any integer type for their operands
// except bool, plain char, enumerated types, and bit-precise (_BitInt) types.
template <typename T>
struct valid_checked_type : std::integral_constant<bool, std::is_integral<T>::value &&
                                                         !std::is_same<T, bool>::value &&
                                                         !std::is_same<T, char>::value> {};

template <>
struct valid_checked_type<int128_t> : std::true_type {};

template <>
struct valid_checked_type<uint128_t> : std::true_type {};

// Widen an integer operand to its 128-bit two's complement bit pattern, returned as a uint128_t
template <typename T>
BOOST_INT128_HOST_DEVICE constexpr uint128_t ckd_widen(const T value) noexcept
{
    BOOST_INT128_IF_CONSTEXPR (std::numeric_limits<T>::is_signed)
    {
        return static_cast<uint128_t>(static_cast<int128_t>(value));
    }
    else
    {
        return static_cast<uint128_t>(value);
    }
}

// Sign and magnitude of an operand together with its 128-bit two's complement
// image. magnitude is the absolute value; negative records the sign.
struct ckd_operand
{
    uint128_t raw;
    uint128_t magnitude;
    bool negative;
};

template <typename T>
BOOST_INT128_HOST_DEVICE constexpr ckd_operand ckd_decompose(const T value) noexcept
{
    const uint128_t raw {ckd_widen(value)};
    const bool negative {std::numeric_limits<T>::is_signed && ((raw >> 127) != 0U)};
    return ckd_operand{raw, negative ? uint128_t{0} - raw : raw, negative};
}

// Exact signed sum of two operands given as (magnitude, sign). carry marks a
// 129th bit, which no 128-bit or narrower target can represent.
struct ckd_sum_result
{
    uint128_t magnitude;
    bool negative;
    bool carry;
};

BOOST_INT128_HOST_DEVICE constexpr ckd_sum_result ckd_signed_sum(const uint128_t a_magnitude, const bool a_negative,
                                                                 const uint128_t b_magnitude, const bool b_negative) noexcept
{
    if (a_negative == b_negative)
    {
        // Equal signs: magnitudes add and may overflow into a 129th bit.
        const uint128_t magnitude {a_magnitude + b_magnitude};
        return ckd_sum_result{magnitude, a_negative, magnitude < a_magnitude};
    }

    // Opposite signs: the smaller magnitude is subtracted and never carries.
    if (a_magnitude >= b_magnitude)
    {
        return ckd_sum_result{a_magnitude - b_magnitude, a_negative, false};
    }

    return ckd_sum_result{b_magnitude - a_magnitude, b_negative, false};
}

// Whether a result of the given sign and magnitude fits in T1. exceeds_width
// forces overflow when the true magnitude does not even fit in 128 bits.
template <typename T1>
BOOST_INT128_HOST_DEVICE constexpr bool ckd_overflows(const uint128_t magnitude, const bool negative, const bool exceeds_width) noexcept
{
    if (exceeds_width)
    {
        return true;
    }

    const uint128_t max_magnitude {static_cast<uint128_t>((std::numeric_limits<T1>::max)())};

    if (negative)
    {
        const uint128_t min_magnitude {std::numeric_limits<T1>::is_signed ? max_magnitude + uint128_t{1} : uint128_t{0}};
        return magnitude > min_magnitude;
    }

    return magnitude > max_magnitude;
}

} // namespace detail

// Checked addition following the C23 <stdckdint.h> ckd_add contract.
//
// Computes a + b as if both operands were represented in a signed integer
// type of infinite range and then converts that exact result to the type
// pointed to by result. *result always receives the exact result wrapped
// around to the width of *result. Returns false when *result represents the
// exact mathematical sum, and true when the sum did not fit and wrap-around
// occurred.
BOOST_INT128_EXPORT template <typename T1, typename T2, typename T3>
BOOST_INT128_HOST_DEVICE constexpr bool ckd_add(T1* result, const T2 a, const T3 b) noexcept
{
    static_assert(detail::valid_checked_type<T1>::value &&
                  detail::valid_checked_type<T2>::value &&
                  detail::valid_checked_type<T3>::value,
                  "ckd_add operands must be integer types other than bool and plain char.");

    const auto op_a {detail::ckd_decompose(a)};
    const auto op_b {detail::ckd_decompose(b)};

    // The modular sum of the widened images is the exact sum mod 2^128, which
    // is all the wrapped result needs for any target no wider than 128 bits.
    *result = static_cast<T1>(op_a.raw + op_b.raw);

    const auto sum {detail::ckd_signed_sum(op_a.magnitude, op_a.negative, op_b.magnitude, op_b.negative)};
    return detail::ckd_overflows<T1>(sum.magnitude, sum.negative, sum.carry);
}

// Checked subtraction following the C23 <stdckdint.h> ckd_sub contract.
//
// Behaves as ckd_add for a - b: *result receives the exact difference wrapped
// to its width, and the return value reports whether that difference did not
// fit.
BOOST_INT128_EXPORT template <typename T1, typename T2, typename T3>
BOOST_INT128_HOST_DEVICE constexpr bool ckd_sub(T1* result, const T2 a, const T3 b) noexcept
{
    static_assert(detail::valid_checked_type<T1>::value &&
                  detail::valid_checked_type<T2>::value &&
                  detail::valid_checked_type<T3>::value,
                  "ckd_sub operands must be integer types other than bool and plain char.");

    const auto op_a {detail::ckd_decompose(a)};
    const auto op_b {detail::ckd_decompose(b)};

    *result = static_cast<T1>(op_a.raw - op_b.raw);

    // a - b is a + (-b): negating b flips its sign while keeping its magnitude.
    const auto difference {detail::ckd_signed_sum(op_a.magnitude, op_a.negative, op_b.magnitude, !op_b.negative)};
    return detail::ckd_overflows<T1>(difference.magnitude, difference.negative, difference.carry);
}

// Checked multiplication following the C23 <stdckdint.h> ckd_mul contract.
//
// Computes a * b as if both operands had infinite range, stores the result
// wrapped to the width of *result, and returns true when the exact product did
// not fit.
BOOST_INT128_EXPORT template <typename T1, typename T2, typename T3>
BOOST_INT128_HOST_DEVICE constexpr bool ckd_mul(T1* result, const T2 a, const T3 b) noexcept
{
    static_assert(detail::valid_checked_type<T1>::value &&
                  detail::valid_checked_type<T2>::value &&
                  detail::valid_checked_type<T3>::value,
                  "ckd_mul operands must be integer types other than bool and plain char.");

    const auto op_a {detail::ckd_decompose(a)};
    const auto op_b {detail::ckd_decompose(b)};

    *result = static_cast<T1>(op_a.raw * op_b.raw);

    // The product magnitude needs more than 128 bits exactly when it exceeds
    // UINT128_MAX. Dividing the maximum by one magnitude tests that without
    // forming a 256-bit product.
    const bool exceeds_width {op_a.magnitude != 0U &&
                              op_b.magnitude > ((std::numeric_limits<uint128_t>::max)() / op_a.magnitude)};

    const uint128_t product_magnitude {op_a.magnitude * op_b.magnitude};
    const bool product_negative {op_a.negative != op_b.negative};

    return detail::ckd_overflows<T1>(product_magnitude, product_negative, exceeds_width);
}

} // namespace int128
} // namespace boost

#endif // BOOST_INT128_UTILITIES_HPP
// ===== END boost/int128/utilities.hpp =====
// ===== BEGIN boost/int128/hash.hpp =====
// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_HASH_HPP
#define BOOST_INT128_HASH_HPP

// [amalgamate] skipped duplicate include of boost/int128/int128.hpp

#ifndef BOOST_INT128_BUILD_MODULE

#include <cstddef>
#include <cstdint>
#include <functional>

#endif

namespace boost {
namespace int128 {
namespace detail {

// The cast is only useless for 64-bit platforms
// Without we get an implicit conversion warning which is arguably worse
#if defined(__GNUC__) && !defined(__clang__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wuseless-cast"
#endif

// splitmix64 finalizer: mixes all 64 input bits into the result before any narrowing to size_t.
// This is required for correctness on platforms where size_t is 32 bits
inline std::size_t hash_finalize_64(std::uint64_t v) noexcept
{
    v ^= v >> 30;
    v *= UINT64_C(0xbf58476d1ce4e5b9);
    v ^= v >> 27;
    v *= UINT64_C(0x94d049bb133111eb);
    v ^= v >> 31;
    return static_cast<std::size_t>(v);
}

#if defined(__GNUC__) && !defined(__clang__)
#  pragma GCC diagnostic pop
#endif

} // namespace detail
} // namespace int128
} // namespace boost

namespace std {

template <>
struct hash<boost::int128::int128_t>
{
    auto operator()(const boost::int128::int128_t v) const noexcept -> std::size_t
    {
        const std::size_t low_hash {boost::int128::detail::hash_finalize_64(v.low)};
        const std::size_t high_hash {boost::int128::detail::hash_finalize_64(static_cast<std::uint64_t>(v.high))};

        // boost::hash_combine style mixing of the two finalized halves
        return low_hash ^ (high_hash + static_cast<std::size_t>(0x9e3779b9) + (low_hash << 6) + (low_hash >> 2));
    }
};

template <>
struct hash<boost::int128::uint128_t>
{
    auto operator()(const boost::int128::uint128_t v) const noexcept -> std::size_t
    {
        const std::size_t low_hash {boost::int128::detail::hash_finalize_64(v.low)};
        const std::size_t high_hash {boost::int128::detail::hash_finalize_64(v.high)};

        // boost::hash_combine style mixing of the two finalized halves
        return low_hash ^ (high_hash + static_cast<std::size_t>(0x9e3779b9) + (low_hash << 6) + (low_hash >> 2));
    }
};

} // namespace std

namespace boost {
namespace int128 {

inline std::size_t hash_value(const uint128_t v) noexcept
{
    return std::hash<uint128_t>{}(v);
}

inline std::size_t hash_value(const int128_t v) noexcept
{
    return std::hash<int128_t>{}(v);
}

} // namespace int128
} // namespace boost

#endif // BOOST_INT128_HASH_HPP
// ===== END boost/int128/hash.hpp =====

#endif // BOOST_INT128_HPP
// ===== END boost/int128.hpp =====
