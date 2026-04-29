//  Copyright Matt Borland 2026.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <iomanip>
#include <vector>
#include <random>
#include <limits>
#include <boost/int128.hpp>
#include <boost/int128/random.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include "cuda_managed_ptr.hpp"
#include "stopwatch.hpp"

// For the CUDA runtime routines (prefixed with "cuda_")
#include <cuda_runtime.h>

using signed_type = boost::int128::int128_t;
using unsigned_type = boost::int128::uint128_t;

/**
 * CUDA Kernel Device code
 *
 * Converts signed int128 values to unsigned int128 values
 */
__global__ void cuda_test(const signed_type *in, unsigned_type *out, int numElements)
{
    int i = blockDim.x * blockIdx.x + threadIdx.x;

    if (i < numElements)
    {
        out[i] = unsigned_type(in[i]);
    }
}

/**
 * Host main routine
 */
int main(void)
{
    std::mt19937_64 rng {42};

    // Error code to check return values for CUDA calls
    cudaError_t err = cudaSuccess;

    // Print the vector length to be used, and compute its size
    int numElements = 50000;
    std::cout << "[Vector operation on " << numElements << " elements]" << std::endl;

    // Allocate the managed input vector
    cuda_managed_ptr<signed_type> input_vector(numElements);

    // Allocate the managed output vector
    cuda_managed_ptr<unsigned_type> output_vector(numElements);

    // Initialize the input vectors with non-negative values so bit patterns match
    boost::random::uniform_int_distribution<signed_type> dist {signed_type{0}, (std::numeric_limits<signed_type>::max)()};
    for (std::size_t i = 0; i < numElements; ++i)
    {
        input_vector[i] = dist(rng);
    }

    // Launch the CUDA Kernel
    int threadsPerBlock = 256;
    int blocksPerGrid =(numElements + threadsPerBlock - 1) / threadsPerBlock;
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

    // Verify that the result vector is correct
    std::vector<unsigned_type> results;
    results.reserve(numElements);
    w.reset();
    for(int i = 0; i < numElements; ++i)
    {
        results.push_back(unsigned_type(input_vector[i]));
    }
    double t = w.elapsed();
    // check the results
    for(int i = 0; i < numElements; ++i)
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
