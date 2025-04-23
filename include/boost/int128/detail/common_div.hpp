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

template <typename T>
BOOST_INT128_FORCE_INLINE void div_mod_less_2_e_32(const T& lhs, const std::uint64_t rhs, T& quotient, T& remainder) noexcept
{
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
        quotient.low = lhs.high / rhs;
        remainder.low = lhs.low;
        remainder.high = lhs.high % rhs;
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
    BOOST_INT128_ASSUME(rhs != 0);

    const auto rhs32 = static_cast<std::uint32_t>(rhs);
    auto abs_lhs {abs(lhs)};

    auto current = static_cast<std::uint64_t>(abs_lhs.high >> 32U);
    quotient.high = static_cast<std::uint64_t>(static_cast<std::uint64_t>(static_cast<std::uint32_t>(current / rhs32)) << 32U);
    remainder.low = static_cast<std::uint64_t>(current % rhs32);

    current = static_cast<std::uint64_t>(remainder.low << 32U) | static_cast<std::uint32_t>(abs_lhs.high);
    quotient.high |= static_cast<std::uint32_t>(current / rhs32);
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

} // namespace detail
} // namespace int128
} // namespace boost

#endif // BOOST_INT128_DETAIL_KNUTH_DIV_HPP
