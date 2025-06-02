// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128.hpp>
#include <boost/core/lightweight_test.hpp>
#include <random>

constexpr std::size_t N {1024};
static std::mt19937_64 rng{42};
static std::uniform_int_distribution<std::uint64_t> dist{0, UINT64_MAX};
static std::uniform_int_distribution<std::int64_t> signed_dist{INT64_MIN, INT64_MAX};

template <typename T>
void test_add_sat();

template <>
void test_add_sat<boost::int128::uint128_t>()
{
    using boost::int128::add_sat;

    auto near_max {std::numeric_limits<boost::int128::uint128_t>::max() - boost::int128::uint128_t{5}};

    for (boost::int128::uint128_t i {0}; i < boost::int128::uint128_t{5}; ++i)
    {
        const auto sat_res {add_sat(near_max,  i)};
        BOOST_TEST(sat_res < std::numeric_limits<boost::int128::uint128_t>::max());

        const auto res {near_max + i};
        BOOST_TEST(sat_res == res);
    }

    near_max += boost::int128::uint128_t{5};
    for (boost::int128::uint128_t i {1}; i < boost::int128::uint128_t{5}; ++i)
    {
        const auto sat_res {add_sat(near_max,  i)};
        BOOST_TEST(sat_res == std::numeric_limits<boost::int128::uint128_t>::max());

        const auto res {near_max + i};
        BOOST_TEST(sat_res != res);
    }
}

template <>
void test_add_sat<boost::int128::int128_t>()
{
    // There are four branches that need to be tested
    // 1) x > 0 && y > 0
    // 2) x < 0 && y > 0
    // 3) x > 0 && y < 0
    // 4) x <= 0 && y <= 0

    using boost::int128::add_sat;

    constexpr auto min {std::numeric_limits<boost::int128::int128_t>::min()};
    constexpr auto max {std::numeric_limits<boost::int128::int128_t>::max()};

    // 1 - We can only overflow, but there are many such cases
    {
        auto near_max {max - boost::int128::int128_t{5}};

        for (boost::int128::int128_t i {0}; i < boost::int128::int128_t{5}; ++i)
        {
            const auto sat_res {add_sat(near_max,  i)};
            BOOST_TEST(sat_res < max);

            const auto res {near_max + i};
            BOOST_TEST(sat_res == res);
        }

        near_max += boost::int128::int128_t{5};
        for (boost::int128::int128_t i {1}; i < boost::int128::int128_t{5}; ++i)
        {
            const auto sat_res {add_sat(near_max,  i)};
            BOOST_TEST(sat_res == max);
        }
    }

    // 2 - Impossible to overflow or underflow since the negative range is larger than positive range
    {
        for (std::size_t i {0}; i < N; ++i)
        {
            boost::int128::int128_t x {signed_dist(rng), dist(rng)};
            boost::int128::int128_t y {signed_dist(rng), dist(rng)};

            if (x > 0)
            {
                x = -x;
            }
            if (y < 0)
            {
                y = -y;
            }

            const auto naive_res {x + y};
            const auto sat_res {add_sat(x, y)};
            BOOST_TEST(naive_res == sat_res);
        }

        const auto min_max_naive_res {min + max};
        const auto min_max_sat_res {add_sat(min, max)};
        BOOST_TEST(min_max_naive_res == min_max_sat_res);
    }

    // 3 - Again impossible to overflow or underflow
    {
        for (std::size_t i {0}; i < N; ++i)
        {
            boost::int128::int128_t x {signed_dist(rng), dist(rng)};
            boost::int128::int128_t y {signed_dist(rng), dist(rng)};

            if (x < 0)
            {
                x = -x;
            }
            if (y > 0)
            {
                y = -y;
            }

            const auto naive_res {x + y};
            const auto sat_res {add_sat(x, y)};
            BOOST_TEST(naive_res == sat_res);
        }

        const auto min_max_naive_res {max + min};
        const auto min_max_sat_res {add_sat(max, min)};
        BOOST_TEST(min_max_naive_res == min_max_sat_res);
    }

    // 4 - We can only underflow, and there exist many ways to do so
    {
        auto near_min {min + boost::int128::int128_t{5}};

        for (boost::int128::int128_t i {0}; i > boost::int128::int128_t{-5}; --i)
        {
            const auto sat_res {add_sat(near_min,  i)};
            BOOST_TEST(sat_res > min);

            const auto res {near_min + i};
            BOOST_TEST(sat_res == res);
        }

        near_min -= boost::int128::int128_t{5};
        for (boost::int128::int128_t i {-1}; i > boost::int128::int128_t{-5}; --i)
        {
            const auto sat_res {add_sat(near_min,  i)};
            BOOST_TEST(sat_res == min);
        }
    }
}

