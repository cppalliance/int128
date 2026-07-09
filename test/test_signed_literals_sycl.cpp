//  Copyright 2026 Matt Borland
//  Distributed under the Boost Software License, Version 1.0.
//  https://www.boost.org/LICENSE_1_0.txt

#include "sycl_test.hpp"
#include <boost/int128/literals.hpp>
#include <iostream>
#include <cstdlib>

using boost::int128::int128_t;
using namespace boost::int128::literals;

// Exercises the signed user-defined literal operators inside a SYCL kernel and checks the
// device-produced values against the same literals evaluated on the host.
int main()
{
    constexpr int N {16};

    sycl::queue q;
    std::cout << "SYCL device: " << q.get_device().get_info<sycl::info::device::name>() << "\n";

    int128_t* out {sycl::malloc_shared<int128_t>(N, q)};

    q.submit([&](sycl::handler& h)
    {
        h.parallel_for(sycl::range<1>(N), [=](sycl::id<1> idx)
        {
            switch (idx[0])
            {
                case 0:  out[0]  = 0_i128; break;
                case 1:  out[1]  = 1_i128; break;
                case 2:  out[2]  = -1_i128; break;
                case 3:  out[3]  = 9223372036854775807_i128; break;
                case 4:  out[4]  = 0_I128; break;
                case 5:  out[5]  = 1_I128; break;
                case 6:  out[6]  = -42_I128; break;
                case 7:  out[7]  = 42_I128; break;
                case 8:  out[8]  = "0"_i128; break;
                case 9:  out[9]  = "170141183460469231731687303715884105727"_i128; break;
                case 10: out[10] = "-170141183460469231731687303715884105728"_i128; break;
                case 11: out[11] = "-999999999999999999"_i128; break;
                case 12: out[12] = "0"_I128; break;
                case 13: out[13] = "12345678901234567890"_I128; break;
                case 14: out[14] = "-12345678901234567890"_I128; break;
                case 15: out[15] = "-1"_I128; break;
                default: break;
            }
        });
    }).wait();

    int128_t expected[N];
    expected[0]  = 0_i128;
    expected[1]  = 1_i128;
    expected[2]  = -1_i128;
    expected[3]  = 9223372036854775807_i128;
    expected[4]  = 0_I128;
    expected[5]  = 1_I128;
    expected[6]  = -42_I128;
    expected[7]  = 42_I128;
    expected[8]  = "0"_i128;
    expected[9]  = "170141183460469231731687303715884105727"_i128;
    expected[10] = "-170141183460469231731687303715884105728"_i128;
    expected[11] = "-999999999999999999"_i128;
    expected[12] = "0"_I128;
    expected[13] = "12345678901234567890"_I128;
    expected[14] = "-12345678901234567890"_I128;
    expected[15] = "-1"_I128;

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
