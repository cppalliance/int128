// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_ALLOW_SIGN_CONVERSION
#  define BOOST_INT128_ALLOW_SIGN_CONVERSION
#endif

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
#  pragma warning(disable : 4702)
#endif

template <typename IntType>
IntType get_max()
{
    return std::numeric_limits<IntType>::max();
}

template <typename IntType>
IntType get_min()
{
    return std::numeric_limits<IntType>::min();
}

#include <boost/random/uniform_int_distribution.hpp>

// Used defined seed for repeatability
static std::mt19937_64 rng(42);

constexpr std::size_t N = 1024;

#if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INT128)

using boost::int128::detail::builtin_u128;
using boost::int128::detail::builtin_i128;

// We reduce the max end of the 128 bit types as they can cause a stack overflow in boost.random

template <>
builtin_u128 get_max<builtin_u128>()
{
    return static_cast<builtin_u128>(UINT64_MAX) << 64 | UINT64_MAX / 32;
}

template <>
builtin_u128 get_min<builtin_u128>()
{
    return 0;
}

template <>
builtin_i128 get_max<builtin_i128>()
{
    return static_cast<builtin_i128>((static_cast<builtin_u128>(1) << 127) - 1) / 32;
}

template <>
builtin_i128 get_min<builtin_i128>()
{
    return -get_max<builtin_i128>() - 1;
}

template <typename T>
T get_root_max()
{
    return std::numeric_limits<T>::max() / 10;
}

template <typename T>
T get_root_min()
{
    return std::numeric_limits<T>::min() / 10;
}

template <>
builtin_u128 get_root_max<builtin_u128>()
{
    return (UINT64_MAX >> 2);
}

template <>
builtin_u128 get_root_min<builtin_u128>()
{
    return 0;
}

template <>
builtin_i128 get_root_max<builtin_i128>()
{
    return INT64_MAX;
}

template <>
builtin_i128 get_root_min<builtin_i128>()
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
        builtin_i128 builtin_value = static_cast<builtin_i128>(value);
        boost::int128::int128_t emulated_value {value};

        builtin_i128 emulated_bits;
        std::memcpy(&emulated_bits, &emulated_value, sizeof(builtin_i128));

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
        builtin_i128 builtin_value;
        builtin_value = static_cast<builtin_i128>(value);
        boost::int128::int128_t emulated_value {};
        emulated_value = value;

        builtin_i128 emulated_bits;
        std::memcpy(&emulated_bits, &emulated_value, sizeof(builtin_i128));

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
        builtin_i128 builtin_value;
        builtin_value = static_cast<builtin_i128>(value);
        boost::int128::int128_t emulated_value {};
        emulated_value = value;

        const auto builtin_value_return = static_cast<IntType>(builtin_value);
        const auto emulated_value_return = static_cast<IntType>(emulated_value);

        BOOST_TEST(builtin_value_return == emulated_value_return);

        // Hits the implicit bool conversion
        if (builtin_value)
        {
            BOOST_TEST(emulated_value);
            const boost::int128::int128_t high_only {static_cast<std::int64_t>(builtin_value), 0};
            BOOST_TEST(high_only);
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

            const auto value {dist(rng)}; // LCOV_EXCL_LINE
            builtin_i128 builtin_value;
            builtin_value = static_cast<builtin_i128>(value) << 64 | static_cast<builtin_i128>(value);
            boost::int128::int128_t emulated_value {static_cast<std::int64_t>(value), value};

            // Converts the value and then normalizes the range
            const auto builtin_value_return = static_cast<FloatType>(builtin_value) / static_cast<FloatType>(1e27L);
            const auto emulated_value_return = static_cast<FloatType>(emulated_value) / static_cast<FloatType>(1e27L);
            FloatType distance = builtin_value_return - emulated_value_return;

            distance = distance < 0 ? -distance : distance; // LCOV_EXCL_LINE

            BOOST_TEST(distance < error_tol); // LCOV_EXCL_LINE
        }
        else
        {
            boost::random::uniform_int_distribution<std::uint64_t> dist(std::numeric_limits<std::uint64_t>::min(),
                                                                        std::numeric_limits<std::uint64_t>::max());

            const auto value {dist(rng)}; // LCOV_EXCL_LINE
            builtin_u128 builtin_value;
            builtin_value = value;
            boost::int128::int128_t emulated_value {};
            emulated_value = value;

            const auto builtin_value_return = static_cast<FloatType>(builtin_value);
            const auto emulated_value_return = static_cast<FloatType>(emulated_value);
            FloatType distance = builtin_value_return - emulated_value_return;

            distance = distance < 0 ? -distance : distance; // LCOV_EXCL_LINE

            BOOST_TEST(distance < error_tol); // LCOV_EXCL_LINE
        }
    }
}

