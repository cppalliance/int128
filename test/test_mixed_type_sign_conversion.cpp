// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#define BOOST_INT128_ALLOW_SIGN_CONVERSION

#include <boost/int128.hpp>
#include <boost/core/lightweight_test.hpp>
#include <random>
#include <cmath>

#ifdef __GNUC__
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wsign-compare"
#  pragma GCC diagnostic ignored "-Wsign-conversion"
#endif

static std::mt19937_64 rng{42};
static std::uniform_int_distribution<std::uint64_t> u_dist{0, static_cast<std::uint64_t>(std::sqrt(UINT64_MAX))};
static std::uniform_int_distribution<std::int64_t> i_dist{0, static_cast<std::int64_t>(std::sqrt(INT64_MAX))};
static constexpr std::size_t N {1024U};

using namespace boost::int128;

void test()
{
    for (std::size_t i {0}; i < N; ++i)
    {
        const auto u_val {u_dist(rng)};
        const auto i_val {i_dist(rng)};

        if (u_val > static_cast<std::uint64_t>(i_val))
        {
            const uint128_t lhs {u_val};
            const int128_t rhs {i_val};

            BOOST_TEST_EQ(lhs + rhs, u_val + static_cast<std::uint64_t>(i_val));
            BOOST_TEST_EQ(lhs - rhs, u_val - static_cast<std::uint64_t>(i_val));
            BOOST_TEST_EQ(lhs * rhs, u_val * static_cast<std::uint64_t>(i_val));
            BOOST_TEST_EQ(lhs / rhs, u_val / static_cast<std::uint64_t>(i_val));
            BOOST_TEST_EQ(lhs % rhs, u_val % static_cast<std::uint64_t>(i_val));
        }
        else
        {
            const int128_t lhs {i_val};
            const uint128_t rhs {u_val};

            BOOST_TEST_EQ(lhs + rhs, static_cast<std::uint64_t>(i_val) + u_val);
            BOOST_TEST_EQ(lhs - rhs, static_cast<std::uint64_t>(i_val) - u_val);
            BOOST_TEST_EQ(lhs * rhs, static_cast<std::uint64_t>(i_val) * u_val);
            BOOST_TEST_EQ(lhs / rhs, static_cast<std::uint64_t>(i_val) / u_val);
            BOOST_TEST_EQ(lhs % rhs, static_cast<std::uint64_t>(i_val) % u_val);
        }
    }
}

int main()
{
    test();

    return boost::report_errors();
}