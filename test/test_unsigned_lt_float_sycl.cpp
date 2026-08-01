//  Copyright 2026 Matt Borland
//  Distributed under the Boost Software License, Version 1.0.
//  https://www.boost.org/LICENSE_1_0.txt
//
// A mixed operation with a builtin floating point type on the device. long double does not
// exist on spir64, so only float and double participate there

#include "sycl_test.hpp"

using boost::int128::uint128;

int main()
{
    return int128_sycl_test::run_compare<uint128>(
        [](uint128 a, uint128 b) { return a < static_cast<double>(b) + 0.5; });
}
