//  Copyright 2026 Matt Borland
//  Distributed under the Boost Software License, Version 1.0.
//  https://www.boost.org/LICENSE_1_0.txt

#include "sycl_test.hpp"

using boost::int128::int128_t;

int main()
{
    return int128_sycl_test::run<int128_t, int128_t>(
        [](int128_t a, int128_t b, int) { return boost::int128::saturating_add(a, b); });
}
