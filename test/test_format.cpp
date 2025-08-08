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

int main()
{
    test_empty<boost::int128::uint128_t>();
    test_empty<boost::int128::int128_t>();

    return boost::report_errors();
}

#else

int main()
{
    return 0;
}

#endif
