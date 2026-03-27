// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
//
// See: https://github.com/cppalliance/int128/issues/377

#include <boost/int128.hpp>
#include <boost/core/lightweight_test.hpp>
#include <limits>
#include <cstdint>

using namespace boost::int128;

template <typename T>
void test_div_by_one()
{
    constexpr auto min_val {std::numeric_limits<int128_t>::min()};
    BOOST_TEST_EQ(min_val, min_val / T{1});
}

template <typename T>
void test_other_vals()
{
    constexpr auto min_val {std::numeric_limits<int128_t>::min()};
    constexpr auto min_div_2 {BOOST_INT128_INT128_C(-85070591730234615865843651857942052864)};
    constexpr auto min_div_4 {BOOST_INT128_INT128_C(-42535295865117307932921825928971026432)};
    constexpr auto min_div_16 {BOOST_INT128_INT128_C(-10633823966279326983230456482242756608)};

    BOOST_TEST_EQ(min_div_2, min_val / T{2});
    BOOST_TEST_EQ(min_div_4, min_val / T{4});
    BOOST_TEST_EQ(min_div_16, min_val / T{16});
}

int main()
{
    test_div_by_one<std::int32_t>();
    test_div_by_one<std::int64_t>();
    test_div_by_one<int128_t>();

    test_other_vals<std::int32_t>();
    test_other_vals<std::int64_t>();
    test_other_vals<int128_t>();

    return boost::report_errors();
}
