// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_DETAIL_KNUTH_DIV_HPP
#define BOOST_INT128_DETAIL_KNUTH_DIV_HPP

#include <boost/int128/detail/config.hpp>
#include <boost/int128/detail/clz.hpp>
#include <cstdint>
#include <cstring>

namespace boost {
namespace int128 {
namespace detail {

#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wassume"
#endif

template <typename T>
BOOST_INT128_FORCE_INLINE void div_mod_less_2_e_32(const T& lhs, const std::uint64_t rhs, T& quotient, T& remainder) noexcept
{
    BOOST_INT128_ASSUME(rhs != 0); // LCOV_EXCL_LINE

    remainder.low = (lhs.high << 32) | (lhs.low >> 32);
    quotient.low = remainder.low / static_cast<std::uint32_t>(rhs);
    remainder.low = ((remainder.low % static_cast<std::uint32_t>(rhs)) << 32) | static_cast<std::uint32_t>(lhs.low);
    quotient.low = (quotient.low << 32) | (remainder.low / static_cast<std::uint32_t>(rhs));
    remainder.low %= static_cast<std::uint32_t>(rhs);
}

// If rhs is greater than 2^32 the result is trivial to find
template <typename T>
BOOST_INT128_FORCE_INLINE void div_mod_greater_2_e_32(const T& lhs, const std::uint64_t rhs, T& quotient, T& remainder) noexcept
{
    using high_word_type = decltype(T{}.high);

    BOOST_INT128_ASSUME(rhs != 0); // LCOV_EXCL_LINE

    #if !defined(BOOST_INT128_NO_CONSTEVAL_DETECTION) && defined(_M_AMD64)

    if (!BOOST_INT128_IS_CONSTANT_EVALUATED(rhs))
    {
        quotient.low = _udiv128(static_cast<std::uint64_t>(lhs.high), lhs.low, rhs, &remainder.low);
    }

    #endif

    if (lhs < rhs)
    {
        remainder = lhs;
    }
    else
    {
        quotient.low = static_cast<std::uint64_t>(lhs.high) / rhs;
        remainder.low = lhs.low;
        remainder.high = static_cast<high_word_type>(static_cast<std::uint64_t>(lhs.high) % rhs);
    }

    const auto n_hi {static_cast<std::uint64_t>(remainder.high)};
    const auto n_lo {remainder.low};

    const auto temp_quo {((n_hi << 32) | (n_lo >> 32)) / rhs};
    const auto temp_rem {((n_hi << 32) | (n_lo >> 32)) % rhs};

    quotient.low = (quotient.low << 32) | temp_quo;

    const auto final_dividend {(temp_rem << 32) | (n_lo & UINT32_MAX)};
    const auto final_quo {final_dividend / rhs};
    const auto final_rem {final_dividend % rhs};

    quotient.low = (quotient.low << 32) | final_quo;
    remainder.low = final_rem;
    remainder.high = 0;
}

template <typename T>
BOOST_INT128_FORCE_INLINE constexpr T half_word_div(const T& lhs, const std::uint32_t rhs, T& quotient, T& remainder) noexcept
{
    using high_word_type = decltype(T{}.high);

    BOOST_INT128_ASSUME(rhs != 0); // LCOV_EXCL_LINE

    const auto rhs32 = static_cast<std::uint32_t>(rhs);
    auto abs_lhs {abs(lhs)};

    auto current = static_cast<std::uint64_t>(abs_lhs.high >> 32U);
    quotient.high = static_cast<high_word_type>(static_cast<std::uint64_t>(static_cast<std::uint32_t>(current / rhs32)) << 32U);
    remainder.low = static_cast<std::uint64_t>(current % rhs32);

    current = static_cast<std::uint64_t>(remainder.low << 32U) | static_cast<std::uint32_t>(abs_lhs.high);
    quotient.high |= static_cast<high_word_type>(current / rhs32);
    remainder.low = static_cast<std::uint64_t>(current % rhs32);

    current = static_cast<std::uint64_t>(remainder.low << 32U) | static_cast<std::uint32_t>(abs_lhs.low >> 32U);
    quotient.low = static_cast<std::uint64_t>(static_cast<std::uint64_t>(static_cast<std::uint32_t>(current / rhs32)) << 32U);
    remainder.low = static_cast<std::uint64_t>(current % rhs32);

    current = remainder.low << 32U | static_cast<std::uint32_t>(abs_lhs.low);
    quotient.low |= static_cast<std::uint32_t>(current / rhs32);
    remainder.low = static_cast<std::uint32_t>(current % rhs32);

    return quotient;
}

template <typename T>
BOOST_INT128_FORCE_INLINE constexpr void one_word_div(const T& lhs, const std::uint64_t rhs, T& quotient, T& remainder) noexcept
{
    if (rhs >= UINT32_MAX)
    {
        div_mod_greater_2_e_32(lhs, rhs, quotient, remainder);
    }
    else
    {
        half_word_div(lhs, static_cast<std::uint32_t>(rhs), quotient, remainder);
    }
}

template <typename T>
BOOST_INT128_FORCE_INLINE constexpr void one_word_div(const T& lhs, const std::uint32_t rhs, T& quotient, T& remainder) noexcept
{
    half_word_div(lhs, rhs, quotient, remainder);
}

// See: The Art of Computer Programming Volume 2 (Semi-numerical algorithms) section 4.3.1
// Algorithm D: Division of Non-negative integers
template <std::size_t u_size, std::size_t v_size, std::size_t q_size>
constexpr void knuth_divide(std::uint32_t (&u)[u_size], const std::size_t m,
                            const std::uint32_t (&v)[v_size], const std::size_t n,
                            std::uint32_t (&q)[q_size]) noexcept
{
    // D.1
    const auto s {countl_zero(v[n - 1])};

    // Create normalized versions of u and v
    std::uint32_t un[5] {};
    std::uint32_t vn[4] {};

    if (s > 0)
    {
        for (std::size_t i {n-1}; i > 0; i--)
        {
            vn[i] = (v[i] << s) | (v[i-1] >> (32 - s));
        }
        vn[0] = v[0] << s;

        un[m] = u[m-1] >> (32-s);
        for (std::size_t i {m-1}; i > 0; i--)
        {
            un[i] = (u[i] << s) | (u[i-1] >> (32-s));
        }
        un[0] = u[0] << s;
    }
    else
    {
        // No normalization needed
        for (std::size_t i {}; i < n; i++)
        {
            vn[i] = v[i];
        }
        for (std::size_t i {}; i < m; i++)
        {
            un[i] = u[i];
        }
    }

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
        if (borrow < 0)
        {
            // D.6
            --q[j];
            std::uint64_t carry {};
            for (std::size_t i = 0; i < n; ++i)
            {
                carry += static_cast<std::uint64_t>(un[j+i]) + vn[i];
                un[j+i] = static_cast<std::uint32_t>(carry & UINT32_MAX);
                carry >>= 32U;
            }
            un[j+n] += static_cast<std::uint32_t>(carry & UINT32_MAX);
        }
    }