template <typename IntType>
void test_unary_plus()
{
    boost::random::uniform_int_distribution<IntType> dist(get_min<IntType>(),
                                                          get_max<IntType>());

    for (std::size_t i {}; i < N; ++i)
    {
        const IntType value {dist(rng)};
        builtin_i128 builtin_value = static_cast<builtin_i128>(value);
        builtin_value = +builtin_value;
        boost::int128::int128_t emulated_value {value};
        emulated_value = +emulated_value;

        builtin_i128 emulated_bits;
        std::memcpy(&emulated_bits, &emulated_value, sizeof(builtin_i128));

        BOOST_TEST(emulated_bits == builtin_value);
    }
}

template <typename IntType>
void test_unary_minus()
{
    boost::random::uniform_int_distribution<IntType> dist(get_min<IntType>(),
                                                          get_max<IntType>());

    for (std::size_t i {}; i < N; ++i)
    {
        const IntType value {dist(rng)};
        auto builtin_value = static_cast<builtin_i128>(value);
        builtin_value = -builtin_value;
        boost::int128::int128_t emulated_value {value};
        emulated_value = -emulated_value;

        builtin_i128 emulated_bits;
        std::memcpy(&emulated_bits, &emulated_value, sizeof(builtin_i128));

        BOOST_TEST(emulated_bits == builtin_value);
    }
}

template <typename IntType>
void test_operator_equality()
{
    boost::random::uniform_int_distribution<IntType> dist(get_min<IntType>(),
                                                          get_max<IntType>());

    // Always equal
    for (std::size_t i {}; i < N; ++i)
    {
        const IntType value {dist(rng)};
        auto builtin_value = static_cast<builtin_i128>(value);
        boost::int128::int128_t emulated_value {value};

        BOOST_TEST(((value == emulated_value) == (emulated_value == value)) ==
                   ((value == builtin_value) == (builtin_value == value)));
    }

    // Potentially equal
    for (std::size_t i {}; i < N; ++i)
    {
        const IntType value {dist(rng)};
        const IntType value2 {dist(rng)};
        auto builtin_value = static_cast<builtin_i128>(value);
        boost::int128::int128_t emulated_value {value};

        BOOST_TEST(((value2 == emulated_value) == (emulated_value == value2)) ==
                   ((value2 == builtin_value) == (builtin_value == value2)));
    }

    // Never equal
    BOOST_INT128_IF_CONSTEXPR (sizeof(IntType) < sizeof(boost::int128::int128_t))
    {
        for (std::size_t i {}; i < N; ++i)
        {
            const IntType value {dist(rng)};
            boost::int128::int128_t emulated_value {1, static_cast<std::uint64_t>(value)};
            BOOST_TEST((value == emulated_value) == (emulated_value == value));
        }
    }

    const boost::int128::int128_t bool_val {dist(rng)};
    BOOST_TEST((true == bool_val) == (bool_val == true));
    const boost::int128::int128_t bool_val2 {static_cast<std::int64_t>(dist(rng)), 0};
    BOOST_TEST((true == bool_val2) == (bool_val2 == true));
    BOOST_TEST(!(bool_val == bool_val2));
    BOOST_TEST(bool_val == bool_val);
    BOOST_TEST(bool_val2 == bool_val2);
}

template <typename IntType>
void test_operator_inequality()
{
    boost::random::uniform_int_distribution<IntType> dist(get_min<IntType>(),
                                                          get_max<IntType>());

    // Always equal
    for (std::size_t i {}; i < N; ++i)
    {
        const IntType value {dist(rng)};
        auto builtin_value = static_cast<builtin_i128>(value);
        boost::int128::int128_t emulated_value {value};

        BOOST_TEST(((value != emulated_value) == (emulated_value != value)) ==
                   ((value != builtin_value) == (builtin_value != value)));
    }

    // Potentially equal
    for (std::size_t i {}; i < N; ++i)
    {
        const IntType value {dist(rng)};
        const IntType value2 {dist(rng)};
        auto builtin_value = static_cast<builtin_i128>(value);
        boost::int128::int128_t emulated_value {value};

        BOOST_TEST(((value2 != emulated_value) == (emulated_value != value2)) ==
                   ((value2 != builtin_value) == (builtin_value != value2)));
    }

    // Never equal
    BOOST_INT128_IF_CONSTEXPR (sizeof(IntType) < sizeof(boost::int128::int128_t))
    {
        for (std::size_t i {}; i < N; ++i)
        {
            const IntType value {dist(rng)};
            boost::int128::int128_t emulated_value {1, static_cast<std::uint64_t>(value)};
            BOOST_TEST((value != emulated_value) == (emulated_value != value));
        }
    }

    const boost::int128::int128_t bool_val {dist(rng)};
    BOOST_TEST((true != bool_val) == (bool_val != true));
    const boost::int128::int128_t bool_val2 {static_cast<std::int64_t>(dist(rng)), 0};
    BOOST_TEST((true != bool_val2) == (bool_val2 != true));
    BOOST_TEST(bool_val != bool_val2);
    BOOST_TEST(!(bool_val != bool_val));
    BOOST_TEST((bool_val != (bool_val+1)));
    BOOST_TEST(((bool_val+1) != bool_val));
    BOOST_TEST(!(bool_val2 != bool_val2));
}

