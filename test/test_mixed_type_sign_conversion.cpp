// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

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
// Use sqrt-bounded ranges so multiplication doesn't overflow the 64-bit oracle, but cover
// negative signed values to exercise the sign-extension path.
static std::uniform_int_distribution<std::uint64_t> u_dist{1, static_cast<std::uint64_t>(std::sqrt(UINT64_MAX))};
static std::uniform_int_distribution<std::int64_t> i_dist{
    -static_cast<std::int64_t>(std::sqrt(INT64_MAX)),
    static_cast<std::int64_t>(std::sqrt(INT64_MAX))};
static constexpr std::size_t N {1024U};

using namespace boost::int128;

#ifdef BOOST_INT128_HAS_INT128

void test()
{
    using boost::int128::detail::builtin_u128;

    for (std::size_t i {0}; i < N; ++i)
    {
        const auto u_val {u_dist(rng)};
        const auto i_val {i_dist(rng)};
        if (i_val == 0)
        {
            continue;  // skip divide/modulo by zero
        }

        const uint128_t lhs_u {u_val};
        const int128_t rhs_i {i_val};

        // Builtin oracle: both operands promoted to unsigned __int128
        const builtin_u128 builtin_lhs {u_val};
        const builtin_u128 builtin_rhs = static_cast<builtin_u128>(static_cast<__int128>(i_val));

        BOOST_TEST_EQ(lhs_u + rhs_i, uint128_t{builtin_lhs + builtin_rhs});
        BOOST_TEST_EQ(lhs_u - rhs_i, uint128_t{builtin_lhs - builtin_rhs});
        BOOST_TEST_EQ(lhs_u * rhs_i, uint128_t{builtin_lhs * builtin_rhs});
        BOOST_TEST_EQ(lhs_u / rhs_i, uint128_t{builtin_lhs / builtin_rhs});
        BOOST_TEST_EQ(lhs_u % rhs_i, uint128_t{builtin_lhs % builtin_rhs});

        // Reverse operand order
        if (u_val == 0)
        {
            continue;
        }
        BOOST_TEST_EQ(rhs_i + lhs_u, uint128_t{builtin_rhs + builtin_lhs});
        BOOST_TEST_EQ(rhs_i - lhs_u, uint128_t{builtin_rhs - builtin_lhs});
        BOOST_TEST_EQ(rhs_i * lhs_u, uint128_t{builtin_rhs * builtin_lhs});
        BOOST_TEST_EQ(rhs_i / lhs_u, uint128_t{builtin_rhs / builtin_lhs});
        BOOST_TEST_EQ(rhs_i % lhs_u, uint128_t{builtin_rhs % builtin_lhs});
    }
}

#endif // BOOST_INT128_HAS_INT128

int main()
{
#ifdef BOOST_INT128_HAS_INT128
    test();
#endif

    return boost::report_errors();
}