    // D.8
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

template <typename T>
BOOST_INT128_FORCE_INLINE constexpr void to_words(const T& x, std::uint32_t (&words)[4], std::size_t& word_count) noexcept
{
    words[0] = static_cast<std::uint32_t>(x.low & UINT32_MAX);
    words[1] = static_cast<std::uint32_t>(x.low >> 32);
    words[2] = static_cast<std::uint32_t>(x.high & UINT32_MAX);
    words[3] = static_cast<std::uint32_t>(x.high >> 32);

    word_count = 4;
    while (word_count > 0 && words[word_count-1] == 0)
    {
        word_count--;
    }

    if (word_count == 0)
    {
        ++word_count;
    }
}

template <typename T>
BOOST_INT128_FORCE_INLINE constexpr T from_words(const std::uint32_t (&words)[4]) noexcept
{
    using high_word_type = decltype(T{}.high);

    const auto low {static_cast<std::uint64_t>(words[0]) | (static_cast<std::uint64_t>(words[1]) << 32)};
    const auto high {static_cast<std::uint64_t>(words[2]) | (static_cast<std::uint64_t>(words[3]) << 32)};

    return {static_cast<high_word_type>(high), low};
}

template <typename T>
BOOST_INT128_FORCE_INLINE constexpr T knuth_div_driver(const T& dividend, const T& divisor, T& remainder) noexcept
{
    BOOST_INT128_ASSUME(divisor != 0);

    std::uint32_t u[4] {};
    std::uint32_t v[4] {};
    std::uint32_t q[4] {};

    std::size_t m {};
    std::size_t n {};

    to_words(dividend, u, m);
    to_words(divisor, v, n);

    knuth_divide(u, m, v, n, q);

    remainder = from_words<T>(u);

    return from_words<T>(q);
}

#if defined(__clang__)
#  pragma clang diagnostic pop
#endif


} // namespace detail
} // namespace int128
} // namespace boost

#endif // BOOST_INT128_DETAIL_KNUTH_DIV_HPP
