// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#define BOOST_INT128_ALLOW_SIGN_COMPARE

#include <boost/int128.hpp>
#include <boost/core/lightweight_test.hpp>
#include <random>

#ifdef __GNUC__
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wsign-compare"
#endif

static std::mt19937_64 rng{42};
static std::uniform_int_distribution<std::uint64_t> u_dist{0, UINT64_MAX};
static std::uniform_int_distribution<std::int64_t> i_dist{0, INT64_MAX};
static constexpr std::size_t N {1024U};

using namespace boost::int128;

void test_left_unsigned()
{
    for (std::size_t i {0}; i < N; ++i)
    {
        const auto lhs {u_dist(rng)};
        const auto rhs {i_dist(rng)};

        const uint128_t lib_lhs {lhs};
        const int128_t lib_rhs {rhs};

        BOOST_TEST_EQ(lib_lhs == lib_rhs, lhs == static_cast<std::uint64_t>(rhs));
        BOOST_TEST_EQ(lib_lhs != lib_rhs, lhs != static_cast<std::uint64_t>(rhs));
        BOOST_TEST_EQ(lib_lhs > lib_rhs, lhs > static_cast<std::uint64_t>(rhs));
        BOOST_TEST_EQ(lib_lhs >= lib_rhs, lhs >= static_cast<std::uint64_t>(rhs));
        BOOST_TEST_EQ(lib_lhs < lib_rhs, lhs < static_cast<std::uint64_t>(rhs));
        BOOST_TEST_EQ(lib_lhs <= lib_rhs, lhs <= static_cast<std::uint64_t>(rhs));
    }

    const uint128_t lhs {42u};
    const int128_t rhs {-42};

    BOOST_TEST_EQ(lhs == rhs, false);
    BOOST_TEST_EQ(lhs != rhs, true);
    BOOST_TEST_EQ(lhs < rhs, false);
    BOOST_TEST_EQ(lhs <= rhs, false);
    BOOST_TEST_EQ(lhs > rhs, true);
    BOOST_TEST_EQ(lhs >= rhs, true);
}

void test_right_unsigned()
{
    for (std::size_t i {0}; i < N; ++i)
    {
        const auto lhs {i_dist(rng)};
        const auto rhs {u_dist(rng)};

        const int128_t lib_lhs {lhs};
        const uint128_t lib_rhs {rhs};

        BOOST_TEST_EQ(lib_lhs == lib_rhs, static_cast<std::uint64_t>(lhs) == rhs);
        BOOST_TEST_EQ(lib_lhs != lib_rhs, static_cast<std::uint64_t>(lhs) != rhs);
        BOOST_TEST_EQ(lib_lhs > lib_rhs, static_cast<std::uint64_t>(lhs) > rhs);
        BOOST_TEST_EQ(lib_lhs >= lib_rhs, static_cast<std::uint64_t>(lhs) >= rhs);
        BOOST_TEST_EQ(lib_lhs < lib_rhs, static_cast<std::uint64_t>(lhs) < rhs);
        BOOST_TEST_EQ(lib_lhs <= lib_rhs, static_cast<std::uint64_t>(lhs) <= rhs);
    }

    const int128_t lhs {-42};
    const uint128_t rhs {42u};

    BOOST_TEST_EQ(lhs == rhs, false);
    BOOST_TEST_EQ(lhs != rhs, true);
    BOOST_TEST_EQ(lhs < rhs, true);
    BOOST_TEST_EQ(lhs <= rhs, true);
    BOOST_TEST_EQ(lhs > rhs, false);
    BOOST_TEST_EQ(lhs >= rhs, false);
}

int main()
{
    test_left_unsigned();
    test_right_unsigned();

    return boost::report_errors();
}