template <typename IntType>
void test_operator_less()
{
    boost::random::uniform_int_distribution<IntType> dist(get_min<IntType>(),
                                                          get_max<IntType>());

    for (std::size_t i {}; i < N; ++i)
    {
        const IntType value {dist(rng)};
        const IntType value2 {dist(rng)};
        auto builtin_value = static_cast<builtin_i128>(value);
        boost::int128::int128_t emulated_value {value};

        BOOST_TEST(((value2 < emulated_value) == (value2 < builtin_value)) ==
                   ((emulated_value < value2) == (builtin_value < value2)));
    }

    BOOST_INT128_IF_CONSTEXPR (sizeof(IntType) < sizeof(boost::int128::int128_t) && std::is_unsigned<IntType>::value)
    {
        for (std::size_t i {}; i < N; ++i)
        {
            const IntType value {dist(rng)};
            boost::int128::int128_t emulated_value {-1, static_cast<std::uint64_t>(value)};
            BOOST_TEST((value < emulated_value) != (emulated_value < value));
        }
    }
}

template <typename IntType>
void test_operator_greater()
{
    boost::random::uniform_int_distribution<IntType> dist(get_min<IntType>(),
                                                          get_max<IntType>());

    for (std::size_t i {}; i < N; ++i)
    {
        const IntType value {dist(rng)};
        const IntType value2 {dist(rng)};
        auto builtin_value = static_cast<builtin_i128>(value);
        boost::int128::int128_t emulated_value {value};

        BOOST_TEST(((value2 > emulated_value) == (value2 > builtin_value)) ==
                   ((emulated_value > value2) == (builtin_value > value2)));
    }

    BOOST_INT128_IF_CONSTEXPR (sizeof(IntType) < sizeof(boost::int128::int128_t) && std::is_unsigned<IntType>::value)
    {
        for (std::size_t i {}; i < N; ++i)
        {
            const IntType value {dist(rng)};
            boost::int128::int128_t emulated_value {-1, static_cast<std::uint64_t>(value)};
            BOOST_TEST((value > emulated_value) != (emulated_value > value));
        }
    }
}

template <typename IntType>
void test_operator_le()
{
    boost::random::uniform_int_distribution<IntType> dist(get_min<IntType>(),
                                                          get_max<IntType>());

    for (std::size_t i {}; i < N; ++i)
    {
        const IntType value {dist(rng)};
        const IntType value2 {dist(rng)};
        auto builtin_value = static_cast<builtin_i128>(value);
        boost::int128::int128_t emulated_value {value};

        BOOST_TEST(((value2 <= emulated_value) == (value2 <= builtin_value)) ==
                   ((emulated_value <= value2) == (builtin_value <= value2)));
    }
}

template <typename IntType>
void test_operator_ge()
{
    boost::random::uniform_int_distribution<IntType> dist(get_min<IntType>(),
                                                          get_max<IntType>());

    for (std::size_t i {}; i < N; ++i)
    {
        const IntType value {dist(rng)};
        const IntType value2 {dist(rng)};
        auto builtin_value = static_cast<builtin_i128>(value);
        boost::int128::int128_t emulated_value {value};

        BOOST_TEST(((value2 >= emulated_value) == (value2 >= builtin_value)) ==
                   ((emulated_value >= value2) == (builtin_value >= value2)));
    }
}

template <typename IntType>
void test_operator_not()
{
    boost::random::uniform_int_distribution<IntType> dist(get_min<IntType>(),
                                                          get_max<IntType>());

    for (std::size_t i {}; i < N; ++i)
    {
        const IntType value {dist(rng)};
        auto builtin_value = static_cast<builtin_i128>(value);
        boost::int128::int128_t emulated_value {value};

        BOOST_TEST(~emulated_value == ~builtin_value);
    }
}

