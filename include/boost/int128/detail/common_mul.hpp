// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_DETAIL_KNUTH_MUL_HPP
#define BOOST_INT128_DETAIL_KNUTH_MUL_HPP

#include <boost/int128/detail/config.hpp>
#include <cstdint>
#include <cstring>

namespace boost {
namespace int128 {
namespace detail {

// See: The Art of Computer Programming Volume 2 (Semi-numerical algorithms) section 4.3.1
// Algorithm M: Multiplication of Non-negative integers
template <typename ReturnType, std::size_t u_size, std::size_t v_size>
BOOST_INT128_FORCE_INLINE constexpr ReturnType knuth_multiply(const std::uint32_t (&u)[u_size],
                                                              const std::uint32_t (&v)[v_size]) noexcept
{
    using high_word_type = decltype(ReturnType{}.high);

    std::uint32_t w[u_size + v_size] {};

    // M.1
    for (std::size_t j {}; j < v_size; ++j)
    {
        // M.2
        if (v[j] == 0)
        {
            w[j + u_size] = 0;
            continue;
        }

        // M.3
        std::uint64_t t {};
        for (std::size_t i {}; i < u_size; ++i)
        {
            // M.4
            t += static_cast<std::uint64_t>(u[i]) * v[j] + w[i + j];
            w[i + j] = static_cast<std::uint32_t>(t);
            t >>= 32u;
        }

        // M.5
        w[j + u_size] = static_cast<std::uint32_t>(t);
    }

    const auto low {static_cast<std::uint64_t>(w[0]) | (static_cast<std::uint64_t>(w[1]) << 32)};
    const auto high {static_cast<std::uint64_t>(w[2]) | (static_cast<std::uint64_t>(w[3]) << 32)};

    return {static_cast<high_word_type>(high), low};
}

BOOST_INT128_FORCE_INLINE constexpr void to_words(const std::uint64_t x, std::uint32_t (&words)[2]) noexcept
{
    #ifndef BOOST_INT128_NO_CONSTEVAL_DETECTION

    if (!BOOST_INT128_IS_CONSTANT_EVALUATED(x))
    {
        std::memcpy(&words, &x, sizeof(std::uint64_t));
    }

    #endif

    words[0] = static_cast<std::uint32_t>(x & UINT32_MAX);
    words[1] = static_cast<std::uint32_t>(x >> 32);
}

BOOST_INT128_FORCE_INLINE constexpr void to_words(const std::uint32_t x, std::uint32_t (&words)[1]) noexcept
{
    words[0] = x;
}

#if defined(__i386__) && defined(__SSE2__)

template <typename T>
BOOST_INT128_FORCE_INLINE T sse_mul(const T lhs, const T rhs) noexcept
{
    using high_word_type = decltype(T{}.high);

    T result;

    const auto a0 {static_cast<std::uint32_t>(lhs.low & UINT32_MAX)};
    const auto a1 {static_cast<std::uint32_t>(lhs.low >> 32)};
    const auto a2 {static_cast<std::uint32_t>(lhs.high & UINT32_MAX)};
    const auto a3 {static_cast<std::uint32_t>(lhs.high >> 32)};

    const auto b0 {static_cast<std::uint32_t>(rhs.low & UINT32_MAX)};
    const auto b1 {static_cast<std::uint32_t>(rhs.low >> 32)};
    const auto b2 {static_cast<std::uint32_t>(rhs.high & UINT32_MAX)};
    const auto b3 {static_cast<std::uint32_t>(rhs.high >> 32)};

    // Perform multiplications in parallel using SSE2
    __m128i a_vec = _mm_set_epi32(0, a2, a1, a0);
    __m128i b0_vec = _mm_set1_epi32(b0);
    __m128i b1_vec = _mm_set1_epi32(b1);

    // Multiply a_vec by b0 and b1
    __m128i prod0 = _mm_mul_epu32(a_vec, b0_vec);
    __m128i prod1 = _mm_mul_epu32(a_vec, b1_vec);

    // Extract results and handle carries
    std::uint64_t p00 = _mm_cvtsi128_si64(prod0);
    std::uint64_t p10 = _mm_cvtsi128_si64(_mm_srli_si128(prod0, 8));
    std::uint64_t p01 = _mm_cvtsi128_si64(prod1);
    std::uint64_t p11 = _mm_cvtsi128_si64(_mm_srli_si128(prod1, 8));

    // Additional multiplications for higher parts
    const auto p20 {static_cast<std::uint64_t>(a2) * b0};
    const auto p02 {static_cast<std::uint64_t>(a0) * b2};
    const auto p21 {static_cast<std::uint64_t>(a2) * b1};
    const auto p12 {static_cast<std::uint64_t>(a1) * b2};

    const auto high_products {static_cast<high_word_type>(
                           (a3 * b0 + a2 * b1 + a1 * b2 + a0 * b3) +
                           (a3 * b1 + a2 * b2 + a1 * b3) +
                           (a3 * b2 + a2 * b3) +
                           (a3 * b3))};

    // Combine results with carries
    const auto middle {(p00 >> 32) + (p01 & UINT32_MAX) + (p10 & UINT32_MAX)};
    result.low = (middle << 32) | (p00 & UINT32_MAX);
    result.high = static_cast<high_word_type>(
                    high_products + p11 + p20 + p02 + p21 + p12 +
                    (p01 >> 32) + (p10 >> 32) + (middle >> 32)
                  );

    return result;
}

#endif // __SSE2__

} // namespace detail
} // namespace int128
} // namespace boost

#endif // BOOST_INT128_DETAIL_KNUTH_MUL_HPP
