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

int main()
{
    test<uint128_t>();
    test<int128_t>();

    return boost::report_errors();
}
