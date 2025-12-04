// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128.hpp>
#include <boost/int128/format.hpp>
#include <boost/core/lightweight_test.hpp>

#ifdef BOOST_INT128_HAS_FORMAT

template <typename T>
void test_empty()
{
    BOOST_TEST_CSTR_EQ(std::format("{}", T{2}).c_str(), "2");
    BOOST_TEST_CSTR_EQ(std::format("{}", T{22}).c_str(), "22");
    BOOST_TEST_CSTR_EQ(std::format("{}", T{222}).c_str(), "222");
    BOOST_TEST_CSTR_EQ(std::format("{}", T{2222}).c_str(), "2222");
    BOOST_TEST_CSTR_EQ(std::format("{}", T{22222}).c_str(), "22222");
    BOOST_TEST_CSTR_EQ(std::format("{}", T{222222}).c_str(), "222222");

    if constexpr (std::is_same_v<T, boost::int128::int128_t>)
    {
        BOOST_TEST_CSTR_EQ(std::format("{}", T{-2}).c_str(), "-2");
        BOOST_TEST_CSTR_EQ(std::format("{}", T{-22}).c_str(), "-22");
        BOOST_TEST_CSTR_EQ(std::format("{}", T{-222}).c_str(), "-222");
        BOOST_TEST_CSTR_EQ(std::format("{}", T{-2222}).c_str(), "-2222");
        BOOST_TEST_CSTR_EQ(std::format("{}", T{-22222}).c_str(), "-22222");
        BOOST_TEST_CSTR_EQ(std::format("{}", T{-222222}).c_str(), "-222222");
    }
}

template <typename T>
void test_binary()
{
    BOOST_TEST_CSTR_EQ(std::format("{:b}", T{2}).c_str(), "10");
    BOOST_TEST_CSTR_EQ(std::format("{:#b}", T{2}).c_str(), "0b10");
    BOOST_TEST_CSTR_EQ(std::format("{:#B}", T{2}).c_str(), "0B10");

    BOOST_TEST_CSTR_EQ(std::format("{:b}", T{5}).c_str(), "101");
    BOOST_TEST_CSTR_EQ(std::format("{:#b}", T{5}).c_str(), "0b101");
    BOOST_TEST_CSTR_EQ(std::format("{:#B}", T{5}).c_str(), "0B101");

    BOOST_TEST_CSTR_EQ(std::format("{:6b}", T{5}).c_str(), "000101");
    BOOST_TEST_CSTR_EQ(std::format("{:#6b}", T{5}).c_str(), "0b000101");
    BOOST_TEST_CSTR_EQ(std::format("{:#06B}", T{5}).c_str(), "0B000101");
}

template <typename T>
void test_octal()
{
    BOOST_TEST_CSTR_EQ(std::format("{:o}", T{42}).c_str(), "52");
    BOOST_TEST_CSTR_EQ(std::format("{:#o}", T{42}).c_str(), "052");

    BOOST_TEST_CSTR_EQ(std::format("{:4o}", T{42}).c_str(), "0052");
    BOOST_TEST_CSTR_EQ(std::format("{:#4o}", T{42}).c_str(), "00052");
}

