//  Copyright Matt Borland 2026.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <sycl/sycl.hpp>
#include <boost/int128.hpp>
#include <boost/int128/numeric.hpp>
#include <iostream>
#include <vector>
#include <random>
#include <cstdint>
#include <cstdlib>

using test_type = boost::int128::uint128;

// Calculates the GCD of two values on the SYCL device and verifies against the host
int main()
{
    std::mt19937_64 rng {42};

    const int numElements {50000};
    std::cout << "[Vector operation on " << numElements << " elements]" << std::endl;

    sycl::queue q;
    std::cout << "SYCL device: " << q.get_device().get_info<sycl::info::device::name>() << std::endl;

    // Allocate shared (USM) memory so the arrays are usable on both host and device
    test_type* in1 {sycl::malloc_shared<test_type>(numElements, q)};
    test_type* in2 {sycl::malloc_shared<test_type>(numElements, q)};
    test_type* out {sycl::malloc_shared<test_type>(numElements, q)};

    for (int i {0}; i < numElements; ++i)
    {
        in1[i] = test_type{rng(), rng()};
        in2[i] = test_type{rng(), rng()};
    }

    // Launch the SYCL kernel: each work item computes one gcd
    q.submit([&](sycl::handler& h)
    {
        h.parallel_for(sycl::range<1>(numElements), [=](sycl::id<1> idx)
        {
            const int i {static_cast<int>(idx[0])};
            out[i] = boost::int128::gcd(in1[i], in2[i]);
        });
    }).wait();

    // Perform the same operation on the host and compare
    std::vector<test_type> results;
    results.reserve(numElements);
    for (int i {0}; i < numElements; ++i)
    {
        results.emplace_back(boost::int128::gcd(in1[i], in2[i]));
    }

    int ret {EXIT_SUCCESS};
    for (int i {0}; i < numElements; ++i)
    {
        if (out[i] != results[i])
        {
            std::cerr << "Result verification failed at element: " << i << "!" << std::endl;
            ret = EXIT_FAILURE;
            break;
        }
    }

    if (ret == EXIT_SUCCESS)
    {
        std::cout << "All CPU and GPU computed elements match!" << std::endl;
    }

    sycl::free(in1, q);
    sycl::free(in2, q);
    sycl::free(out, q);

    return ret;
}
