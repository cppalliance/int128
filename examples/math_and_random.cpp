// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// Allowing sign conversion is a required pre-requisite for Boost.Random
#define BOOST_INT128_ALLOW_SIGN_CONVERSION

#include <boost/int128.hpp>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wundef"
#pragma clang diagnostic ignored "-Wstring-conversion"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
#endif

#include <boost/math/statistics/univariate_statistics.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/xoshiro.hpp>
#include <iostream>
#include <limits>
#include <array>

int main()
{
    // Setup our rng and distribution
    boost::random::xoshiro128pp rng {42};
    boost::random::uniform_int_distribution<boost::int128::uint128_t> dist {0, (std::numeric_limits<boost::int128::uint128_t>::max)()};

    // Create a dataset for ourselves of random uint128_ts using our dist and rng from above
    std::array<boost::int128::uint128_t, 10000> data_set;
    for (auto& value : data_set)
    {
        value = dist(rng);
    }

    // Perform some rudimentary statistical analysis on our dataset
    std::cout << "    Mean: " << boost::math::statistics::mean(data_set) << '\n'
              << "Variance: " << boost::math::statistics::variance(data_set) << '\n'
              << "  Median: " << boost::math::statistics::median(data_set) << std::endl;

    return 0;
}
