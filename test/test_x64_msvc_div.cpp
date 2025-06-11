// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128.hpp>
#include <boost/core/lightweight_test.hpp>
#include <random>

#if defined(_M_AMD64) && !defined(__GNUC__) && !defined(__clang__) && _MSC_VER >= 1920

static std::mt19937_64 rng{42};
static constexpr std::size_t N{1024U};
static std::uniform_int_distribution<std::uint64_t> dist{UINT64_C(0), UINT64_MAX};
static std::uniform_int_distribution<std::uint32_t> dist32{UINT32_C(0), UINT32_MAX};

void test_two_words()
{
    for (std::size_t i{}; i < N; ++i)
    {
        boost::int128::uint128_t lhs{dist(rng), dist(rng)};
        boost::int128::uint128_t rhs{dist(rng), dist(rng)};

        // Guarantee lhs is greater than rhs
        if (lhs < rhs)
        {
            std::swap(lhs, rhs);
        }


        boost::int128::uint128_t remainder{};
        const auto quotient{boost::int128::detail::impl::div_mod_msvc<true>(lhs, rhs, remainder)};

        boost::int128::uint128_t knuth_remainder{};
        const auto knuth_quotient{boost::int128::detail::knuth_div(lhs, rhs, knuth_remainder)};

        BOOST_TEST_EQ(remainder, knuth_remainder);
        BOOST_TEST_EQ(quotient, knuth_quotient);
    }
}

void test_four_by_three()
{
    for (std::size_t i{}; i < N; ++i)
    {
        boost::int128::uint128_t lhs{dist(rng), dist(rng)};
        boost::int128::uint128_t rhs{dist32(rng), dist(rng)};

        boost::int128::uint128_t remainder{};
        const auto quotient{boost::int128::detail::impl::div_mod_msvc<true>(lhs, rhs, remainder)};

        boost::int128::uint128_t knuth_remainder{};
        const auto knuth_quotient{boost::int128::detail::knuth_div(lhs, rhs, knuth_remainder)};

        BOOST_TEST_EQ(remainder, knuth_remainder);
        BOOST_TEST_EQ(quotient, knuth_quotient);
    }
}

int main()
{
    test_two_words();
    test_four_by_three();

    return boost::report_errors();
}

#else

int main()
{
    return 0;
}

#endif 
