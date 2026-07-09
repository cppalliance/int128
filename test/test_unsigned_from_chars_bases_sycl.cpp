//  Copyright 2026 Matt Borland
//  Distributed under the Boost Software License, Version 1.0.
//  https://www.boost.org/LICENSE_1_0.txt

#include "sycl_test.hpp"

using boost::int128::uint128_t;

int main()
{
    int result {0};
    result |= int128_sycl_test::run_from_chars<uint128_t>(2);
    result |= int128_sycl_test::run_from_chars<uint128_t>(8);
    result |= int128_sycl_test::run_from_chars<uint128_t>(16);
    result |= int128_sycl_test::run_from_chars<uint128_t>(36);
    return result;
}
