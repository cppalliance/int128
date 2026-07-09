//  Copyright 2026 Matt Borland
//  Distributed under the Boost Software License, Version 1.0.
//  https://www.boost.org/LICENSE_1_0.txt

#include "sycl_test.hpp"
#include <boost/int128/literals.hpp>
#include <iostream>
#include <cstdlib>

using boost::int128::uint128_t;
using namespace boost::int128::literals;

// Exercises the user-defined literal operators inside a SYCL kernel and checks the
// device-produced values against the same literals evaluated on the host.
int main()
{
    constexpr int N {24};

    sycl::queue q;
    std::cout << "SYCL device: " << q.get_device().get_info<sycl::info::device::name>() << "\n";

    uint128_t* out {sycl::malloc_shared<uint128_t>(N, q)};

    q.submit([&](sycl::handler& h)
    {
        h.parallel_for(sycl::range<1>(N), [=](sycl::id<1> idx)
        {
            switch (idx[0])
            {
                case 0:  out[0]  = 0_u128; break;
                case 1:  out[1]  = 1_u128; break;
                case 2:  out[2]  = 18446744073709551615_u128; break;
                case 3:  out[3]  = 999999999999999999_u128; break;
                case 4:  out[4]  = 0_U128; break;
                case 5:  out[5]  = 1_U128; break;
                case 6:  out[6]  = 18446744073709551615_U128; break;
                case 7:  out[7]  = 999999999999999999_U128; break;
                case 8:  out[8]  = "0"_u128; break;
                case 9:  out[9]  = "1"_u128; break;
                case 10: out[10] = "340282366920938463463374607431768211455"_u128; break;
                case 11: out[11] = "999999999999999999"_u128; break;
                case 12: out[12] = "0"_U128; break;
                case 13: out[13] = "1"_U128; break;
                case 14: out[14] = "340282366920938463463374607431768211455"_U128; break;
                case 15: out[15] = "999999999999999999"_U128; break;
                case 16: out[16] = 0_u128; break;
                case 17: out[17] = 1_u128; break;
                case 18: out[18] = 18446744073709551615_u128; break;
                case 19: out[19] = 42_u128; break;
                case 20: out[20] = 0_U128; break;
                case 21: out[21] = 1_U128; break;
                case 22: out[22] = 18446744073709551615_U128; break;
                case 23: out[23] = 42_U128; break;
                default: break;
            }
        });
    }).wait();

    uint128_t expected[N];
    expected[0]  = 0_u128;
    expected[1]  = 1_u128;
    expected[2]  = 18446744073709551615_u128;
    expected[3]  = 999999999999999999_u128;
    expected[4]  = 0_U128;
    expected[5]  = 1_U128;
    expected[6]  = 18446744073709551615_U128;
    expected[7]  = 999999999999999999_U128;
    expected[8]  = "0"_u128;
    expected[9]  = "1"_u128;
    expected[10] = "340282366920938463463374607431768211455"_u128;
    expected[11] = "999999999999999999"_u128;
    expected[12] = "0"_U128;
    expected[13] = "1"_U128;
    expected[14] = "340282366920938463463374607431768211455"_U128;
    expected[15] = "999999999999999999"_U128;
    expected[16] = 0_u128;
    expected[17] = 1_u128;
    expected[18] = 18446744073709551615_u128;
    expected[19] = 42_u128;
    expected[20] = 0_U128;
    expected[21] = 1_U128;
    expected[22] = 18446744073709551615_U128;
    expected[23] = 42_U128;

    int failures {0};
    for (int i {0}; i < N; ++i)
    {
        if (out[i] != expected[i])
        {
            std::cerr << "Mismatch at literal case " << i << "\n";
            ++failures;
        }
    }

    sycl::free(out, q);

    if (failures == 0)
    {
        std::cout << "Test PASSED\n";
        return EXIT_SUCCESS;
    }

    std::cerr << "Test FAILED with " << failures << " mismatches\n";
    return EXIT_FAILURE;
}
