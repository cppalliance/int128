// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128.hpp>
#include <boost/core/lightweight_test.hpp>
#include <random>

#include "boost/multiprecision/fwd.hpp"

using namespace boost::int128;

static constexpr std::size_t N {1024U};
static std::mt19937_64 rng(42);
static std::uniform_int_distribution<std::uint64_t> dist(0, UINT64_MAX);
static std::uniform_int_distribution<std::int64_t> idist(INT64_MIN, INT64_MAX);

#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable:4127)
#endif

template <std::size_t layout>
void test_unsigned_div()
{
    for (std::size_t i {}; i < N; ++i)
    {
        uint128_t lhs;
        uint128_t rhs;

        switch (layout)
        {
            case 0U:
                lhs = uint128_t {dist(rng), dist(rng)};
                rhs = uint128_t {dist(rng), dist(rng)};
                break;
            case 1U:
                lhs = uint128_t {dist(rng)};
                rhs = uint128_t {dist(rng), dist(rng)};
                break;
            case 2U:
                lhs = uint128_t {dist(rng), dist(rng)};
                rhs = uint128_t {dist(rng)};
                break;
            case 3U:
                lhs = uint128_t {dist(rng)};
                rhs = uint128_t {dist(rng)};
                break;
            default:                        // LCOV_EXCL_LINE
                BOOST_INT128_UNREACHABLE;   // LCOV_EXCL_LINE
        }

        const auto div_res {div(lhs, rhs)};
        BOOST_TEST_EQ(div_res.quot, lhs / rhs);
        BOOST_TEST_EQ(div_res.rem, lhs % rhs);

        const auto inv_div_res {div(rhs, lhs)};
        BOOST_TEST_EQ(inv_div_res.quot, rhs / lhs);
        BOOST_TEST_EQ(inv_div_res.rem, rhs % lhs);
    }

    uint128_t lhs {dist(rng), dist(rng)};
    uint128_t zero {dist(rng) * 0U, dist(rng) * 0U};
    const auto lhs_num {div(lhs, zero)};
    BOOST_TEST_EQ(lhs_num.quot, 0U);
    BOOST_TEST_EQ(lhs_num.rem, 0U);

    const auto lhs_denom {div(zero, lhs)};
    BOOST_TEST_EQ(lhs_denom.quot, 0U);
    BOOST_TEST_EQ(lhs_denom.rem, 0U);
}

#ifdef _MSC_VER
#  pragma warning(pop)
#endif

int main()
{
    test_unsigned_div<0>();
    test_unsigned_div<1>();
    test_unsigned_div<2>();
    test_unsigned_div<3>();

    return boost::report_errors();
}
