// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128.hpp>
#include <boost/core/lightweight_test.hpp>
#include <numeric>

template <typename T>
void test_trivial()
{
    using boost::int128::midpoint;

    BOOST_TEST_EQ(T{3}, midpoint(T{2}, T{4}));
    BOOST_TEST_EQ(T{4}, midpoint(T{2}, T{6}));
}

template <typename T>
void test_rounding()
{
    using boost::int128::midpoint;

    BOOST_TEST_EQ(T{4}, midpoint(T{4}, T{3}));
    BOOST_TEST_EQ(T{3}, midpoint(T{2}, T{5}));
    BOOST_TEST_EQ(T{4}, midpoint(T{5}, T{2}));
}

template <typename T>
void test_big();

template <>
void test_big<boost::int128::uint128_t>()
{
    using boost::int128::uint128_t;
    using boost::int128::midpoint;

    constexpr auto max = std::numeric_limits<uint128_t>::max();

    BOOST_TEST_EQ(BOOST_INT128_UINT128_C(170141183460469231731687303715884105728), midpoint(max, std::numeric_limits<uint128_t>::min()));
    BOOST_TEST_EQ(max - 1u, midpoint(max, max - 2u));
}

template <>
void test_big<boost::int128::int128_t>()
{
    using boost::int128::int128_t;
    using boost::int128::midpoint;

    constexpr auto max = std::numeric_limits<int128_t>::max();
    constexpr auto min = std::numeric_limits<int128_t>::min();

    BOOST_TEST_EQ(BOOST_INT128_INT128_C(-170141183460469231731687303715884105728), midpoint(max, min));

    BOOST_TEST_EQ(max - 1, midpoint(max, max - 2));
    BOOST_TEST_EQ(min, midpoint(min, min + 1));
    BOOST_TEST_EQ(min + 1, midpoint(min, min + 2));
}

int main()
{
    test_trivial<boost::int128::uint128_t>();
    test_trivial<boost::int128::int128_t>();

    test_rounding<boost::int128::uint128_t>();
    test_rounding<boost::int128::int128_t>();

    test_big<boost::int128::uint128_t>();
    test_big<boost::int128::int128_t>();

    return boost::report_errors();
}
