// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128/int128.hpp>
#include <boost/int128/literals.hpp>
#include <boost/int128/iostream.hpp>
#include <boost/core/lightweight_test.hpp>
#include <limits>

using namespace boost::int128::literals;

void test_u128_literals()
{
    BOOST_TEST(boost::int128::uint128_t{0} == 0_u128);
    BOOST_TEST(boost::int128::uint128_t{10} == "10"_U128);
    BOOST_TEST(boost::int128::uint128_t{0} == BOOST_INT128_UINT128_C(0));

    const boost::int128::uint128_t max_val {std::numeric_limits<boost::int128::uint128_t>::max()};
    const auto macro_val {BOOST_INT128_UINT128_C(340282366920938463463374607431768211455)};
    BOOST_TEST(max_val == macro_val);
}

void test_i128_literals()
{
    BOOST_TEST(boost::int128::int128_t{0} == 0_i128);
    BOOST_TEST(boost::int128::int128_t{10} == "10"_I128);
    BOOST_TEST(boost::int128::int128_t{0} == BOOST_INT128_INT128_C(0));

    const boost::int128::int128_t max_val {std::numeric_limits<boost::int128::int128_t>::max()};
    const auto macro_val {BOOST_INT128_INT128_C(170141183460469231731687303715884105727)};
    BOOST_TEST(max_val == macro_val);

    const boost::int128::int128_t min_val {std::numeric_limits<boost::int128::int128_t>::min()};
    const auto min_macro_val {BOOST_INT128_INT128_C(-170141183460469231731687303715884105728)};
    BOOST_TEST(min_val == min_macro_val);
}

int main()
{
    test_u128_literals();
    test_i128_literals();

    return boost::report_errors();
}
