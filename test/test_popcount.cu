//  Copyright Matt Borland 2026.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_INT128_ALLOW_SIGN_CONVERSION

#include <iostream>
#include <vector>
#include <random>
#include <limits>
#include <boost/int128.hpp>
#include <boost/int128/bit.hpp>
#include <boost/int128/random.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include "cuda_managed_ptr.hpp"
#include "stopwatch.hpp"

#include <cuda_runtime.h>

using test_type = boost::int128::uint128_t;

__global__ void cuda_test(const test_type *in, int *out, int numElements)
{
    int i = blockDim.x * blockIdx.x + threadIdx.x;

    if (i < numElements)
    {
        out[i] = boost::int128::popcount(in[i]);
    }
}

int main(void)
{
    std::mt19937_64 rng {42};

    cudaError_t err = cudaSuccess;

    int numElements = 50000;
    std::cout << "[Vector operation on " << numElements << " elements]" << std::endl;

    cuda_managed_ptr<test_type> input_vector(numElements);
    cuda_managed_ptr<int> output_vector(numElements);

    boost::random::uniform_int_distribution<test_type> dist {test_type{0U}, (std::numeric_limits<test_type>::max)()};
    for (std::size_t i = 0; i < numElements; ++i)
    {
        input_vector[i] = dist(rng);
    }

    int threadsPerBlock = 256;
    int blocksPerGrid = (numElements + threadsPerBlock - 1) / threadsPerBlock;
    std::cout << "CUDA kernel launch with " << blocksPerGrid << " blocks of " << threadsPerBlock << " threads" << std::endl;

    watch w;

    cuda_test<<<blocksPerGrid, threadsPerBlock>>>(input_vector.get(), output_vector.get(), numElements);
    cudaDeviceSynchronize();

    std::cout << "CUDA kernal done in: " << w.elapsed() << "s" << std::endl;

    err = cudaGetLastError();
    if (err != cudaSuccess)
    {
        std::cerr << "Failed to launch kernel (error code " << cudaGetErrorString(err) << ")!" << std::endl;
        return EXIT_FAILURE;
    }

    std::vector<int> results;
    results.reserve(numElements);
    w.reset();
    for (int i = 0; i < numElements; ++i)
    {
        results.push_back(boost::int128::popcount(input_vector[i]));
    }
    double t = w.elapsed();

    for (int i = 0; i < numElements; ++i)
    {
        if (output_vector[i] != results[i])
        {
            std::cerr << "Result verification failed at element " << i << "!" << std::endl;
            return EXIT_FAILURE;
        }
    }

    std::cout << "Test PASSED, normal calculation time: " << t << "s" << std::endl;
    std::cout << "Done\n";

    return 0;
}
