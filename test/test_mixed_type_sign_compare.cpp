// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128.hpp>
#include <boost/core/lightweight_test.hpp>
#include <random>

#ifdef BOOST_INT128_HAS_SPACESHIP_OPERATOR
#include <compare>
#endif

#ifdef __GNUC__
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wsign-compare"
#  pragma GCC diagnostic ignored "-Wsign-conversion"
#endif

#ifdef BOOST_INT128_HAS_INT128

static std::mt19937_64 rng{42};
static std::uniform_int_distribution<std::uint64_t> u_dist{0, UINT64_MAX};
static std::uniform_int_distribution<std::int64_t> i_dist{INT64_MIN, INT64_MAX};
static constexpr std::size_t N {1024U};

using namespace boost::int128;

void test_left_unsigned()
{
    using boost::int128::detail::builtin_u128;

    for (std::size_t i {0}; i < N; ++i)
    {
        const auto lhs {u_dist(rng)};
        const auto rhs {i_dist(rng)};

        const uint128 lib_lhs {lhs};
        const int128 lib_rhs {rhs};

        // Builtin oracle: same-rank int128/uint128 -> both promote to unsigned __int128
        const builtin_u128 builtin_lhs {lhs};
        const builtin_u128 builtin_rhs = static_cast<builtin_u128>(static_cast<__int128>(rhs));

        BOOST_TEST_EQ(lib_lhs == lib_rhs, builtin_lhs == builtin_rhs);
        BOOST_TEST_EQ(lib_lhs != lib_rhs, builtin_lhs != builtin_rhs);
        BOOST_TEST_EQ(lib_lhs > lib_rhs,  builtin_lhs >  builtin_rhs);
        BOOST_TEST_EQ(lib_lhs >= lib_rhs, builtin_lhs >= builtin_rhs);
        BOOST_TEST_EQ(lib_lhs < lib_rhs,  builtin_lhs <  builtin_rhs);
        BOOST_TEST_EQ(lib_lhs <= lib_rhs, builtin_lhs <= builtin_rhs);

        #ifdef BOOST_INT128_HAS_SPACESHIP_OPERATOR

        BOOST_TEST((lib_lhs <=> lib_rhs) == (builtin_lhs <=> builtin_rhs));

        #endif
    }

    // Edge cases that the old deviations would have answered differently
    {
        const uint128 lhs {42u};
        const int128 rhs {-42};

        // Builtin: int128(-42) -> unsigned huge; 42 vs huge
        BOOST_TEST_EQ(lhs == rhs, false);
        BOOST_TEST_EQ(lhs != rhs, true);
        BOOST_TEST_EQ(lhs <  rhs, true);   // 42 < huge
        BOOST_TEST_EQ(lhs <= rhs, true);
        BOOST_TEST_EQ(lhs >  rhs, false);
        BOOST_TEST_EQ(lhs >= rhs, false);
    }
}

void test_right_unsigned()
{
    using boost::int128::detail::builtin_u128;

    for (std::size_t i {0}; i < N; ++i)
    {
        const auto lhs {i_dist(rng)};
        const auto rhs {u_dist(rng)};

        const int128 lib_lhs {lhs};
        const uint128 lib_rhs {rhs};

        const builtin_u128 builtin_lhs = static_cast<builtin_u128>(static_cast<__int128>(lhs));
        const builtin_u128 builtin_rhs {rhs};

        BOOST_TEST_EQ(lib_lhs == lib_rhs, builtin_lhs == builtin_rhs);
        BOOST_TEST_EQ(lib_lhs != lib_rhs, builtin_lhs != builtin_rhs);
        BOOST_TEST_EQ(lib_lhs > lib_rhs,  builtin_lhs >  builtin_rhs);
        BOOST_TEST_EQ(lib_lhs >= lib_rhs, builtin_lhs >= builtin_rhs);
        BOOST_TEST_EQ(lib_lhs < lib_rhs,  builtin_lhs <  builtin_rhs);
        BOOST_TEST_EQ(lib_lhs <= lib_rhs, builtin_lhs <= builtin_rhs);

        #ifdef BOOST_INT128_HAS_SPACESHIP_OPERATOR

        BOOST_TEST((lib_lhs <=> lib_rhs) == (builtin_lhs <=> builtin_rhs));

        #endif
    }

    {
        const int128 lhs {-42};
        const uint128 rhs {42u};

        // Builtin: int128(-42) -> unsigned huge; huge vs 42
        BOOST_TEST_EQ(lhs == rhs, false);
        BOOST_TEST_EQ(lhs != rhs, true);
        BOOST_TEST_EQ(lhs <  rhs, false);  // huge not < 42
        BOOST_TEST_EQ(lhs <= rhs, false);
        BOOST_TEST_EQ(lhs >  rhs, true);
        BOOST_TEST_EQ(lhs >= rhs, true);
    }
}

#endif // BOOST_INT128_HAS_INT128

int main()
{
    #ifdef BOOST_INT128_HAS_INT128

    test_left_unsigned();
    test_right_unsigned();

    #endif

    return boost::report_errors();
}
