// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128/detail/int128_imp.hpp>
#include <boost/int128/detail/conversions.hpp>
#include <boost/core/lightweight_test.hpp>
#include <boost/mp11.hpp>
#include <cstring>
#include <cstdint>
#include <random>
#include <limits>
#include <cmath>
#include <sstream>
#include <iostream>
#include <iomanip>

#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wold-style-cast"
#  pragma clang diagnostic ignored "-Wundef"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wsign-conversion"
#  pragma clang diagnostic ignored "-Wfloat-equal"
#  pragma clang diagnostic ignored "-Wsign-compare"
#  pragma clang diagnostic ignored "-Woverflow"

#  if (__clang_major__ >= 10 && !defined(__APPLE__)) || __clang_major__ >= 13
#    pragma clang diagnostic ignored "-Wdeprecated-copy"
#  endif

#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wundef"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wsign-conversion"
#  pragma GCC diagnostic ignored "-Wsign-compare"
#  pragma GCC diagnostic ignored "-Wfloat-equal"
#  pragma GCC diagnostic ignored "-Woverflow"

#elif defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 4389)
#  pragma warning(disable : 4127)
#endif

template <typename IntType>
constexpr IntType get_max()
{
    return std::numeric_limits<IntType>::max();
}

template <typename IntType>
constexpr IntType get_min()
{
    return std::numeric_limits<IntType>::min();
}

#include <boost/random/uniform_int_distribution.hpp>

// Used defined seed for repeatability
static std::mt19937_64 rng(42);

constexpr std::size_t N = 1024;

#ifdef BOOST_INT128_HAS_INT128

using boost::int128::detail::builtin_u128;
using boost::int128::detail::builtin_i128;

// We reduce the max end of the 128 bit types as they can cause a stack overflow in boost.random

template <>
constexpr builtin_u128 get_max<builtin_u128>()
{
    return static_cast<builtin_u128>(UINT64_MAX) << 64 | UINT64_MAX / 32;
}

template <>
constexpr builtin_u128 get_min<builtin_u128>()
{
    return 0;
}

template <>
constexpr builtin_i128 get_max<builtin_i128>()
{
    return static_cast<builtin_i128>((static_cast<builtin_u128>(1) << 127) - 1) / 32;
}

template <>
constexpr builtin_i128 get_min<builtin_i128>()
{
    return -get_max<builtin_i128>() - 1;
}

template <typename T>
constexpr T get_root_max()
{
    return std::numeric_limits<T>::max() / 10;
}

template <typename T>
constexpr T get_root_min()
{
    return std::numeric_limits<T>::min() / 10;
}

template <>
constexpr builtin_u128 get_root_max<builtin_u128>()
{
    return (UINT64_MAX >> 2);
}

template <>
constexpr builtin_u128 get_root_min<builtin_u128>()
{
    return 0;
}

template <>
constexpr builtin_i128 get_root_max<builtin_i128>()
{
    return INT64_MAX;
}

template <>
constexpr builtin_i128 get_root_min<builtin_i128>()
{
    return INT64_MIN;
}

template <typename IntType>
void test_arithmetic_constructor()
{
    boost::random::uniform_int_distribution<IntType> dist(get_min<IntType>(),
                                                          get_max<IntType>());

    for (std::size_t i {}; i < N; ++i)
    {
        const IntType value {dist(rng)};
        const auto builtin_value = static_cast<builtin_u128>(value);
        boost::int128::uint128_t emulated_value {value};

        builtin_u128 emulated_bits;
        std::memcpy(&emulated_bits, &emulated_value, sizeof(builtin_u128));

        BOOST_TEST(emulated_bits == builtin_value);
    }
}

template <typename IntType>
void test_assignment_operators()
{
    boost::random::uniform_int_distribution<IntType> dist(get_min<IntType>(),
                                                          get_max<IntType>());

    for (std::size_t i {}; i < N; ++i)
    {
        const IntType value {dist(rng)};
        builtin_u128 builtin_value {};
        builtin_value = static_cast<builtin_u128>(value);
        boost::int128::uint128_t emulated_value {};
        emulated_value = value;

        builtin_u128 emulated_bits;
        std::memcpy(&emulated_bits, &emulated_value, sizeof(builtin_u128));

        BOOST_TEST(emulated_bits == builtin_value);
    }
}