template <typename IntType>
void test_operator_or()
{
    boost::random::uniform_int_distribution<IntType> dist(get_min<IntType>(),
                                                          get_max<IntType>());

    for (std::size_t i {}; i < N; ++i)
    {
        const IntType value {dist(rng)};
        const IntType value2 {dist(rng)};
        auto builtin_value = static_cast<builtin_i128>(value);
        boost::int128::int128_t emulated_value {value};

        auto check_1_value {emulated_value};
        check_1_value |= value2;

        BOOST_INT128_IF_CONSTEXPR (!std::is_same<IntType, builtin_u128>::value)
        {
            BOOST_TEST(check_1_value == (builtin_value | value2));
            BOOST_TEST((value2 | emulated_value) == (value2 | builtin_value));
        }
        else
        {
            builtin_u128 emulated_checkpoint;
            std::memcpy(&emulated_checkpoint, &check_1_value, sizeof(emulated_checkpoint));

            builtin_u128 builtin_checkpoint;
            builtin_value |= value2;
            std::memcpy(&builtin_checkpoint, &builtin_value, sizeof(builtin_checkpoint));

            BOOST_TEST(emulated_checkpoint == builtin_checkpoint);
        }
    }
}

template <typename IntType>
void test_operator_and()
{
    boost::random::uniform_int_distribution<IntType> dist(get_min<IntType>(),
                                                          get_max<IntType>());

    for (std::size_t i {}; i < N; ++i)
    {
        const IntType value {dist(rng)};
        const IntType value2 {dist(rng)};
        auto builtin_value = static_cast<builtin_i128>(value);
        boost::int128::int128_t emulated_value {value};

        auto check_1_value {emulated_value};
        check_1_value &= value2;

        BOOST_INT128_IF_CONSTEXPR (!std::is_same<IntType, builtin_u128>::value)
        {
            BOOST_TEST(check_1_value == (builtin_value & value2));
            BOOST_TEST((value2 & emulated_value) == (value2 & builtin_value));
        }
        else
        {
            builtin_u128 emulated_checkpoint;
            std::memcpy(&emulated_checkpoint, &check_1_value, sizeof(emulated_checkpoint));

            builtin_u128 builtin_checkpoint;
            builtin_value &= value2;
            std::memcpy(&builtin_checkpoint, &builtin_value, sizeof(builtin_checkpoint));

            BOOST_TEST(emulated_checkpoint == builtin_checkpoint);
        }
    }
}

template <typename IntType>
void test_operator_xor()
{
    boost::random::uniform_int_distribution<IntType> dist(get_min<IntType>(),
                                                          get_max<IntType>());

    for (std::size_t i {}; i < N; ++i)
    {
        const IntType value {dist(rng)};
        const IntType value2 {dist(rng)};
        auto builtin_value = static_cast<builtin_i128>(value);
        boost::int128::int128_t emulated_value {value};

        auto check_1_value {emulated_value};
        check_1_value ^= value2;

        BOOST_INT128_IF_CONSTEXPR (!std::is_same<IntType, builtin_u128>::value)
        {
            BOOST_TEST(check_1_value == (builtin_value ^ value2));
            BOOST_TEST((value2 ^ emulated_value) == (value2 ^ builtin_value));
        }
        else
        {
            builtin_u128 emulated_checkpoint;
            std::memcpy(&emulated_checkpoint, &check_1_value, sizeof(emulated_checkpoint));

            builtin_u128 builtin_checkpoint;
            builtin_value ^= value2;
            std::memcpy(&builtin_checkpoint, &builtin_value, sizeof(builtin_checkpoint));

            BOOST_TEST(emulated_checkpoint == builtin_checkpoint);
        }
    }
}

template <typename IntType>
void test_operator_left_shift()
{
    boost::random::uniform_int_distribution<IntType> dist(static_cast<IntType>(0),
                                                          get_max<IntType>());

    boost::random::uniform_int_distribution<unsigned> shift_dist(0, sizeof(IntType) * CHAR_BIT - 1);

    for (std::size_t i {}; i < N; ++i)
    {
        const IntType value {dist(rng)};
        const unsigned shift_value {shift_dist(rng)};
        auto builtin_value = static_cast<builtin_i128>(value);
        boost::int128::int128_t emulated_value {value};

        // Test 1: Test the <<= operator
        auto builtin_copy = builtin_value;
        auto emulated_copy = emulated_value;

        builtin_copy <<= shift_value;
        emulated_copy <<= shift_value;

        BOOST_TEST(emulated_copy == builtin_copy);

        // Test 2: Test the binary << operator
        auto shifted_builtin = builtin_value << shift_value;
        auto shifted_emulated = emulated_value << shift_value;

        BOOST_TEST(shifted_emulated == shifted_builtin);

        // Test 3: Test with IntType as left operand and int128 variants as right operand
        auto int_shift_builtin = value << static_cast<unsigned>(builtin_value);
        auto int_shift_emulated = value << static_cast<unsigned>(emulated_value);

        static_assert(std::is_same<decltype(int_shift_builtin),
                                   decltype(int_shift_emulated)>::value, "Mismatched types");

        BOOST_TEST(int_shift_emulated == int_shift_builtin);
    }

    // Edge cases
    const boost::int128::int128_t val {UINT64_MAX};
    BOOST_TEST((val << 130) == 0);
    BOOST_TEST((val << -5) == 0);
}