template <typename T>
void test_sub_sat();

template <>
void test_sub_sat<boost::int128::uint128_t>()
{
    using boost::int128::sub_sat;

    auto near_min {std::numeric_limits<boost::int128::uint128_t>::min() + boost::int128::uint128_t{5}};

    for (boost::int128::uint128_t i {0}; i < boost::int128::uint128_t{5}; ++i)
    {
        const auto sat_res {sub_sat(near_min,  i)};
        BOOST_TEST(sat_res > std::numeric_limits<boost::int128::uint128_t>::min());

        const auto res {near_min - i};
        BOOST_TEST(sat_res == res);
    }

    near_min -= boost::int128::uint128_t{5};
    for (boost::int128::uint128_t i {1}; i < boost::int128::uint128_t{5}; ++i)
    {
        const auto sat_res {sub_sat(near_min,  i)};
        BOOST_TEST(sat_res == std::numeric_limits<boost::int128::uint128_t>::min());

        const auto res {near_min + i};
        BOOST_TEST(sat_res != res);
    }
}

template <>
void test_sub_sat<boost::int128::int128_t>()
{
    // The same as add_sat but the logic backwards
    // There are four branches that need to be tested
    // 1) x > 0 && y > 0
    // 2) x < 0 && y > 0
    // 3) x > 0 && y < 0
    // 4) x <= 0 && y <= 0

    using boost::int128::sub_sat;

    constexpr auto min {std::numeric_limits<boost::int128::int128_t>::min()};
    constexpr auto max {std::numeric_limits<boost::int128::int128_t>::max()};

    // 1 - Nothing bad can happen here
    {
        for (std::size_t i {0}; i < N; ++i)
        {
            boost::int128::int128_t x {signed_dist(rng), dist(rng)};
            boost::int128::int128_t y {signed_dist(rng), dist(rng)};

            if (x < 0)
            {
                x = -x;
            }
            if (y < 0)
            {
                y = -y;
            }

            const auto naive_res {x - y};
            const auto sat_res {sub_sat(x, y)};
            BOOST_TEST(naive_res == sat_res);
        }

        const auto min_max_naive_res {max - max};
        const auto min_max_sat_res {sub_sat(max, max)};
        BOOST_TEST(min_max_naive_res == min_max_sat_res);
    }

    // 2 - Underflow is possible in this case
    {
        auto near_min {min + boost::int128::int128_t{5}};

        for (boost::int128::int128_t i {0}; i < boost::int128::int128_t{5}; ++i)
        {
            const auto sat_res {sub_sat(near_min,  i)};
            BOOST_TEST(sat_res > min);

            const auto res {near_min - i};
            BOOST_TEST(sat_res == res);
        }

        near_min = min;
        for (boost::int128::int128_t i {1}; i < boost::int128::int128_t{5}; ++i)
        {
            const auto sat_res {sub_sat(near_min,  i)};
            BOOST_TEST(sat_res == min);

            const auto res {near_min - i};
            BOOST_TEST(sat_res != res);
        }
    }

    // 3 - Overflow is possible in this case
    {
        auto near_max {max - boost::int128::int128_t{5}};

        for (boost::int128::int128_t i {0}; i > boost::int128::int128_t{-5}; --i)
        {
            const auto sat_res {sub_sat(near_max,  i)};
            BOOST_TEST(sat_res < max);

            const auto res {near_max - i};
            BOOST_TEST(sat_res == res);
        }

        near_max += boost::int128::int128_t{5};
        for (boost::int128::int128_t i {-1}; i > boost::int128::int128_t{-5}; --i)
        {
            const auto sat_res {sub_sat(near_max,  i)};
            BOOST_TEST(sat_res == max);
        }
    }

    // 4 - Nothing bad here
    {
        for (std::size_t i {0}; i < N; ++i)
        {
            boost::int128::int128_t x {signed_dist(rng), dist(rng)};
            boost::int128::int128_t y {signed_dist(rng), dist(rng)};

            if (x > 0)
            {
                x = -x;
            }
            if (y > 0)
            {
                y = -y;
            }

            const auto naive_res {x - y};
            const auto sat_res {sub_sat(x, y)};
            BOOST_TEST(naive_res == sat_res);
        }

        const auto min_max_naive_res {min - min};
        const auto min_max_sat_res {sub_sat(min, min)};
        BOOST_TEST(min_max_naive_res == min_max_sat_res);
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

template <>
void test_mul_sat<boost::int128::int128_t>()
{
    using boost::int128::mul_sat;

    {
        boost::int128::int128_t x {2};
        boost::int128::int128_t y {2};
        int bit_count {4};

        while (bit_count < 128)
        {
            const auto sat_res {mul_sat(x, y)};
            BOOST_TEST(sat_res < std::numeric_limits<boost::int128::int128_t>::max());

            const auto res {x * y};
            BOOST_TEST(res == sat_res);

            x <<= 1U;
            y <<= 1U;

            bit_count += 2;
        }

        while (bit_count < 254)
        {
            const auto sat_res {mul_sat(x, y)};
            BOOST_TEST(sat_res == std::numeric_limits<boost::int128::int128_t>::max());

            const auto res {x * y};
            BOOST_TEST(res != sat_res);

            x <<= 1U;
            y <<= 1U;

            bit_count += 2;
        }
    }
    {
        boost::int128::int128_t x {2};
        boost::int128::int128_t y {-2};
        int bit_count {4};

        while (bit_count < 128)
        {
            const auto sat_res {mul_sat(x, y)};
            BOOST_TEST(sat_res < std::numeric_limits<boost::int128::int128_t>::max());
            BOOST_TEST(sat_res > std::numeric_limits<boost::int128::int128_t>::min());

            const boost::int128::int128_t res {x * y};
            BOOST_TEST(res == sat_res);

            x <<= 1;
            y <<= 1;

            bit_count += 2;
        }

        while (bit_count < 254)
        {
            if (y > 0)
            {
                return; // LCOV_EXCL_LINE
            }

            const boost::int128::int128_t sat_res {mul_sat(x, y)};
            BOOST_TEST(sat_res == std::numeric_limits<boost::int128::int128_t>::min());
            x <<= 1;
            y <<= 1;

            bit_count += 2;
        }
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

template <>
void test_div_sat<boost::int128::int128_t>()
{
    using boost::int128::div_sat;

    for (std::size_t i {}; i < N; ++i)
    {
        const boost::int128::int128_t value1{signed_dist(rng), dist(rng)};
        const boost::int128::int128_t value2{signed_dist(rng), dist(rng)};

        const auto sat_res {div_sat(value1, value2)};
        const auto res {value1 / value2};

        BOOST_TEST(sat_res == res);
    }

    constexpr auto min_val {std::numeric_limits<boost::int128::int128_t>::min()};
    constexpr auto max_val{std::numeric_limits<boost::int128::int128_t>::max()};
    BOOST_TEST(div_sat(min_val, -1) == max_val);
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

template <>
void test_saturate_cast<boost::int128::int128_t>()
{
    using boost::int128::saturate_cast;

    for (std::size_t i {}; i < N; ++i)
    {
        const auto value {signed_dist(rng)};
        const boost::int128::int128_t big_value{value};

        BOOST_TEST(saturate_cast<std::int64_t>(big_value) == value);
        BOOST_TEST(saturate_cast<std::int64_t>(big_value) == static_cast<std::int64_t>(big_value));
    }

    for (std::size_t i {}; i < N; ++i)
    {
        const auto hi_word {signed_dist(rng)};
        const auto lo_word {dist(rng)};
        const boost::int128::int128_t big_value{hi_word, lo_word};

        if (hi_word > 0)
        {
            BOOST_TEST(saturate_cast<std::int64_t>(big_value) == std::numeric_limits<std::int64_t>::max());
        }
        else
        {
            BOOST_TEST(saturate_cast<std::int64_t>(big_value) == std::numeric_limits<std::int64_t>::min());
        }
    }
}

int main()
{
    test_add_sat<boost::int128::uint128_t>();
    test_sub_sat<boost::int128::uint128_t>();
    test_mul_sat<boost::int128::uint128_t>();
    test_div_sat<boost::int128::uint128_t>();
    test_saturate_cast<boost::int128::uint128_t>();

    test_add_sat<boost::int128::int128_t>();
    test_sub_sat<boost::int128::int128_t>();
    test_mul_sat<boost::int128::int128_t>();
    test_div_sat<boost::int128::int128_t>();
    test_saturate_cast<boost::int128::int128_t>();

    return boost::report_errors();
}
