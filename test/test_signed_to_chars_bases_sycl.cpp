//  Copyright 2026 Matt Borland
//  Distributed under the Boost Software License, Version 1.0.
//  https://www.boost.org/LICENSE_1_0.txt

#include "sycl_test.hpp"

using boost::int128::int128;

int main()
{
    int result {0};
    result |= int128_sycl_test::run_to_chars<int128>(2);
    result |= int128_sycl_test::run_to_chars<int128>(8);
    result |= int128_sycl_test::run_to_chars<int128>(16);
    result |= int128_sycl_test::run_to_chars<int128>(36);
    return result;
}