template <typename IntType>
void test_integer_conversion_operators()
{
    boost::random::uniform_int_distribution<IntType> dist(get_min<IntType>(),
                                                          get_max<IntType>());

    for (std::size_t i {}; i < N; ++i)
    {
        const IntType value {dist(rng)};
        builtin_u128 builtin_value;
        builtin_value = static_cast<builtin_u128>(value);
        boost::int128::uint128_t emulated_value {};
        emulated_value = value;

        const auto builtin_value_return = static_cast<IntType>(builtin_value);
        const auto emulated_value_return = static_cast<IntType>(emulated_value);

        BOOST_TEST(builtin_value_return == emulated_value_return);

        // Hits the implicit bool conversion
        if (builtin_value)
        {
            BOOST_TEST(emulated_value);
        }
    }
}

template <typename FloatType>
void test_float_conversion_operators()
{
    // Float128 won't have numerics limit defined all the time,
    // Plus this affords some additional tolerance
    constexpr FloatType error_tol {std::is_same<FloatType, float>::value ?
        static_cast<FloatType>(std::numeric_limits<float>::epsilon()) :
        static_cast<FloatType>(std::numeric_limits<double>::epsilon())};

    for (std::size_t i {}; i < N; ++i)
    {
        BOOST_INT128_IF_CONSTEXPR (!std::is_same<FloatType, float>::value)
        {
            boost::random::uniform_int_distribution<std::uint32_t> dist(std::numeric_limits<std::uint32_t>::min(),
                                                                        std::numeric_limits<std::uint32_t>::max());

            const auto value {dist(rng)};
            builtin_u128 builtin_value;
            builtin_value = static_cast<builtin_u128>(value) << 64 | static_cast<builtin_u128>(value);
            boost::int128::uint128_t emulated_value {value, value};

            // Converts the value and then normalizes the range
            const auto builtin_value_return = static_cast<FloatType>(builtin_value) / static_cast<FloatType>(1e27L);
            const auto emulated_value_return = static_cast<FloatType>(emulated_value) / static_cast<FloatType>(1e27L);
            FloatType distance = builtin_value_return - emulated_value_return;

            distance = distance < 0 ? -distance : distance;

            BOOST_TEST(distance < error_tol);
        }
        else
        {
            boost::random::uniform_int_distribution<std::uint64_t> dist(std::numeric_limits<std::uint64_t>::min(),
                                                                        std::numeric_limits<std::uint64_t>::max());

            const auto value {dist(rng)};
            builtin_u128 builtin_value;
            builtin_value = value;
            boost::int128::uint128_t emulated_value {};
            emulated_value = value;

            const auto builtin_value_return = static_cast<FloatType>(builtin_value);
            const auto emulated_value_return = static_cast<FloatType>(emulated_value);
            FloatType distance = builtin_value_return - emulated_value_return;

            distance = distance < 0 ? -distance : distance;

            BOOST_TEST(distance < error_tol);
        }
    }
}

struct test_caller
{
    template<typename T>
    void operator()(T) const
    {
        test_arithmetic_constructor<T>();
        test_assignment_operators<T>();
        test_integer_conversion_operators<T>();
    }
};

int main()
{
    using test_types = boost::mp11::mp_list<
        char,
        unsigned char,
        char16_t,
        char32_t,
        wchar_t,
        short,
        unsigned short,
        int,
        unsigned int,
        long,
        unsigned long,
        long long,
        unsigned long long,
        builtin_i128,
        builtin_u128
    >;

    boost::mp11::mp_for_each<test_types>(test_caller());

    test_float_conversion_operators<float>();
    test_float_conversion_operators<double>();
    test_float_conversion_operators<long double>();

    return boost::report_errors();
}

#else

int main()
{
    static_cast<void>(N);
    return 0;
}

#endif // BOOST_INT128_HAS_INT128
