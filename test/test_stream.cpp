// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128.hpp>
#include <boost/core/lightweight_test.hpp>
#include <iostream>
#include <iomanip>
#include <sstream>

#if defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 4127) // conditional expression is constant pre-C++17
#endif

template <typename T>
void test_istream()
{
    T val;
    std::stringstream in;
    in.str("42");
    in >> val;
    BOOST_TEST_EQ(val, static_cast<T>(42));

    // Hex value
    T hex_val;
    std::stringstream in_hex;
    in_hex.flags(std::ios_base::hex);
    in_hex.str("0xffff");
    in_hex >> hex_val;
    BOOST_TEST_EQ(hex_val, static_cast<T>(0xFFFF));

    T hex_val_2;
    std::stringstream in_hex_2;
    in_hex_2.flags(std::ios_base::hex);
    in_hex_2.str("FFFFFFFFFFFF");
    in_hex_2 >> hex_val_2;
    BOOST_TEST_EQ(hex_val_2, static_cast<T>(0xFFFFFFFFFFFF));

    // Octal Value
    T oct_val;
    std::stringstream in_oct;
    in_oct.flags(std::ios_base::oct);
    in_oct.str("01111");
    in_oct >> oct_val;
    BOOST_TEST_EQ(oct_val, static_cast<T>(01111));

    T oct_val_2;
    std::stringstream in_oct_2;
    in_oct_2.flags(std::ios_base::oct);
    in_oct_2.str("42");
    in_oct_2 >> oct_val_2;
    BOOST_TEST_EQ(oct_val_2, static_cast<T>(042));
}

template <typename T>
void test_ostream()
{
    T val {42};
    std::stringstream out;
    out << val;
    BOOST_TEST_CSTR_EQ(out.str().c_str(), "42");

    T hex_val {0xFF};
    std::stringstream hex_out;
    hex_out.flags(std::ios_base::hex);
    hex_out << hex_val;
    BOOST_TEST_CSTR_EQ(hex_out.str().c_str(), "0xff");

    std::stringstream hex_out_upper;
    hex_out_upper.flags(std::ios_base::hex | std::ios_base::uppercase);
    hex_out_upper << hex_val;
    BOOST_TEST_CSTR_EQ(hex_out_upper.str().c_str(), "0XFF");

    T octal_val {04};
    std::stringstream octal_out;
    octal_out.flags(std::ios_base::oct);
    octal_out << octal_val;
    BOOST_TEST_CSTR_EQ(octal_out.str().c_str(), "04");

    BOOST_INT128_IF_CONSTEXPR (std::is_same<T, boost::int128::uint128_t>::value)
    {
        boost::int128::uint128_t max_val {std::numeric_limits<boost::int128::uint128_t>::max()};
        std::stringstream max_out;
        max_out << max_val;
        BOOST_TEST_CSTR_EQ(max_out.str().c_str(), "340282366920938463463374607431768211455");
    }
    else
    {
        boost::int128::int128_t max_val {std::numeric_limits<boost::int128::int128_t>::max()};
        std::stringstream max_out;
        max_out << max_val;
        BOOST_TEST_CSTR_EQ(max_out.str().c_str(), "170141183460469231731687303715884105727");

        boost::int128::int128_t min_val {std::numeric_limits<boost::int128::int128_t>::min()};
        std::stringstream min_out;
        min_out << min_val;
        BOOST_TEST_CSTR_EQ(min_out.str().c_str(), "-170141183460469231731687303715884105728");

        boost::int128::int128_t regular_val {-4500};
        std::stringstream reg_out;
        reg_out << regular_val;
        BOOST_TEST_CSTR_EQ(reg_out.str().c_str(), "-4500");
    }
}

void test_error_values()
{
    using boost::int128::uint128_t;
    using boost::int128::int128_t;

    uint128_t val;
    std::stringstream in;
    in.str("-42");
    in >> val;
    BOOST_TEST_EQ(val, 0U);

    uint128_t val2;
    std::stringstream in2;
    in2.str("+42");
    in2 >> val2;
    BOOST_TEST_EQ(val2, 0U);

    uint128_t val3;
    std::stringstream in3;
    in3.str("");
    in3 >> val3;
    BOOST_TEST_EQ(val3, 0U);

    int128_t val4;
    std::stringstream in4;
    in4.str("-");
    in4 >> val4;
    BOOST_TEST_EQ(val4, 0);

    uint128_t val5;
    std::stringstream in5;
    in5.str("F");
    in5 >> val5;
    BOOST_TEST_EQ(val5, 0U);

    uint128_t val6;
    std::stringstream in6;
    in6.str("3F");
    in6 >> val6;
    BOOST_TEST_EQ(val6, 3U);

    uint128_t val7;
    std::stringstream in7;
    in7.str("100000000000000000000000000000000000000000000000000000000000000000000000000000000000");
    in7 >> val7;
    BOOST_TEST_EQ(val7, 0U);

    boost::int128::detail::from_chars(nullptr, nullptr, val7);
    BOOST_TEST_EQ(val7, 0U);
}

#if defined(_MSC_VER)
#  pragma warning(pop)
#endif

int main()
{
    test_istream<boost::int128::uint128_t>();
    test_istream<boost::int128::int128_t>();

    test_ostream<boost::int128::uint128_t>();
    test_ostream<boost::int128::int128_t>();

    test_error_values();

    return boost::report_errors();
}