template <typename IntType>
void test_operator_right_shift()
{
    boost::random::uniform_int_distribution<IntType> dist(static_cast<IntType>(0),
                                                          get_max<IntType>());

    boost::random::uniform_int_distribution<unsigned> shift_dist(0, sizeof(IntType) * CHAR_BIT - 1);

    for (std::size_t i {}; i < N; ++i)
    {
        const IntType value {dist(rng)};
        const unsigned shift_value {shift_dist(rng)};
        auto builtin_value = static_cast<builtin_i128>(value);
        boost::int128::int128_t emulated_value {value};

        // Test 1: Test the >>= operator
        auto builtin_copy = builtin_value;
        auto emulated_copy = emulated_value;

        builtin_copy >>= shift_value;
        emulated_copy >>= shift_value;

        BOOST_TEST(emulated_copy == builtin_copy);

        // Test 2: Test the binary >> operator
        auto shifted_builtin = builtin_value >> shift_value;
        auto shifted_emulated = emulated_value >> shift_value;

        BOOST_TEST(shifted_emulated == shifted_builtin);

        // Test 3: Test with IntType as left operand and int128 variants as right operand
        auto int_shift_builtin = value >> static_cast<unsigned>(builtin_value);
        auto int_shift_emulated = value >> static_cast<unsigned>(emulated_value);

        static_assert(std::is_same<decltype(int_shift_builtin),
                                   decltype(int_shift_emulated)>::value, "Mismatched types");

        BOOST_TEST(int_shift_emulated == int_shift_builtin);
    }

    // Edge cases
    const boost::int128::int128_t val {UINT64_MAX};
    BOOST_TEST((val >> 130) == 0);
    BOOST_TEST((val >> -5) == 0);
}

void test_increment_operator()
{
    boost::int128::int128_t emulated_value {INT64_MAX - N/2};
    builtin_i128 builtin_value {INT64_MAX - N/2};

    for (std::size_t i {}; i < N; ++i)
    {
        ++emulated_value;
        ++builtin_value;

        BOOST_TEST(emulated_value == builtin_value);
    }
}

template <typename IntType>
void test_operator_add()
{
    boost::random::uniform_int_distribution<IntType> dist(get_min<IntType>(),
                                                          get_max<IntType>());

    for (std::size_t i {}; i < N; ++i)
    {
        IntType value {dist(rng)};
        IntType value2 {dist(rng)};

        // Avoid UB from signed rollover
        BOOST_INT128_IF_CONSTEXPR (std::is_same<IntType, builtin_u128>::value || std::is_same<IntType, builtin_i128>::value)
        {
            value /= 100;
            value2 /= 100;
        }

        auto builtin_value = static_cast<builtin_i128>(value);
        boost::int128::int128_t emulated_value {value};

        auto check_1_value {emulated_value};
        check_1_value += value2;
        BOOST_TEST(check_1_value == (builtin_value + value2));
        BOOST_TEST((value2 + emulated_value) == (value2 + builtin_value));
    }

    // Edge case where we go from low word to high word
    builtin_i128 builtin_value = UINT64_MAX - 2U;
    boost::int128::int128_t emulated_value = UINT64_MAX - 2U;

    for (std::size_t i {}; i < N; ++i)
    {
        BOOST_TEST(++builtin_value == ++emulated_value);
    }
}

template <typename IntType>
void test_operator_sub()
{
    boost::random::uniform_int_distribution<IntType> dist(get_min<IntType>(),
                                                          get_max<IntType>());

    for (std::size_t i {}; i < N; ++i)
    {
        IntType value {dist(rng)};
        IntType value2 {dist(rng)};

        // Avoid UB from signed rollover / unrepresentable values
        BOOST_INT128_IF_CONSTEXPR (std::is_same<IntType, builtin_u128>::value || std::is_same<IntType, builtin_i128>::value)
        {
            value /= 100;
            value2 /= 100;
        }

        auto builtin_value = static_cast<builtin_i128>(value);
        boost::int128::int128_t emulated_value {value};

        // Avoid UB from signed rollover
        if (value > value2)
        {
            auto check_1_value {emulated_value};
            check_1_value -= value2;
            BOOST_TEST(check_1_value == (builtin_value - value2));
        }
        else
        {
            BOOST_TEST((value2 - emulated_value) == (value2 - builtin_value));
        }
    }

    // Edge case where we go from high word to low word
    builtin_i128 builtin_value = UINT64_MAX;
    builtin_value += 10;
    boost::int128::int128_t emulated_value = UINT64_MAX;
    emulated_value += 10;

    for (std::size_t i {}; i < N; ++i)
    {
        BOOST_TEST(--builtin_value == --emulated_value);
    }
}

