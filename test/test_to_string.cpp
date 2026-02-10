// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128.hpp>
#include <boost/int128/charconv.hpp>
#include <boost/charconv.hpp>
#include <boost/core/lightweight_test.hpp>
#include <string>
#include <limits>
#include <array>
#include <cstdint>

using namespace boost::int128;

template <typename T>
void test()
{
    constexpr T min {std::numeric_limits<T>::min()};
    constexpr T max {std::numeric_limits<T>::max()};
    constexpr std::array<T, 4> values = {{ min, max, T{0}, T{42} }};

    for (const auto value : values)
    {
        char buffer[64];
        const auto r {boost::charconv::to_chars(buffer, buffer + sizeof(buffer), value, 10)};
        BOOST_TEST(r);
        *r.ptr = '\0';

        const auto value_str {to_string(value)};

        BOOST_TEST_CSTR_EQ(buffer, value_str.c_str());
    }
}

template <typename T, std::enable_if_t<std::is_signed<T>::value, bool> = true>
void test_builtin()
{
    constexpr T min {std::numeric_limits<T>::min()};
    constexpr T max {std::numeric_limits<T>::max()};
    constexpr std::array<T, 4> values = {{ min, max, T{0}, T{42} }};

    for (const auto value : values)
    {
        char buffer[64];
        const auto r {boost::charconv::to_chars(buffer, buffer + sizeof(buffer), value, 10)};
        BOOST_TEST(r);
        *r.ptr = '\0';

        const auto value_str {to_string(int128_t{value})};

        BOOST_TEST_CSTR_EQ(buffer, value_str.c_str());
        BOOST_TEST_CSTR_EQ(std::to_string(value).c_str(), value_str.c_str());
    }
}

template <typename T, std::enable_if_t<std::is_unsigned<T>::value, bool> = true>
void test_builtin()
{
    constexpr T min {std::numeric_limits<T>::min()};
    constexpr T max {std::numeric_limits<T>::max()};
    constexpr std::array<T, 4> values = {{ min, max, T{0}, T{42} }};

    for (const auto value : values)
    {
        char buffer[64];
        const auto r {boost::charconv::to_chars(buffer, buffer + sizeof(buffer), value, 10)};
        BOOST_TEST(r);
        *r.ptr = '\0';

        {
            const auto value_str {to_string(int128_t{value})};

            BOOST_TEST_CSTR_EQ(buffer, value_str.c_str());
            BOOST_TEST_CSTR_EQ(std::to_string(value).c_str(), value_str.c_str());
        }
        {
            const auto value_str {to_string(uint128_t{value})};

            BOOST_TEST_CSTR_EQ(buffer, value_str.c_str());
            BOOST_TEST_CSTR_EQ(std::to_string(value).c_str(), value_str.c_str());
        }
    }
}

int main()
{
    test<uint128_t>();
    test<int128_t>();

    test_builtin<std::int8_t>();
    test_builtin<std::uint8_t>();
    test_builtin<std::int16_t>();
    test_builtin<std::uint16_t>();
    test_builtin<std::int32_t>();
    test_builtin<std::uint32_t>();
    test_builtin<std::int64_t>();
    test_builtin<std::uint64_t>();

    return boost::report_errors();
}
