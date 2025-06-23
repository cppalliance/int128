// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128.hpp>

#ifndef BOOST_INT128_NO_CONSTEVAL_DETECTION

template <typename IntType>
constexpr bool test_spot_div(IntType lhs, IntType rhs, IntType known_res)
{
    const auto check_1{lhs / rhs};
    lhs /= rhs;

    return check_1 == known_res && lhs == known_res;
}

int main()
{
    static_assert(test_spot_div<boost::int128::uint128_t>(boost::int128::uint128_t{50012077812411ULL, 6429278683030093824ULL}, boost::int128::uint128_t{542101086ULL, 4477988020393345024ULL}, 92256), "Spot failure");

    return 0;
}

#else 

int main()
{
    return 0;
}

#endif // BOOST_INT128_NO_CONSTEVAL_DETECTION