template <typename IntType>
void test_operator_mul()
{
    boost::random::uniform_int_distribution<IntType> dist(get_root_min<IntType>(),
                                                          get_root_max<IntType>());

    for (std::size_t i {}; i < N; ++i)
    {
        IntType value {dist(rng)};
        IntType value2 {dist(rng)};

        auto builtin_value = static_cast<builtin_i128>(value);
        boost::int128::int128_t emulated_value {value};

        auto check_1_value {emulated_value};
        check_1_value *= value2;
        BOOST_TEST(check_1_value == (builtin_value * value2));
        BOOST_TEST((value2 * emulated_value) == (value2 * builtin_value));
    }
}

template <typename IntType>
void test_operator_div()
{
    boost::random::uniform_int_distribution<IntType> dist(get_min<IntType>(),
        get_max<IntType>());
    
    // I am not entirely sure what the MSVC int128 is doing here for rollover situations

    #ifdef _WIN32

    BOOST_INT128_IF_CONSTEXPR(std::is_same<IntType, boost::int128::detail::builtin_u128>::value)
    {
        return;
    }

    #endif

    for (std::size_t i {}; i < N; ++i)
    {
        IntType value {dist(rng)};
        IntType value2 {dist(rng)};

        // Avoid UB of div by 0
        while (value == 0)
        {
            value = dist(rng); // LCOV_EXCL_LINE
        }
        while (value2 == 0)
        {
            value2 = dist(rng); // LCOV_EXCL_LINE
        }

        auto builtin_value = static_cast<builtin_i128>(value);
        boost::int128::int128_t emulated_value {value};

        auto check_1_value {emulated_value};
        check_1_value /= value2;

        static_assert(sizeof(decltype(emulated_value / value2)) ==
                      sizeof(decltype(builtin_value / value2)), "Mismatch Return Types");

        static_assert(sizeof(decltype(value2 / emulated_value)) ==
                      sizeof(decltype(value2 / builtin_value)), "Mismatch Return Types");

        BOOST_TEST(check_1_value == (builtin_value / value2));
        BOOST_TEST((value2 / emulated_value) == (value2 / builtin_value));
    }

    // Test 2 word by 1 word and 1 word by 2 word
    BOOST_INT128_IF_CONSTEXPR (sizeof(IntType) < sizeof(boost::int128::uint128_t) && sizeof(IntType) > sizeof(char))
    {
        for (std::size_t i {}; i < N; ++i)
        {
            IntType value {dist(rng)};
            IntType value2 {dist(rng)};

            // Avoid UB of div by 0
            while (value == 0)
            {
                value = dist(rng); // LCOV_EXCL_LINE
            }
            while (value2 == 0)
            {
                value2 = dist(rng); // LCOV_EXCL_LINE
            }

            const boost::int128::int128_t emulated_value {static_cast<std::int64_t>(value), static_cast<std::uint64_t>(value)};
            const auto builtin_value = static_cast<builtin_i128>(emulated_value);
            const boost::int128::int128_t small_emulated_value {0, static_cast<std::uint64_t>(value)};

            auto check_1_value {emulated_value};
            check_1_value /= value2;

            static_assert(sizeof(decltype(emulated_value / value2)) ==
                          sizeof(decltype(builtin_value / value2)), "Mismatch Return Types");

            static_assert(sizeof(decltype(value2 / emulated_value)) ==
                          sizeof(decltype(value2 / builtin_value)), "Mismatch Return Types");

            BOOST_TEST(check_1_value == (builtin_value / value2));
            BOOST_TEST((value2 / emulated_value) == (value2 / builtin_value));

            // Forces decision process
            const boost::int128::int128_t check_2_value {value2};
            BOOST_TEST(check_1_value == (emulated_value / check_2_value));

            // Shouldn't crash
            BOOST_TEST(check_2_value / IntType(0) == 0);
            BOOST_TEST(value / static_cast<boost::int128::int128_t>(0) == 0);

            // Always 0
            BOOST_TEST(small_emulated_value / emulated_value == 0);
            BOOST_TEST(small_emulated_value / small_emulated_value == 1);
        }
    }
}

