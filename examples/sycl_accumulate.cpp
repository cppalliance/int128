//  Copyright Matt Borland 2026.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// Exact, order-independent reduction on the GPU.
//
// A dot product of 64-bit integers overflows a 64-bit accumulator, and a
// floating-point accumulator loses the low-order bits. That second problem is
// worse on a GPU than on a CPU: a parallel reduction visits the elements in
// whatever order the hardware schedules, and floating-point addition is not
// associative, so the answer changes from run to run and from device to
// device. A uint128_t accumulator is wide enough to be exact here, and
// integer addition is associative, so the device result is bit-for-bit equal
// to the host result no matter how the work is partitioned.

#include <sycl/sycl.hpp>
#include <boost/int128.hpp>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <random>

using boost::int128::uint128_t;

int main()
{
    const int numElements {1 << 20};
    const int numPartials {1024};

    std::cout << "[Exact dot product of " << numElements << " pairs of 40-bit integers]" << std::endl;

    sycl::queue q;
    std::cout << "SYCL device: " << q.get_device().get_info<sycl::info::device::name>() << std::endl;

    // Shared (USM) allocations are readable from both the host and the device
    std::uint64_t* lhs {sycl::malloc_shared<std::uint64_t>(numElements, q)};
    std::uint64_t* rhs {sycl::malloc_shared<std::uint64_t>(numElements, q)};
    uint128_t* partials {sycl::malloc_shared<uint128_t>(numPartials, q)};

    std::mt19937_64 rng {42};
    for (int i {0}; i < numElements; ++i)
    {
        lhs[i] = rng() >> 24U;
        rhs[i] = rng() >> 24U;
    }

    // Each work item accumulates a strided slice into its own 128-bit partial sum
    q.submit([&](sycl::handler& h)
    {
        h.parallel_for(sycl::range<1>(numPartials), [=](sycl::id<1> idx)
        {
            const int p {static_cast<int>(idx[0])};

            uint128_t acc {0};
            for (int i {p}; i < numElements; i += numPartials)
            {
                acc += uint128_t{lhs[i]} * uint128_t{rhs[i]};
            }

            partials[p] = acc;
        });
    }).wait();

    uint128_t device_total {0};
    for (int i {0}; i < numPartials; ++i)
    {
        device_total += partials[i];
    }

    // The same sum on the host, walking the data in both directions
    uint128_t host_ascending {0};
    for (int i {0}; i < numElements; ++i)
    {
        host_ascending += uint128_t{lhs[i]} * uint128_t{rhs[i]};
    }

    uint128_t host_descending {0};
    for (int i {numElements - 1}; i >= 0; --i)
    {
        host_descending += uint128_t{lhs[i]} * uint128_t{rhs[i]};
    }

    // What the same reduction produces with a 64-bit accumulator
    std::uint64_t narrow {0};
    for (int i {0}; i < numElements; ++i)
    {
        narrow += lhs[i] * rhs[i];
    }

    std::cout << std::endl;
    std::cout << "uint128_t accumulator, exact and " << boost::int128::bit_width(host_ascending) << " bits wide" << std::endl;
    std::cout << "  device, " << numPartials << " strided partials : " << device_total << std::endl;
    std::cout << "  host, ascending order         : " << host_ascending << std::endl;
    std::cout << "  host, descending order        : " << host_descending << std::endl;
    std::cout << "  all three agree, so the partitioning of the reduction does not matter" << std::endl;

    std::cout << std::endl;
    std::cout << "uint64_t accumulator, silently wrong" << std::endl;
    std::cout << "  result                        : " << narrow << std::endl;
    std::cout << "  times it wrapped past 2^64    : " << (host_ascending >> 64U) << std::endl;

    // Widening to double is not a fix either: a single product already needs
    // more significand bits than double has.
    const uint128_t exact_product {uint128_t{lhs[0]} * uint128_t{rhs[0]}};
    const double rounded_product {static_cast<double>(lhs[0]) * static_cast<double>(rhs[0])};
    const uint128_t rounded_as_integer {rounded_product};
    const uint128_t rounding_error {rounded_as_integer > exact_product ?
                                    rounded_as_integer - exact_product :
                                    exact_product - rounded_as_integer};

    std::cout << std::endl;
    std::cout << "double, too few significand bits for even one product" << std::endl;
    std::cout << "  lhs[0] * rhs[0] as uint128_t  : " << exact_product
              << " (" << boost::int128::bit_width(exact_product) << " bits)" << std::endl;
    std::cout << "  lhs[0] * rhs[0] as double     : " << std::setprecision(17) << rounded_product << std::endl;
    std::cout << "  rounding error                : " << rounding_error << std::endl;

    const bool exact {device_total == host_ascending && device_total == host_descending};

    std::cout << std::endl;
    if (exact)
    {
        std::cout << "All CPU and GPU computed elements match!" << std::endl;
    }
    else
    {
        std::cerr << "Result verification failed!" << std::endl;
    }

    sycl::free(lhs, q);
    sycl::free(rhs, q);
    sycl::free(partials, q);

    return exact ? EXIT_SUCCESS : EXIT_FAILURE;
}
