//  Copyright 2026 Matt Borland
//  Distributed under the Boost Software License, Version 1.0.
//  https://www.boost.org/LICENSE_1_0.txt

#include "sycl_test.hpp"

using boost::int128::int128_t;
using boost::int128::uint128_t;

int main()
{
    return int128_sycl_test::run<uint128_t, int128_t>(
        [](uint128_t a, uint128_t, int) { return static_cast<int128_t>(a); });
}
