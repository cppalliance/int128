// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_DETAIL_COMMON_DIV_HPP
#define BOOST_INT128_DETAIL_COMMON_DIV_HPP

#include <boost/int128/detail/config.hpp>
#include <boost/int128/detail/clz.hpp>

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
BOOST_INT128_FORCE_INLINE constexpr void half_word_div(const T& lhs, const std::uint32_t rhs, T& quotient, T& remainder) noexcept
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
BOOST_INT128_FORCE_INLINE constexpr void half_word_div(const T& lhs, const std::uint32_t rhs, T& quotient) noexcept
{
    BOOST_INT128_ASSUME(rhs != 0); // LCOV_EXCL_LINE

    quotient.high = lhs.high / rhs;
    auto remainder {((static_cast<std::uint64_t>(lhs.high) % rhs) << 32) | (lhs.low >> 32)};
    quotient.low = (remainder / rhs) << 32;
    remainder = ((remainder % rhs) << 32) | (lhs.low & UINT32_MAX);
    quotient.low |= (remainder / rhs) & UINT32_MAX;
}

namespace impl {

#if defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 4127) // Pre c++17 the if constexpr remainder part will hit this
#endif

template <std::size_t v_size>
BOOST_INT128_FORCE_INLINE constexpr void unpack_v(std::uint32_t (&vn)[4], const std::uint32_t (&v)[v_size],
    const bool needs_shift, const int s, const int complement_s, const std::integral_constant<std::size_t, 2>&) noexcept
{
    vn[1] = needs_shift ? ((v[1] << s) | (v[0] >> complement_s)) : v[1];
    vn[0] = needs_shift ? (v[0] << s) : v[0];
}

template <std::size_t v_size>
BOOST_INT128_FORCE_INLINE constexpr void unpack_v(std::uint32_t (&vn)[4], const std::uint32_t (&v)[v_size],
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
constexpr void knuth_divide(std::uint32_t (&u)[u_size], const std::size_t m,
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
            q_hat--;
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
BOOST_INT128_FORCE_INLINE constexpr std::size_t to_words(const T& x, std::uint32_t (&words)[4]) noexcept
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

BOOST_INT128_FORCE_INLINE constexpr std::size_t to_words(const std::uint64_t x, std::uint32_t (&words)[2]) noexcept
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

BOOST_INT128_FORCE_INLINE constexpr std::size_t to_words(const std::uint32_t x, std::uint32_t (&words)[1]) noexcept
{
    words[0] = x;

    return 1;
}

template <typename T>
BOOST_INT128_FORCE_INLINE constexpr T from_words(const std::uint32_t (&words)[4]) noexcept
{
    using high_word_type = decltype(T{}.high);

    const auto low {static_cast<std::uint64_t>(words[0]) | (static_cast<std::uint64_t>(words[1]) << 32)};
    const auto high {static_cast<std::uint64_t>(words[2]) | (static_cast<std::uint64_t>(words[3]) << 32)};

    return {static_cast<high_word_type>(high), low};
}

#if defined(_M_AMD64) && !defined(__GNUC__) && !defined(__clang__) && _MSC_VER >= 1920

template <bool needs_mod, typename T>
BOOST_INT128_FORCE_INLINE T div_mod_msvc(T dividend, T divisor, T& remainder)
{
    using high_word_type = decltype(T{}.high);

    // Skip normalization if divisor is already large enough
    // Use direct division and intrinsic
    // This is only possible in the unsigned case
    BOOST_INT128_IF_CONSTEXPR (!std::numeric_limits<T>::is_signed)
    {
        constexpr auto divisor_lower_bound {UINT64_MAX >> 1};
        if (divisor.high >= divisor_lower_bound)
        {
            T quotient {};
            quotient.low = static_cast<std::uint64_t>(dividend.high / divisor.high);

            // Compute q * divisor
            std::uint64_t p0_hi;
            auto p0_lo {_umul128(quotient.low, divisor.low, &p0_hi)};

            std::uint64_t p1_hi;
            const auto p1_lo {_umul128(quotient.low, static_cast<std::uint64_t>(divisor.high), &p1_hi)};

            // Combine: product = (p1_hi : p1_lo + p0_hi : p0_lo)
            T product {};
            product.low = p0_lo;
            const auto carry {BOOST_INT128_ADD_CARRY(0, p0_hi, p1_lo,
                reinterpret_cast<std::uint64_t*>(&product.high))};
            p1_hi += static_cast<std::uint64_t>(carry);

            // Check if quotient is too large (at most by 1)
            if (p1_hi > 0 || product > dividend)
            {
                --quotient.low;
                // Subtract divisor from product instead of recomputing
                const auto borrow {BOOST_INT128_SUB_BORROW(0, product.low, divisor.low, &product.low)};
                BOOST_INT128_SUB_BORROW(borrow, static_cast<std::uint64_t>(product.high),
                    static_cast<std::uint64_t>(divisor.high),
                    reinterpret_cast<std::uint64_t*>(&product.high));
            }

            BOOST_INT128_IF_CONSTEXPR (needs_mod)
            {
                // remainder = dividend - product
                const auto borrow {BOOST_INT128_SUB_BORROW(0, dividend.low, product.low, &remainder.low)};
                BOOST_INT128_SUB_BORROW(borrow, static_cast<std::uint64_t>(dividend.high),
                    static_cast<std::uint64_t>(product.high),
                    reinterpret_cast<std::uint64_t*>(&remainder.high));
            }

            return quotient;
        }
    }

    // Special case: divisor.low == 0 simplifies to single _udiv128
    if (divisor.low == 0)
    {
        const auto shift_amount {countl_zero(static_cast<std::uint64_t>(divisor.high))};
        const auto divisor_normalized {static_cast<std::uint64_t>(divisor.high) << shift_amount};
        const auto complement_shift {64 - shift_amount};

        std::uint64_t high_digit;
        std::uint64_t dividend_high_shifted;

        if (shift_amount == 0)
        {
            high_digit = 0;
            dividend_high_shifted = static_cast<std::uint64_t>(dividend.high);
        }
        else
        {
            high_digit = static_cast<std::uint64_t>(dividend.high) >> complement_shift;
            dividend_high_shifted = (static_cast<std::uint64_t>(dividend.high) << shift_amount) |
                (dividend.low >> complement_shift);
        }

        T quotient {};
        std::uint64_t rem_high;

        if (high_digit >= divisor_normalized)
        {
            quotient.high = 1;
            high_digit -= divisor_normalized;
        }

        quotient.low = _udiv128(high_digit, dividend_high_shifted, divisor_normalized, &rem_high);

        BOOST_INT128_IF_CONSTEXPR (needs_mod)
        {
            remainder.high = 0;
            remainder.low = rem_high >> shift_amount;
        }

        return quotient;
    }

    // General case: normalize and use refined quotient estimation
    const auto shift_amount {countl_zero(static_cast<std::uint64_t>(divisor.high))};
    const auto complement_shift {64 - shift_amount};

    // Normalize divisor
    std::uint64_t divisor_high_normalized;
    std::uint64_t divisor_low_normalized;

    if (shift_amount == 0)
    {
        divisor_high_normalized = static_cast<std::uint64_t>(divisor.high);
        divisor_low_normalized = divisor.low;
    }
    else
    {
        divisor_high_normalized = (static_cast<std::uint64_t>(divisor.high) << shift_amount) |
            (divisor.low >> complement_shift);
        divisor_low_normalized = divisor.low << shift_amount;
    }

    // Normalize dividend (need extra high digit)
    std::uint64_t high_digit;
    std::uint64_t dividend_high_shifted;
    std::uint64_t dividend_low_shifted;

    if (shift_amount == 0)
    {
        high_digit = 0;
        dividend_high_shifted = static_cast<std::uint64_t>(dividend.high);
        dividend_low_shifted = dividend.low;
    }
    else
    {
        high_digit = static_cast<std::uint64_t>(dividend.high) >> complement_shift;
        dividend_high_shifted = (static_cast<std::uint64_t>(dividend.high) << shift_amount) |
            (dividend.low >> complement_shift);
        dividend_low_shifted = dividend.low << shift_amount;
    }

    // Initial quotient estimate: divide (high_digit : dividend_high_shifted) by divisor_high_normalized
    T quotient {};
    std::uint64_t q_estimate;
    std::uint64_t rem_estimate;

    if (high_digit >= divisor_high_normalized)
    {
        // Quotient won't fit in 64 bits from this division alone
        quotient.high = 1;
        q_estimate = _udiv128(high_digit - divisor_high_normalized, dividend_high_shifted,
            divisor_high_normalized, &rem_estimate);
    }
    else
    {
        q_estimate = _udiv128(high_digit, dividend_high_shifted, divisor_high_normalized, &rem_estimate);
    }

    // Refine estimate: check if q_estimate * divisor_low_normalized > (rem_estimate : dividend_low_shifted)
    // This correction is needed at most twice
    std::uint64_t prod_lo_hi;
    auto prod_lo_lo {_umul128(q_estimate, divisor_low_normalized, &prod_lo_hi)};

    // Compare (prod_lo_hi : prod_lo_lo) with (rem_estimate : dividend_low_shifted)
    if (prod_lo_hi > rem_estimate ||
        (prod_lo_hi == rem_estimate && prod_lo_lo > dividend_low_shifted))
    {
        --q_estimate;
        const auto old_rem {rem_estimate};
        rem_estimate += divisor_high_normalized;

        // Check for overflow in rem_estimate (means no more corrections needed)
        if (rem_estimate >= old_rem)
        {
            // Recompute product for second check
            prod_lo_lo = _umul128(q_estimate, divisor_low_normalized, &prod_lo_hi);

            if (prod_lo_hi > rem_estimate ||
                (prod_lo_hi == rem_estimate && prod_lo_lo > dividend_low_shifted))
            {
                --q_estimate;
            }
        }
    }

    quotient.low = q_estimate;

    // Compute remainder if needed
    BOOST_INT128_IF_CONSTEXPR (needs_mod)
    {
        // Compute q_estimate * divisor_normalized and subtract from normalized dividend
        std::uint64_t p0_hi;
        const auto p0_lo {_umul128(q_estimate, divisor_low_normalized, &p0_hi)};

        std::uint64_t p1_hi;
        const auto p1_lo {_umul128(q_estimate, divisor_high_normalized, &p1_hi)};

        // Combine middle terms
        std::uint64_t product_mid;
        auto mid_carry {BOOST_INT128_ADD_CARRY(0, p1_lo, p0_hi, &product_mid)};

        // Subtract product from normalized dividend
        std::uint64_t rem_low;
        std::uint64_t rem_high;
        auto borrow {BOOST_INT128_SUB_BORROW(0, dividend_low_shifted, p0_lo, &rem_low)};
        borrow = BOOST_INT128_SUB_BORROW(borrow, dividend_high_shifted, product_mid, &rem_high);
        borrow = BOOST_INT128_SUB_BORROW(borrow, high_digit, p1_hi + mid_carry, &high_digit);

        // Handle quotient.high contribution
        if (quotient.high)
        {
            auto b2 {BOOST_INT128_SUB_BORROW(0, rem_low, divisor_low_normalized, &rem_low)};
            b2 = BOOST_INT128_SUB_BORROW(b2, rem_high, divisor_high_normalized, &rem_high);
            borrow = BOOST_INT128_SUB_BORROW(b2, high_digit, 0, &high_digit);
        }

        // If there's a borrow, we over-subtracted - add divisor back and decrement quotient
        if (borrow)
        {
            auto carry {BOOST_INT128_ADD_CARRY(0, rem_low, divisor_low_normalized, &rem_low)};
            BOOST_INT128_ADD_CARRY(carry, rem_high, divisor_high_normalized, &rem_high);
            --quotient.low;
            if (quotient.low == UINT64_MAX && quotient.high)
            {
                --quotient.high;
            }
        }

        // Un-normalize remainder
        if (shift_amount == 0)
        {
            remainder.low = rem_low;
            remainder.high = static_cast<high_word_type>(rem_high);
        }
        else
        {
            remainder.low = (rem_low >> shift_amount) | (rem_high << complement_shift);
            remainder.high = static_cast<high_word_type>(rem_high >> shift_amount);
        }
    }

    return quotient;
}

#endif

} // namespace impl

// We only need to take the time to process the remainder in the modulo case
// In the division case it is a waste of cycles

template <typename T>
BOOST_INT128_FORCE_INLINE constexpr void one_word_div(const T& lhs, const std::uint64_t rhs, T& quotient) noexcept
{
    #if defined(_M_AMD64) && !defined(__GNUC__) && !defined(__clang__) && _MSC_VER >= 1920 && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION)

    if (!BOOST_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        using high_word_type = decltype(T{}.high);

        quotient.high = static_cast<high_word_type>(static_cast<std::uint64_t>(lhs.high) / rhs);
        auto remainder {static_cast<std::uint64_t>(lhs.high) % rhs};
        quotient.low = _udiv128(remainder, lhs.low, rhs, &remainder);
        return;
    }

    #endif

    if (rhs <= UINT32_MAX)
    {
        half_word_div(lhs, static_cast<std::uint32_t>(rhs), quotient);
    }
    else
    {
        std::uint32_t u[4] {};
        std::uint32_t v[2] {};
        std::uint32_t q[4] {};

        const auto m {impl::to_words(lhs, u)};
        const auto n {impl::to_words(rhs, v)};

        impl::knuth_divide<false>(u, m, v, n, q);

        quotient = impl::from_words<T>(q);
    }
}

template <typename T>
BOOST_INT128_FORCE_INLINE constexpr void one_word_div(const T& lhs, const std::uint64_t rhs, T& quotient, T& remainder) noexcept
{
    #if defined(_M_AMD64) && !defined(__GNUC__) && !defined(__clang__) && _MSC_VER >= 1920 && !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION)

    if (!BOOST_INT128_IS_CONSTANT_EVALUATED(lhs))
    {
        using high_word_type = decltype(T{}.high);

        quotient.high = static_cast<high_word_type>(static_cast<std::uint64_t>(lhs.high) / rhs);
        remainder.low = static_cast<std::uint64_t>(lhs.high) % rhs;
        quotient.low = _udiv128(remainder.low, lhs.low, rhs, &remainder.low);
        return;
    }

    #else

    if (rhs <= UINT32_MAX)
    {
        half_word_div(lhs, static_cast<std::uint32_t>(rhs), quotient, remainder);
    }
    else
    {
        std::uint32_t u[4] {};
        std::uint32_t v[2] {};
        std::uint32_t q[4] {};

        const auto m {impl::to_words(lhs, u)};
        const auto n {impl::to_words(rhs, v)};

        impl::knuth_divide<true>(u, m, v, n, q);

        quotient = impl::from_words<T>(q);
        remainder = impl::from_words<T>(u);
    }

    #endif
}

template <typename T>
BOOST_INT128_FORCE_INLINE constexpr void one_word_div(const T& lhs, const std::uint32_t rhs, T& quotient, T& remainder) noexcept
{
    half_word_div(lhs, rhs, quotient, remainder);
}

template <typename T>
BOOST_INT128_FORCE_INLINE constexpr void one_word_div(const T& lhs, const std::uint32_t rhs, T& quotient) noexcept
{
    half_word_div(lhs, rhs, quotient);
}

#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable : 4127) // Conditional expression is constant is true pre-C++17
#  pragma warning(disable : 4804) // Unsafe comparison with bool
#endif

template <typename T>
BOOST_INT128_FORCE_INLINE constexpr T knuth_div(const T& dividend, const T& divisor) noexcept
{
    BOOST_INT128_ASSUME(divisor != static_cast<T>(0));

    #if defined(_M_AMD64) && !defined(__GNUC__) && !defined(__clang__) && _MSC_VER >= 1920

    BOOST_INT128_IF_CONSTEXPR(!std::numeric_limits<T>::is_signed)
    {
        if (!BOOST_INT128_IS_CONSTANT_EVALUATED(dividend))
        {
            T remainder {};
            return impl::div_mod_msvc<false>(dividend, divisor, remainder);
        }
    }

    #endif

    std::uint32_t u[4] {};
    std::uint32_t v[4] {};
    std::uint32_t q[4] {};

    const auto m {impl::to_words(dividend, u)};
    const auto n {impl::to_words(divisor, v)};

    impl::knuth_divide<false>(u, m, v, n, q);

    return impl::from_words<T>(q);
}

template <typename T>
BOOST_INT128_FORCE_INLINE constexpr T knuth_div(const T& dividend, const T& divisor, T& remainder) noexcept
{
    BOOST_INT128_ASSUME(divisor != static_cast<T>(0));

    #if defined(_M_AMD64) && !defined(__GNUC__) && !defined(__clang__) && _MSC_VER >= 1920

    BOOST_INT128_IF_CONSTEXPR(!std::numeric_limits<T>::is_signed)
    {
        if (!BOOST_INT128_IS_CONSTANT_EVALUATED(dividend))
        {
            return impl::div_mod_msvc<true>(dividend, divisor, remainder);
        }
    }


    #endif

    std::uint32_t u[4] {};
    std::uint32_t v[4] {};
    std::uint32_t q[4] {};

    const auto m {impl::to_words(dividend, u)};
    const auto n {impl::to_words(divisor, v)};

    impl::knuth_divide<true>(u, m, v, n, q);

    remainder = impl::from_words<T>(u);

    return impl::from_words<T>(q);
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
