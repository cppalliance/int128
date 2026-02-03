// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_ALLOW_SIGN_CONVERSION
#  define BOOST_INT128_ALLOW_SIGN_CONVERSION
#endif

#include <boost/int128.hpp>
#include <boost/int128/charconv.hpp>
#include <boost/core/lightweight_test.hpp>
#include <random>
#include <array>

#ifdef __clang__
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wold-style-cast"
#endif

#include <boost/random/uniform_int_distribution.hpp>

#ifdef __clang__
#  pragma clang diagnostic pop
#endif

static std::mt19937_64 rng{42};
boost::random::uniform_int_distribution<boost::int128::uint128_t> dist(std::numeric_limits<boost::int128::uint128_t>::min(),
                                                                       std::numeric_limits<boost::int128::uint128_t>::max());

constexpr std::array<boost::int128::uint128_t, 5> edge_cases {
    boost::int128::uint128_t{1, 0},
    boost::int128::uint128_t{1, 1},
    boost::int128::uint128_t{2, 0},
    boost::int128::uint128_t{2, UINT64_MAX - 1U},
    boost::int128::uint128_t{4, 0},
};

constexpr int trivial_num_digits(boost::int128::uint128_t x) noexcept
{
    int digits = 0;

    while (x)
    {
        x /= UINT64_C(10);
        ++digits;
    }

    return digits;
}

void test()
{
    for (int i {}; i < 39; ++i)
    {
        const auto current_val {boost::charconv::detail::int128_pow10[static_cast<std::size_t>(i)]};
        const auto current_val_digits {boost::charconv::detail::num_digits(current_val)};
        BOOST_TEST_EQ(current_val_digits, i + 1);
    }

    for (int i {1}; i < 39; ++i)
    {
        const auto current_val {boost::charconv::detail::int128_pow10[static_cast<std::size_t>(i)] - 1U};
        const auto current_val_digits {boost::charconv::detail::num_digits(current_val)};
        BOOST_TEST_EQ(current_val_digits, i);
    }

    for (int i {}; i < 1024; ++i)
    {
        const auto value {dist(rng)};
        const auto current_val_digits {boost::charconv::detail::num_digits(value)};
        const auto trivial_digits {trivial_num_digits(value)};

        BOOST_TEST_EQ(current_val_digits, trivial_digits);
    }

    for (const auto value : edge_cases)
    {
        const auto current_val_digits {boost::charconv::detail::num_digits(value)};
        const auto trivial_digits {trivial_num_digits(value)};

        BOOST_TEST_EQ(current_val_digits, trivial_digits);
    }
}

int main()
{
    test();

    return boost::report_errors();
}