template <typename T>
void test_decimal()
{
    BOOST_TEST_CSTR_EQ(std::format("{:d}", T{42}).c_str(), "42");
    BOOST_TEST_CSTR_EQ(std::format("{:#d}", T{42}).c_str(), "42");

    BOOST_TEST_CSTR_EQ(std::format("{:+d}", T{42}).c_str(), "+42");
    BOOST_TEST_CSTR_EQ(std::format("{:+#d}", T{42}).c_str(), "+42");

    BOOST_TEST_CSTR_EQ(std::format("{: d}", T{42}).c_str(), " 42");
    BOOST_TEST_CSTR_EQ(std::format("{: #d}", T{42}).c_str(), " 42");

    BOOST_TEST_CSTR_EQ(std::format("{:+3d}", T{42}).c_str(), "+042");
    BOOST_TEST_CSTR_EQ(std::format("{:+#3d}", T{42}).c_str(), "+042");

    BOOST_TEST_CSTR_EQ(std::format("{:-3d}", T{42}).c_str(), "042");
    BOOST_TEST_CSTR_EQ(std::format("{:-#3d}", T{42}).c_str(), "042");

    if constexpr (std::is_same_v<T, boost::int128::int128_t>)
    {
        BOOST_TEST_CSTR_EQ(std::format("{: 3d}", T{42}).c_str(), " 042");
        BOOST_TEST_CSTR_EQ(std::format("{: #3d}", T{42}).c_str(), " 042");

        BOOST_TEST_CSTR_EQ(std::format("{:-3d}", T{-42}).c_str(), "-042");
        BOOST_TEST_CSTR_EQ(std::format("{:-#3d}", T{-42}).c_str(), "-042");

        BOOST_TEST_CSTR_EQ(std::format("{: 3d}", T{-42}).c_str(), "-042");
        BOOST_TEST_CSTR_EQ(std::format("{: #3d}", T{-42}).c_str(), "-042");
    }

    BOOST_TEST_CSTR_EQ(std::format("{:+d}", T{42}).c_str(), "+42");
    BOOST_TEST_CSTR_EQ(std::format("{:+#?d}", T{42}).c_str(), "+42");
    BOOST_TEST_CSTR_EQ(std::format("{:+#c}", T{42}).c_str(), "+42");
}

template <typename T>
void test_hex()
{
    BOOST_TEST_CSTR_EQ(std::format("{:x}", T{42}).c_str(), "2a");
    BOOST_TEST_CSTR_EQ(std::format("{:#x}", T{42}).c_str(), "0x2a");

    BOOST_TEST_CSTR_EQ(std::format("{:X}", T{42}).c_str(), "2A");
    BOOST_TEST_CSTR_EQ(std::format("{:#X}", T{42}).c_str(), "0X2A");

    BOOST_TEST_CSTR_EQ(std::format("{: X}", T{42}).c_str(), " 2A");
    BOOST_TEST_CSTR_EQ(std::format("{: #X}", T{42}).c_str(), " 0X2A");

    BOOST_TEST_CSTR_EQ(std::format("{:+X}", T{42}).c_str(), "+2A");
    BOOST_TEST_CSTR_EQ(std::format("{:+#X}", T{42}).c_str(), "+0X2A");

    if constexpr (std::is_same_v<T, boost::int128::int128_t>)
    {
        BOOST_TEST_CSTR_EQ(std::format("{:-X}", T{-42}).c_str(), "-2A");
        BOOST_TEST_CSTR_EQ(std::format("{:-#X}", T{-42}).c_str(), "-0X2A");
    }

    BOOST_TEST_CSTR_EQ(std::format("{:5X}", T{42}).c_str(), "0002A");
    BOOST_TEST_CSTR_EQ(std::format("{:#5X}", T{42}).c_str(), "0X0002A");

    BOOST_TEST_CSTR_EQ(std::format("{: 5X}", T{42}).c_str(), " 0002A");
    BOOST_TEST_CSTR_EQ(std::format("{: #5X}", T{42}).c_str(), " 0X0002A");
}

template <typename T>
void test_string_insertion()
{
    BOOST_TEST_CSTR_EQ(std::format("Height is: {:d}", T {0}).c_str(), "Height is: 0");
    BOOST_TEST_CSTR_EQ(std::format("Height is: {}", T {2}).c_str(), "Height is: 2");

    BOOST_TEST_CSTR_EQ(std::format("Height is: {:d} meters", T {0}).c_str(), "Height is: 0 meters");
    BOOST_TEST_CSTR_EQ(std::format("Height is: {} meters", T {2}).c_str(), "Height is: 2 meters");
}

int main()
{
    test_empty<boost::int128::uint128_t>();
    test_empty<boost::int128::int128_t>();

    test_binary<boost::int128::uint128_t>();
    test_binary<boost::int128::int128_t>();

    test_octal<boost::int128::uint128_t>();
    test_octal<boost::int128::int128_t>();

    test_decimal<boost::int128::uint128_t>();
    test_decimal<boost::int128::int128_t>();

    test_hex<boost::int128::uint128_t>();
    test_hex<boost::int128::int128_t>();

    test_string_insertion<boost::int128::uint128_t>();
    test_string_insertion<boost::int128::int128_t>();

    return boost::report_errors();
}

#else

int main()
{
    return 0;
}

#endif