template <typename IntType>
void test_operator_mod()
{
    boost::random::uniform_int_distribution<IntType> dist(get_min<IntType>(),
        get_max<IntType>());

    // I am not entirely sure what the MSVC int128 is doing here for rollover situations

    #ifdef _WIN32

    BOOST_INT128_IF_CONSTEXPR(std::is_same<IntType, boost::int128::detail::builtin_u128>::value)
    {
        return;
    }

    #endif

    for (std::size_t i {}; i < N; ++i)
    {
        IntType value {dist(rng)};
        IntType value2 {dist(rng)};

        // Avoid UB of div by 0
        while (value == 0)
        {
            value = dist(rng);
        }
        while (value2 == 0)
        {
            value2 = dist(rng);
        }

        // With too large an unsigned __int128 value we end up with bitwise equal but signed results
        BOOST_INT128_IF_CONSTEXPR (sizeof(IntType) == sizeof(boost::int128::int128_t))
        {
            value >>= 2;
            value2 >>= 2;
        }

        auto builtin_value = static_cast<builtin_i128>(value);
        boost::int128::int128_t emulated_value {value};

        auto check_1_value {emulated_value};
        check_1_value %= value2;

        static_assert(sizeof(decltype(emulated_value % value2)) ==
                      sizeof(decltype(builtin_value % value2)), "Mismatch Return Types");

        static_assert(sizeof(decltype(value2 % emulated_value)) ==
                      sizeof(decltype(value2 % builtin_value)), "Mismatch Return Types");

        BOOST_TEST(check_1_value == (builtin_value % value2));
        BOOST_TEST((value2 % emulated_value) == (value2 % builtin_value));
    }

    // Test 2 word by 1 word and 1 word by 2 word
    BOOST_INT128_IF_CONSTEXPR (sizeof(IntType) < sizeof(boost::int128::uint128_t) && sizeof(IntType) > sizeof(char))
    {
        for (std::size_t i {}; i < N; ++i)
        {
            IntType value {dist(rng)};
            IntType value2 {dist(rng)};

            // Avoid UB of div by 0
            while (value == 0)
            {
                value = dist(rng); // LCOV_EXCL_LINE
            }
            while (value2 == 0)
            {
                value2 = dist(rng); // LCOV_EXCL_LINE
            }

            const boost::int128::int128_t emulated_value {static_cast<std::int64_t>(value), static_cast<std::uint64_t>(value)};
            const auto builtin_value = static_cast<builtin_i128>(emulated_value);
            const boost::int128::int128_t small_emulated_value {0, static_cast<std::uint64_t>(value)};

            auto check_1_value {emulated_value};
            check_1_value %= value2;

            static_assert(sizeof(decltype(emulated_value % value2)) ==
                          sizeof(decltype(builtin_value % value2)), "Mismatch Return Types");

            static_assert(sizeof(decltype(value2 % emulated_value)) ==
                          sizeof(decltype(value2 % builtin_value)), "Mismatch Return Types");

            BOOST_TEST(check_1_value == (builtin_value % value2));
            BOOST_TEST((value2 % emulated_value) == (value2 % builtin_value));

            // Forces decision process
            const boost::int128::int128_t check_2_value {value2};
            BOOST_TEST(check_1_value == (emulated_value % check_2_value));

            // Shouldn't crash
            // Codecov has these marked as partials which is as long as they pass is irrelevant 
            BOOST_TEST(check_2_value % IntType(0) == 0);                                        // LCOV_EXCL_LINE
            BOOST_TEST(value % static_cast<boost::int128::int128_t>(0) == 0);                   // LCOV_EXCL_LINE
            BOOST_TEST(small_emulated_value % static_cast<boost::int128::int128_t>(0) == 0);    // LCOV_EXCL_LINE
            BOOST_TEST(small_emulated_value % emulated_value == small_emulated_value);          // LCOV_EXCL_LINE
            BOOST_TEST(small_emulated_value % small_emulated_value == 0);                       // LCOV_EXCL_LINE
        }
    }
}

template <typename IntType, std::enable_if_t<boost::int128::detail::is_signed_integer_v<IntType>, bool> = true>
void test_abs()
{
    using boost::int128::abs;

    boost::random::uniform_int_distribution<IntType> dist(get_min<IntType>(),
                                                          get_max<IntType>());

    for (std::size_t i {}; i < N; ++i)
    {
        const auto value {dist(rng)};
        boost::int128::int128_t emulated_value {value};
        auto builtin_value = static_cast<builtin_i128>(value);

        if (value < 0)
        {
            BOOST_TEST(abs(emulated_value) == -emulated_value);
            BOOST_TEST(abs(emulated_value) == -builtin_value);
        }
        else
        {
            BOOST_TEST(abs(emulated_value) == emulated_value);
            BOOST_TEST(abs(emulated_value) == builtin_value);
        }
    }
}

