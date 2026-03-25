//  Copyright Matt Borland 2026.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <vector>
#include <random>
#include <limits>
#include <boost/int128.hpp>
#include <boost/int128/numeric.hpp>
#include <boost/int128/random.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#include <cuda_runtime.h>

using test_type = boost::int128::uint128_t;

// Calculates the GCD of 2 values on device
__global__ void cuda_gcd(const test_type* in1, const test_type* in2, test_type* out, int numElements)
{
    int i = blockDim.x * blockIdx.x + threadIdx.x;

    if (i < numElements)
    {
        out[i] = boost::int128::gcd(in1[i], in2[i]);
    }
}

// Allocate managed space so that the arrays can be used on both host and device
void allocate(test_type* in, int numElements)
{
    cudaError_t err = cudaSuccess;
    err = cudaMallocManaged(&in, numElements * sizeof(test_type));
    if (err != cudaSuccess)
    {
        throw std::runtime_error(cudaGetErrorString(err));
    }

    cudaDeviceSynchronize();
}

void cleanup(test_type* in1, test_type* in2, test_type* out)
{
    if (in1 != nullptr)
    {
        free(in1);
        in1 = nullptr;
    }

    if (in2 != nullptr)
    {
        free(in2);
        in2 = nullptr;
    }

    if (out != nullptr)
    {
        free(out);
        out = nullptr;
    }
}

int main()
{
    std::mt19937_64 rng {42};

    const int numElements = 50000;
    std::cout << "[Vector operation on " << numElements << " elements]" << std::endl;

    // Allocate managed space for our inputs and GPU outputs
    // We then fill them with random numbers

    test_type* in1 = nullptr;
    test_type* in2 = nullptr;
    test_type* out = nullptr;

    allocate(in1, numElements);
    allocate(in2, numElements);
    allocate(out, numElements);

    boost::random::uniform_int_distribution<test_type> dist {(std::numeric_limits<test_type>::min)(), (std::numeric_limits<test_type>::max)()};
    for (std::size_t i = 0; i < numElements; ++i)
    {
        in1[i] = dist(rng);
        in2[i] = dist(rng);
    }

    const int threadsPerBlock = 256;
    const int blocksPerGrid = (numElements + threadsPerBlock - 1) / threadsPerBlock;
    std::cout << "CUDA kernel launch with " << blocksPerGrid << " blocks of " << threadsPerBlock << " threads" << std::endl;

    // Launch the CUDA kernel and check for errors

    cuda_gcd<<<blocksPerGrid, threadsPerBlock>>>(in1, in2, out, numElements);
    cudaDeviceSynchronize();

    cudaError_t err = cudaGetLastError();
    if (err != cudaSuccess)
    {
        std::cerr << "Failed to launch kernel (error code " << cudaGetErrorString(err) << ")!" << std::endl;
        cleanup(in1, in2, out);
        return EXIT_FAILURE;
    }

    // We now will perform the same operation using the same inputs on CPU,
    // to compare the results for equality

    std::vector<test_type> results;
    results.reserve(numElements);

    for (int i = 0; i < numElements; ++i)
    {
        results.push_back(boost::int128::gcd(in1[i], in2[i]));
    }

    // We can now compare that our operation on GPU and the same operation on CPU have identical results

    for (int i = 0; i < numElements; ++i)
    {
        if (out[i] != results[i])
        {
            std::cerr << "Result verification failed at element: " << i << "!" << std::endl;
            cleanup(in1, in2, out);
            return EXIT_FAILURE;
        }
    }

    cleanup(in1, in2, out);

    return 0;
}
