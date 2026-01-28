// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
//
// This header injects additional functionality into the Boost.Charconv namespace
// If this library is accepted into boost this functionality could be moved their

#ifndef BOOST_INT128_CHARCONV_HPP
#define BOOST_INT128_CHARCONV_HPP

#if __has_include(<boost/charconv.hpp>)

#include <boost/int128/int128.hpp>
#include <boost/int128/literals.hpp>
#include <boost/charconv.hpp>
#include <boost/core/detail/string_view.hpp>

namespace boost {
namespace charconv {

namespace detail {

template <>
struct is_signed<int128::uint128_t> { static constexpr bool value = false; };

template <>
struct is_signed<int128::int128_t> { static constexpr bool value = true; };

template <>
struct make_unsigned<int128::uint128_t> { using type = int128::uint128_t; };

template <>
struct make_unsigned<int128::int128_t> { using type = int128::uint128_t; };

template <>
struct make_signed<int128::uint128_t> { using type = int128::int128_t; };

template <>
struct make_signed<int128::int128_t> { using type = int128::int128_t; };

BOOST_INT128_INLINE_CONSTEXPR int128::uint128_t int128_pow10[39] =
{
    BOOST_INT128_UINT128_C(1),
    BOOST_INT128_UINT128_C(10),
    BOOST_INT128_UINT128_C(100),
    BOOST_INT128_UINT128_C(1000),
    BOOST_INT128_UINT128_C(10000),
    BOOST_INT128_UINT128_C(100000),
    BOOST_INT128_UINT128_C(1000000),
    BOOST_INT128_UINT128_C(10000000),
    BOOST_INT128_UINT128_C(100000000),
    BOOST_INT128_UINT128_C(1000000000),
    BOOST_INT128_UINT128_C(10000000000),
    BOOST_INT128_UINT128_C(100000000000),
    BOOST_INT128_UINT128_C(1000000000000),
    BOOST_INT128_UINT128_C(10000000000000),
    BOOST_INT128_UINT128_C(100000000000000),
    BOOST_INT128_UINT128_C(1000000000000000),
    BOOST_INT128_UINT128_C(10000000000000000),
    BOOST_INT128_UINT128_C(100000000000000000),
    BOOST_INT128_UINT128_C(1000000000000000000),
    BOOST_INT128_UINT128_C(10000000000000000000),
    BOOST_INT128_UINT128_C(100000000000000000000),
    BOOST_INT128_UINT128_C(1000000000000000000000),
    BOOST_INT128_UINT128_C(10000000000000000000000),
    BOOST_INT128_UINT128_C(100000000000000000000000),
    BOOST_INT128_UINT128_C(1000000000000000000000000),
    BOOST_INT128_UINT128_C(10000000000000000000000000),
    BOOST_INT128_UINT128_C(100000000000000000000000000),
    BOOST_INT128_UINT128_C(1000000000000000000000000000),
    BOOST_INT128_UINT128_C(10000000000000000000000000000),
    BOOST_INT128_UINT128_C(100000000000000000000000000000),
    BOOST_INT128_UINT128_C(1000000000000000000000000000000),
    BOOST_INT128_UINT128_C(10000000000000000000000000000000),
    BOOST_INT128_UINT128_C(100000000000000000000000000000000),
    BOOST_INT128_UINT128_C(1000000000000000000000000000000000),
    BOOST_INT128_UINT128_C(10000000000000000000000000000000000),
    BOOST_INT128_UINT128_C(100000000000000000000000000000000000),
    BOOST_INT128_UINT128_C(1000000000000000000000000000000000000),
    BOOST_INT128_UINT128_C(10000000000000000000000000000000000000),
    BOOST_INT128_UINT128_C(100000000000000000000000000000000000000)
};

constexpr int num_digits(const int128::uint128_t& x) noexcept
{
    if (x.high == UINT64_C(0))
    {
        return num_digits(x.low);
    }

    // Use the most significant bit position to approximate log10
    // log10(x) ~= log2(x) / log2(10) ~= log2(x) / 3.32

    const auto msb {64 + (63 - int128::detail::impl::countl_impl(x.high))};
    
    // Approximate log10
    const auto estimated_digits {(msb * 1000) / 3322 + 1};

    if (estimated_digits < 39 && x >= int128_pow10[static_cast<std::size_t>(estimated_digits)])
    {
        return estimated_digits + 1;
    }

    // Estimated digits can't be less than 20 digits (65-bits minimum)
    if (x < int128_pow10[estimated_digits - 1])
    {
        return estimated_digits - 1;
    }

    return estimated_digits;
}

} // namespace detail

BOOST_CHARCONV_CONSTEXPR to_chars_result to_chars(char* first, char* last, const int128::uint128_t value, const int base = 10) noexcept
{
    if (base == 10)
    {
        return detail::to_chars_128integer_impl<int128::uint128_t, int128::uint128_t>(first, last, value);
    }

    return detail::to_chars_integer_impl<int128::uint128_t, int128::uint128_t>(first, last, value, base);
}

BOOST_CHARCONV_CONSTEXPR to_chars_result to_chars(char* first, char* last, const int128::int128_t value, const int base = 10) noexcept
{
    if (base == 10)
    {
        return detail::to_chars_128integer_impl<int128::int128_t, int128::uint128_t>(first, last, value);
    }

    return detail::to_chars_integer_impl<int128::int128_t, int128::uint128_t>(first, last, value, base);
}

BOOST_CHARCONV_GCC5_CONSTEXPR from_chars_result from_chars(const char* first, const char* last, int128::uint128_t& value, const int base = 10) noexcept
{
    return detail::from_chars_integer_impl<int128::uint128_t, int128::uint128_t>(first, last, value, base);
}

BOOST_CHARCONV_GCC5_CONSTEXPR from_chars_result from_chars(core::string_view sv, int128::uint128_t& value, const int base = 10) noexcept
{
    return detail::from_chars_integer_impl<int128::uint128_t, int128::uint128_t>(sv.data(), sv.data() + sv.size(), value, base);
}

BOOST_CHARCONV_GCC5_CONSTEXPR from_chars_result from_chars(const char* first, const char* last, int128::int128_t& value, const int base = 10) noexcept
{
    return detail::from_chars_integer_impl<int128::int128_t, int128::uint128_t>(first, last, value, base);
}

BOOST_CHARCONV_GCC5_CONSTEXPR from_chars_result from_chars(core::string_view sv, int128::int128_t& value, int base = 10) noexcept
{
    return detail::from_chars_integer_impl<int128::int128_t, int128::uint128_t>(sv.data(), sv.data() + sv.size(), value, base);
}

} // namespace charconv
} // namespace boost

#else

#error "This header requires Boost.Charconv to be present"

#endif // __has_include(<boost/charconv.hpp>)

#endif // BOOST_INT128_CHARCONV_HPP
