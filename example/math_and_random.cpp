// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128.hpp>
#include <boost/int128/random.hpp> // Not included in the convenience header, but needed for boost.random interop

// tag::exclude[]
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wundef"
#pragma clang diagnostic ignored "-Wstring-conversion"
#pragma clang diagnostic ignored "-Wsign-conversion"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#endif

// end::exclude[]
#include <boost/math/statistics/univariate_statistics.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <iostream>
#include <limits>
#include <array>
#include <random>

int main()
{
    std::cout << "=== uint128 ===" << '\n';

    // Setup our rng and distribution
    std::mt19937_64 rng {42};
    boost::random::uniform_int_distribution<boost::int128::uint128> dist {0, (std::numeric_limits<boost::int128::uint128>::max)()};

    // Create a dataset for ourselves of random uint128s using our dist and rng from above
    std::array<boost::int128::uint128, 10000> data_set;
    for (auto& value : data_set)
    {
        value = dist(rng);
    }

    // Perform some rudimentary statistical analysis on our dataset
    std::cout << "    Mean: " << boost::math::statistics::mean(data_set) << '\n';
    std::cout << "Variance: " << boost::math::statistics::variance(data_set) << '\n';
    std::cout << "  Median: " << boost::math::statistics::median(data_set) << '\n';

    std::cout << "=== int128 ===" << '\n';

    // We can also generate random signed integers using int128
    boost::random::uniform_int_distribution<boost::int128::int128> signed_dist {std::numeric_limits<boost::int128::int128>::min(), std::numeric_limits<boost::int128::int128>::max()};

    std::cout << "Random int128: " << signed_dist(rng) << std::endl;

    return 0;
}