template <typename IntType, std::enable_if_t<!boost::int128::detail::is_signed_integer_v<IntType>, bool> = true>
void test_abs()
{
    boost::random::uniform_int_distribution<IntType> dist(get_min<IntType>(),
                                                          get_max<IntType>());

    for (std::size_t i {}; i < N; ++i)
    {
        auto value {dist(rng)};

        // Avoid signed rollover UB
        BOOST_INT128_IF_CONSTEXPR (std::is_same<IntType, builtin_u128>::value)
        {
            value /= 100;
        }

        boost::int128::int128_t emulated_value {value};
        auto builtin_value = static_cast<builtin_i128>(value);

        BOOST_TEST(abs(emulated_value) == emulated_value);
        BOOST_TEST(abs(emulated_value) == builtin_value);
    }
}

template <typename IntType>
void test_spot_mod(const IntType value, const IntType value2)
{
    auto builtin_value = static_cast<builtin_i128>(value);
    boost::int128::int128_t emulated_value{value};

    auto check_1_value{emulated_value};
    check_1_value %= value2;

    static_assert(sizeof(decltype(emulated_value % value2)) ==
        sizeof(decltype(builtin_value % value2)), "Mismatch Return Types");

    static_assert(sizeof(decltype(value2 % emulated_value)) ==
        sizeof(decltype(value2 % builtin_value)), "Mismatch Return Types");

    BOOST_TEST(check_1_value == (builtin_value % value2));
    BOOST_TEST((value2 % emulated_value) == (value2 % builtin_value));
}

struct test_caller
{
    template<typename T>
    void operator()(T) const
    {
        test_arithmetic_constructor<T>();
        test_assignment_operators<T>();
        test_integer_conversion_operators<T>();

        test_unary_plus<T>();
        test_unary_minus<T>();
        
        test_operator_equality<T>();
        test_operator_inequality<T>();
        test_operator_less<T>();
        test_operator_greater<T>();
        test_operator_le<T>();
        test_operator_ge<T>();
        
        test_operator_not<T>();
        test_operator_or<T>();
        test_operator_and<T>();
        test_operator_xor<T>();

        #if !defined(UBSAN) && !defined(ASAN) && !defined(__SANITIZE_ADDRESS__)
        test_operator_left_shift<T>();
        test_operator_right_shift<T>();
        #endif

        test_increment_operator();
        test_operator_add<T>();
        test_operator_sub<T>();
        test_operator_mul<T>();
        // See: https://github.com/cppalliance/int128/issues/147#issuecomment-2936929238
        #if !defined(_M_IX86) || (defined(_M_IX86) && defined(_DEBUG))
        test_operator_div<T>();
        test_operator_mod<T>();
        #endif

        test_abs<T>();
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
        #ifndef BOOST_INT128_HAS_MSVC_INT128 // MSVC std::_Signed128 is not compatible with Boost.Random
        builtin_i128,
        #endif // BOOST_INT128_HAS_MSVC_INT128
        builtin_u128
    >;

    boost::mp11::mp_for_each<test_types>(test_caller());

    // MSVC std::_Signed128 does not provide a float conversion operator
    #ifndef BOOST_INT128_HAS_MSVC_INT128

    test_float_conversion_operators<float>();
    test_float_conversion_operators<double>();
    test_float_conversion_operators<long double>();

    #endif // BOOST_INT128_HAS_MSVC_INT128

    // See: https://github.com/cppalliance/int128/issues/147#issuecomment-2936929238
    #if !defined(_M_IX86) || (defined(_M_IX86) && defined(_DEBUG))
    // lhs % rhs == -880554185798178108
    // rhs % lhs == -1184271995001643447
    test_spot_mod(INT64_C(-7986186155808038790), INT64_C(-1184271995001643447));

    // lhs % rhs == 15
    test_spot_mod(static_cast<short>(81), static_cast<short>(-22));

    // lhs % rhs == 1194843042943565928
    test_spot_mod(INT64_C(6627510689879126116), INT64_C(-1358166911733890047));

    // lhs % rhs == 45765284099668012
    test_spot_mod(INT64_C(3120322666916965645), INT64_C(-1024852460939099211));
    #endif

    return boost::report_errors();
}

#else

int main()
{
    static_cast<void>(N);
    return 0;
}

#endif
