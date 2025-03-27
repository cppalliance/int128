// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_DETAIL_TRAITS_HPP
#define BOOST_INT128_DETAIL_TRAITS_HPP

#include <boost/int128/detail/config.hpp>
#include <type_traits>

#ifdef BOOST_INT128_HAS_CONCEPTS
#include <concepts>
#endif

namespace boost {
namespace int128 {
namespace detail {

template <typename T>
struct signed_integer
{
    static constexpr bool value = std::is_signed<T>::value && std::is_integral<T>::value;
};

template <typename T>
static constexpr bool is_signed_integer_v = signed_integer<T>::value;

template <typename T>
struct unsigned_integer
{
    static constexpr bool value = std::is_unsigned<T>::value && std::is_integral<T>::value;
};

template <typename T>
static constexpr bool is_unsigned_integer_v = unsigned_integer<T>::value;

template <typename T>
static constexpr bool is_any_integer_v = signed_integer<T>::value || unsigned_integer<T>::value
                                         #ifdef BOOST_INT128_HAS_INT128
                                         || std::is_same<T, builtin_i128>::value || std::is_same<T, builtin_u128>::value
                                         #endif // BOOST_INT128_HAS_INT128
                                         ;

#ifdef BOOST_INT128_HAS_CONCEPTS

concept signed_integer = signed_integer_v<T>;
concept unsigned_integer = unsigned_integer_v<T>;
concept integer = is_any_integer_v<T>;

#endif

} // namespace detail
} // namespace int128
} // namespace boost

#ifdef BOOST_INT128_HAS_CONCEPTS

#define BOOST_INT128_SIGNED_INTEGER_CONCEPT signed_integer SignedInteger
#define BOOST_INT128_UNSIGNED_INTEGER_CONCEPT unsigned_integer UnsignedInteger
#define BOOST_INT128_INTEGER_CONCEPT integer Integer

#define BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT BOOST_INT128_SIGNED_INTEGER_CONCEPT
#define BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT BOOST_INT128_UNSIGNED_INTEGER_CONCEPT
#define BOOST_INT128_DEFAULTED_INTEGER_CONCEPT BOOST_INT128_INTEGER_CONCEPT

#else

#define BOOST_INT128_DEFAULTED_SIGNED_INTEGER_CONCEPT typename SignedInteger, std::enable_if_t<detail::is_signed_integer_v<SignedInteger>, bool> = true
#define BOOST_INT128_DEFAULTED_UNSIGNED_INTEGER_CONCEPT typename UnsignedInteger, std::enable_if_t<detail::is_unsigned_integer_v<UnsignedInteger>, bool> = true
#define BOOST_INT128_DEFAULTED_INTEGER_CONCEPT typename Integer, std::enable_if_t<detail::is_any_integer_v<Integer>, bool> = true

#define BOOST_INT128_SIGNED_INTEGER_CONCEPT typename SignedInteger, std::enable_if_t<detail::is_signed_integer_v<SignedInteger>, bool>
#define BOOST_INT128_UNSIGNED_INTEGER_CONCEPT typename UnsignedInteger, std::enable_if_t<detail::is_unsigned_integer_v<UnsignedInteger>, bool>
#define BOOST_INT128_INTEGER_CONCEPT typename Integer, std::enable_if_t<detail::is_any_integer_v<Integer>, bool>

#endif

#endif // BOOST_INT128_DETAIL_TRAITS_HPP
