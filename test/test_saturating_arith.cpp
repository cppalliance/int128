// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128.hpp>
#include <boost/core/lightweight_test.hpp>
#include <random>

template <typename T>
void test_sat_add()
{
    using boost::int128::add_sat;

    T near_max {std::numeric_limits<T>::max() - T{5}};

    for (T i {0}; i < T{5}; ++i)
    {
        const auto sat_res {add_sat(near_max,  i)};
        BOOST_TEST(sat_res < std::numeric_limits<T>::max());

        const auto res {near_max + i};
        BOOST_TEST(sat_res == res);
    }

    near_max += T{5};
    for (T i {1}; i < T{5}; ++i)
    {
        const auto sat_res {add_sat(near_max,  i)};
        BOOST_TEST(sat_res == std::numeric_limits<T>::max());

        const auto res {near_max + i};
        BOOST_TEST(sat_res != res);
    }
}

template <typename T>
void test_sat_sub()
{
    using boost::int128::sub_sat;

    T near_min {std::numeric_limits<T>::min() + T{5}};

    for (T i {0}; i < T{5}; ++i)
    {
        const auto sat_res {sub_sat(near_min,  i)};
        BOOST_TEST(sat_res > std::numeric_limits<T>::min());

        const auto res {near_min - i};
        BOOST_TEST(sat_res == res);
    }

    near_min -= T{5};
    for (T i {1}; i < T{5}; ++i)
    {
        const auto sat_res {sub_sat(near_min,  i)};
        BOOST_TEST(sat_res == std::numeric_limits<T>::min());

        const auto res {near_min + i};
        BOOST_TEST(sat_res != res);
    }
}

template <typename T>
void test_sat_mul();

template <>
void test_sat_mul<boost::int128::uint128_t>()
{
    using boost::int128::mul_sat;

    boost::int128::uint128_t x {2U};
    boost::int128::uint128_t y {2U};
    int bit_count {4};

    for (int i {0}; bit_count <= 128; ++i)
    {
        const auto sat_res {mul_sat(x, y)};
        BOOST_TEST(sat_res < std::numeric_limits<boost::int128::uint128_t>::max());

        const auto res {x * y};
        BOOST_TEST(res == sat_res);

        x <<= 1U;
        y <<= 1U;

        bit_count += 2;
    }

    for (int i {0}; bit_count < 256; ++i)
    {
        const auto sat_res {mul_sat(x, y)};
        BOOST_TEST(sat_res == std::numeric_limits<boost::int128::uint128_t>::max());

        const auto res {x * y};
        BOOST_TEST(res != sat_res);

        x <<= 1U;
        y <<= 1U;

        bit_count += 2;
    }

}

template <typename T>
void test_sat_div();

template <>
void test_sat_div<boost::int128::uint128_t>()
{
    using boost::int128::div_sat;

    std::mt19937_64 rng{42};
    std::uniform_int_distribution<std::uint64_t> dist{0, UINT64_MAX};

    for (int i {}; i < 1024; ++i)
    {
        const boost::int128::uint128_t value1{dist(rng), dist(rng)};
        const boost::int128::uint128_t value2{dist(rng), dist(rng)};

        const auto sat_res {div_sat(value1, value2)};
        const auto res {value1 / value2};

        BOOST_TEST(sat_res == res);
    }
}

int main()
{
    test_sat_add<boost::int128::uint128_t>();
    test_sat_sub<boost::int128::uint128_t>();
    test_sat_mul<boost::int128::uint128_t>();
    test_sat_div<boost::int128::uint128_t>();

    return boost::report_errors();
}
