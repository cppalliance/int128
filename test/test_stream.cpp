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
}

template <typename T>
void test_ostream()
{
    T val {42};
    std::stringstream out;
    out << val;
    BOOST_TEST_CSTR_EQ(out.str().c_str(), "42");

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
    }
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

    return boost::report_errors();
}
