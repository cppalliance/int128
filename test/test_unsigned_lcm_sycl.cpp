//  Copyright 2026 Matt Borland
//  Distributed under the Boost Software License, Version 1.0.
//  https://www.boost.org/LICENSE_1_0.txt

#include "sycl_test.hpp"

using boost::int128::uint128;

int main()
{
    // Restrict inputs so the true LCM stays within 128 bits.
    return int128_sycl_test::run<uint128, uint128>(
        [](uint128 a, uint128 b, int) { return boost::int128::lcm(a >> 64, b >> 64); });
}
