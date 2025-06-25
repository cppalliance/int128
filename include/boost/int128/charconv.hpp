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

} // namespace detail

BOOST_CHARCONV_CONSTEXPR to_chars_result to_chars(char* first, char* last, int128::uint128_t value, int base = 10) noexcept
{
    if (base == 10)
    {
        return detail::to_chars_128integer_impl<int128::uint128_t, int128::uint128_t>(first, last, value);
    }

    return detail::to_chars_integer_impl<int128::uint128_t, int128::uint128_t>(first, last, value, base);
}

BOOST_CHARCONV_CONSTEXPR to_chars_result to_chars(char* first, char* last, int128::int128_t value, int base = 10) noexcept
{
    if (base == 10)
    {
        return detail::to_chars_128integer_impl<int128::int128_t, int128::uint128_t>(first, last, value);
    }

    return detail::to_chars_integer_impl<int128::int128_t, int128::uint128_t>(first, last, value, base);
}

BOOST_CHARCONV_GCC5_CONSTEXPR from_chars_result from_chars(const char* first, const char* last, int128::uint128_t& value, int base = 10) noexcept
{
    return detail::from_chars_integer_impl<int128::uint128_t, int128::uint128_t>(first, last, value, base);
}

BOOST_CHARCONV_GCC5_CONSTEXPR from_chars_result from_chars(core::string_view sv, int128::uint128_t& value, int base = 10) noexcept
{
    return detail::from_chars_integer_impl<int128::uint128_t, int128::uint128_t>(sv.data(), sv.data() + sv.size(), value, base);
}

BOOST_CHARCONV_GCC5_CONSTEXPR from_chars_result from_chars(const char* first, const char* last, int128::int128_t& value, int base = 10) noexcept
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
