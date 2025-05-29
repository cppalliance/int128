// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128.hpp>
#include <boost/core/lightweight_test.hpp>
#include <random>

constexpr std::size_t N {1024};
static std::mt19937_64 rng{42};
static std::uniform_int_distribution<std::uint64_t> dist{0, UINT64_MAX};

template <typename T>
void test_add_sat()
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
void test_sub_sat()
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
void test_mul_sat();

template <>
void test_mul_sat<boost::int128::uint128_t>()
{
    using boost::int128::mul_sat;

    boost::int128::uint128_t x {2U};
    boost::int128::uint128_t y {2U};
    int bit_count {4};

    while (bit_count <= 128)
    {
        const auto sat_res {mul_sat(x, y)};
        BOOST_TEST(sat_res < std::numeric_limits<boost::int128::uint128_t>::max());

        const auto res {x * y};
        BOOST_TEST(res == sat_res);

        x <<= 1U;
        y <<= 1U;

        bit_count += 2;
    }

    while (bit_count < 256)
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
void test_div_sat();

template <>
void test_div_sat<boost::int128::uint128_t>()
{
    using boost::int128::div_sat;

    for (std::size_t i {}; i < N; ++i)
    {
        const boost::int128::uint128_t value1{dist(rng), dist(rng)};
        const boost::int128::uint128_t value2{dist(rng), dist(rng)};

        const auto sat_res {div_sat(value1, value2)};
        const auto res {value1 / value2};

        BOOST_TEST(sat_res == res);
    }
}

template <typename T>
void test_saturate_cast();

template <>
void test_saturate_cast<boost::int128::uint128_t>()
{
    using boost::int128::saturate_cast;

    for (std::size_t i {}; i < N; ++i)
    {
        const auto value {dist(rng)};
        const boost::int128::uint128_t big_value{value};

        BOOST_TEST(saturate_cast<std::uint64_t>(big_value) == value);
        BOOST_TEST(saturate_cast<std::uint64_t>(big_value) == static_cast<std::uint64_t>(big_value));
    }

    for (std::size_t i {}; i < N; ++i)
    {
        const auto value {dist(rng)};
        const boost::int128::uint128_t big_value{value, value};
        BOOST_TEST(saturate_cast<std::uint64_t>(big_value) == std::numeric_limits<std::uint64_t>::max());
        BOOST_TEST(saturate_cast<std::uint64_t>(big_value) != static_cast<std::uint64_t>(big_value));
    }
}

int main()
{
    test_add_sat<boost::int128::uint128_t>();
    test_sub_sat<boost::int128::uint128_t>();
    test_mul_sat<boost::int128::uint128_t>();
    test_div_sat<boost::int128::uint128_t>();
    test_saturate_cast<boost::int128::uint128_t>();

    return boost::report_errors();
}
