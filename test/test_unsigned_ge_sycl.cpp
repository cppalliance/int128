//  Copyright 2026 Matt Borland
//  Distributed under the Boost Software License, Version 1.0.
//  https://www.boost.org/LICENSE_1_0.txt

#include "sycl_test.hpp"

using boost::int128::uint128_t;

int main()
{
    return int128_sycl_test::run_compare<uint128_t>(
        [](uint128_t a, uint128_t b) { return a >= b; });
}
