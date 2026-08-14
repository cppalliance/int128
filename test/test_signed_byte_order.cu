//  Copyright Matt Borland 2026.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <vector>
#include <random>
#include <limits>
#include <boost/int128.hpp>
#include <boost/int128/byte_conversions.hpp>
#include <boost/int128/random.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include "cuda_managed_ptr.hpp"
#include "stopwatch.hpp"

#include <cuda_runtime.h>

using test_type = boost::int128::int128;

__global__ void cuda_test(const test_type *in, test_type *out_be, test_type *out_le, test_type *out_round, int numElements)
{
    int i = blockDim.x * blockIdx.x + threadIdx.x;

    if (i < numElements)
    {
        out_be[i] = boost::int128::to_be(in[i]);
        out_le[i] = boost::int128::to_le(in[i]);

        // Both directions on the device, which must recover the input
        out_round[i] = boost::int128::from_be(boost::int128::to_be(in[i]));
    }
}

int main(void)
{
    std::mt19937_64 rng {42};

    cudaError_t err = cudaSuccess;

    int numElements = 50000;
    std::cout << "[Vector operation on " << numElements << " elements]" << std::endl;

    cuda_managed_ptr<test_type> input_vector(numElements);
    cuda_managed_ptr<test_type> output_be(numElements);
    cuda_managed_ptr<test_type> output_le(numElements);
    cuda_managed_ptr<test_type> output_round(numElements);

    boost::random::uniform_int_distribution<test_type> dist {(std::numeric_limits<test_type>::min)(), (std::numeric_limits<test_type>::max)()};
    for (std::size_t i = 0; i < numElements; ++i)
    {
        input_vector[i] = dist(rng);
    }

    int threadsPerBlock = 256;
    int blocksPerGrid = (numElements + threadsPerBlock - 1) / threadsPerBlock;
    std::cout << "CUDA kernel launch with " << blocksPerGrid << " blocks of " << threadsPerBlock << " threads" << std::endl;

    watch w;

    cuda_test<<<blocksPerGrid, threadsPerBlock>>>(input_vector.get(), output_be.get(), output_le.get(), output_round.get(), numElements);
    cudaDeviceSynchronize();

    std::cout << "CUDA kernal done in: " << w.elapsed() << "s" << std::endl;

    err = cudaGetLastError();
    if (err != cudaSuccess)
    {
        std::cerr << "Failed to launch kernel (error code " << cudaGetErrorString(err) << ")!" << std::endl;
        return EXIT_FAILURE;
    }

    std::vector<test_type> be_results;
    std::vector<test_type> le_results;
    be_results.reserve(numElements);
    le_results.reserve(numElements);
    w.reset();
    for (int i = 0; i < numElements; ++i)
    {
        be_results.push_back(boost::int128::to_be(input_vector[i]));
        le_results.push_back(boost::int128::to_le(input_vector[i]));
    }
    double t = w.elapsed();

    for (int i = 0; i < numElements; ++i)
    {
        if (output_be[i] != be_results[i] || output_le[i] != le_results[i] || output_round[i] != input_vector[i])
        {
            std::cerr << "Result verification failed at element " << i << "!" << std::endl;
            return EXIT_FAILURE;
        }
    }

    std::cout << "Test PASSED, normal calculation time: " << t << "s" << std::endl;
    std::cout << "Done\n";

    return 0;
}
