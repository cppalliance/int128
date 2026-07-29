// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_BIT_HPP
#define BOOST_INT128_BIT_HPP

#include <boost/int128/int128.hpp>
#include <boost/int128/detail/config.hpp>
#include <boost/int128/detail/clz.hpp>
#include <boost/int128/detail/ctz.hpp>

namespace boost {
namespace int128 {

namespace impl {

BOOST_INT128_HOST_DEVICE constexpr int countl_zero_impl(const uint128_t x) noexcept
{
    return x.high == 0 ? 64 + detail::countl_zero(x.low) : detail::countl_zero(x.high);
}

} // namespace impl

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr int countl_zero(const uint128_t x) noexcept
{
    #if defined(BOOST_INT128_HAS_INT128) && !(defined(__CUDACC__) && defined(BOOST_INT128_ENABLE_CUDA)) && BOOST_INT128_HAS_BUILTIN(__builtin_clzg) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION)

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(x))
    {
        return impl::countl_zero_impl(x);
    }

    return __builtin_clzg(static_cast<detail::builtin_u128>(x));

    #else

    return impl::countl_zero_impl(x);

    #endif
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
    // __builtin_stdc_bit_ceil not available, but this is equivalent
    return x <= 1U ? static_cast<uint128_t>(1) : static_cast<uint128_t>(2) << (127 - countl_zero(x - 1));
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr uint128_t bit_floor(const uint128_t x) noexcept
{
    // __builtin_stdc_bit_floor not available, but this is equivalent
    return x == 0U ? static_cast<uint128_t>(0) : static_cast<uint128_t>(1) << (127 - countl_zero(x));
}

namespace impl {

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr int countr_zero_impl(const uint128_t x) noexcept
{
    return x.low == 0 ? 64 + detail::countr_zero(x.high) : detail::countr_zero(x.low);
}

} // namespace impl

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr int countr_zero(const uint128_t x) noexcept
{
    #if defined(BOOST_INT128_HAS_INT128) && !(defined(__CUDACC__) && defined(BOOST_INT128_ENABLE_CUDA)) && BOOST_INT128_HAS_BUILTIN(__builtin_clzg) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION)

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(x))
    {
        return impl::countr_zero_impl(x);
    }

    return __builtin_ctzg(static_cast<detail::builtin_u128>(x));

    #else

    return impl::countr_zero_impl(x);

    #endif
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr int countr_one(const uint128_t x) noexcept
{
    return countr_zero(~x);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr uint128_t rotl(const uint128_t x, const int s) noexcept
{
    // __builtin_stdc_rotate_left not available
    constexpr auto mask {127U};
    return x << (static_cast<unsigned>(s) & mask) | x >> (static_cast<unsigned>(-s) & mask);
}

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr uint128_t rotr(const uint128_t x, const int s) noexcept
{
    // __builtin_stdc_rotate_right not available
    constexpr auto mask {127U};
    return x >> (static_cast<unsigned>(s) & mask) | x << (static_cast<unsigned>(-s) & mask);
}

namespace impl {

BOOST_INT128_TEST_EXPORT BOOST_INT128_HOST_DEVICE constexpr int popcount_impl(std::uint64_t x) noexcept
{
    x = x - ((x >> 1U) & UINT64_C(0x5555555555555555));
    x = (x & UINT64_C(0x3333333333333333)) + ((x >> 2U) & UINT64_C(0x3333333333333333));
    x = (x + (x >> 4U)) & UINT64_C(0x0F0F0F0F0F0F0F0F);

    return static_cast<int>((x * UINT64_C(0x0101010101010101)) >> 56U);
}

// The exact-match overload above is selected for the 64-bit halves
BOOST_INT128_TEST_EXPORT BOOST_INT128_HOST_DEVICE constexpr int popcount_impl(const uint128_t x) noexcept
{
    return popcount_impl(x.high) + popcount_impl(x.low);
}

} // namespace impl

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr int popcount(const uint128_t x) noexcept
{
    #if defined(BOOST_INT128_HAS_INT128) && !(defined(__CUDACC__) && defined(BOOST_INT128_ENABLE_CUDA)) && BOOST_INT128_HAS_BUILTIN(__builtin_popcountg) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION)

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(x))
    {
        return impl::popcount_impl(x);
    }

    return __builtin_popcountg(static_cast<detail::builtin_u128>(x));

    #elif BOOST_INT128_HAS_BUILTIN(__builtin_popcountll) && !(defined(__CUDACC__) && defined(BOOST_INT128_ENABLE_CUDA))

    return __builtin_popcountll(x.high) + __builtin_popcountll(x.low);

    #elif defined(_M_AMD64) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION)

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(x))
    {
        return impl::popcount_impl(x); // LCOV_EXCL_LINE
    }
    else
    {
        #ifdef __AVX__

        return static_cast<int>(_mm_popcnt_u64(x.high) +  _mm_popcnt_u64(x.low));

        #else

        return static_cast<int>(__popcnt64(x.high) + __popcnt64(x.low));

        #endif
    }

    #elif defined(_M_IX86) && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION)

    if (BOOST_INT128_IS_CONSTANT_EVALUATED(x))
    {
        return impl::popcount_impl(x); // LCOV_EXCL_LINE
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

    #else

    return impl::popcount_impl(x);

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

BOOST_INT128_EXPORT BOOST_INT128_HOST_DEVICE constexpr bool has_single_bit(const uint128_t x) noexcept
{
    return popcount(x) == 1;
}

} // namespace int128
} // namespace boost

#endif // BOOST_INT128_BIT_HPP
