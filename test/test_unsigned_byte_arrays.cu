//  Copyright Matt Borland 2026.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <vector>
#include <random>
#include <limits>
#include <array>
#include <cstdint>
#include <boost/int128.hpp>
#include <boost/int128/byte_conversions.hpp>
#include <boost/int128/random.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include "cuda_managed_ptr.hpp"
#include "stopwatch.hpp"

#include <cuda_runtime.h>

using test_type = boost::int128::uint128;
using bytes_type = std::array<std::uint8_t, sizeof(test_type)>;

// The byte arrays are written by assignment and read through a raw pointer, so no
// std::array member function is called from device code
__global__ void cuda_test(const test_type *in, const std::uint8_t *in_bytes,
                          bytes_type *out_be, bytes_type *out_le, bytes_type *out_ne,
                          test_type *from_be, test_type *from_le, test_type *from_ne, int numElements)
{
    int i = blockDim.x * blockIdx.x + threadIdx.x;

    if (i < numElements)
    {
        out_be[i] = boost::int128::to_be_bytes(in[i]);
        out_le[i] = boost::int128::to_le_bytes(in[i]);
        out_ne[i] = boost::int128::to_ne_bytes(in[i]);

        const std::uint8_t *bytes = in_bytes + i * static_cast<int>(sizeof(test_type));
        from_be[i] = boost::int128::from_be_bytes<test_type>(bytes);
        from_le[i] = boost::int128::from_le_bytes<test_type>(bytes);
        from_ne[i] = boost::int128::from_ne_bytes<test_type>(bytes);
    }
}

int main(void)
{
    std::mt19937_64 rng {42};

    cudaError_t err = cudaSuccess;

    int numElements = 50000;
    std::cout << "[Vector operation on " << numElements << " elements]" << std::endl;

    cuda_managed_ptr<test_type> input_vector(numElements);
    cuda_managed_ptr<std::uint8_t> input_bytes(numElements * sizeof(test_type));
    cuda_managed_ptr<bytes_type> output_be(numElements);
    cuda_managed_ptr<bytes_type> output_le(numElements);
    cuda_managed_ptr<bytes_type> output_ne(numElements);
    cuda_managed_ptr<test_type> output_from_be(numElements);
    cuda_managed_ptr<test_type> output_from_le(numElements);
    cuda_managed_ptr<test_type> output_from_ne(numElements);

    boost::random::uniform_int_distribution<test_type> dist {test_type{0U}, (std::numeric_limits<test_type>::max)()};
    boost::random::uniform_int_distribution<unsigned> byte_dist {0U, 255U};
    for (std::size_t i = 0; i < numElements; ++i)
    {
        input_vector[i] = dist(rng);
    }
    for (std::size_t i = 0; i < numElements * sizeof(test_type); ++i)
    {
        input_bytes[i] = static_cast<std::uint8_t>(byte_dist(rng));
    }

    int threadsPerBlock = 256;
    int blocksPerGrid = (numElements + threadsPerBlock - 1) / threadsPerBlock;
    std::cout << "CUDA kernel launch with " << blocksPerGrid << " blocks of " << threadsPerBlock << " threads" << std::endl;

    watch w;

    cuda_test<<<blocksPerGrid, threadsPerBlock>>>(input_vector.get(), input_bytes.get(),
                                                  output_be.get(), output_le.get(), output_ne.get(),
                                                  output_from_be.get(), output_from_le.get(), output_from_ne.get(),
                                                  numElements);
    cudaDeviceSynchronize();

    std::cout << "CUDA kernal done in: " << w.elapsed() << "s" << std::endl;

    err = cudaGetLastError();
    if (err != cudaSuccess)
    {
        std::cerr << "Failed to launch kernel (error code " << cudaGetErrorString(err) << ")!" << std::endl;
        return EXIT_FAILURE;
    }

    const std::uint8_t *host_bytes = input_bytes.get();

    w.reset();
    std::vector<bytes_type> be_results;
    std::vector<bytes_type> le_results;
    std::vector<bytes_type> ne_results;
    be_results.reserve(numElements);
    le_results.reserve(numElements);
    ne_results.reserve(numElements);
    for (int i = 0; i < numElements; ++i)
    {
        be_results.push_back(boost::int128::to_be_bytes(input_vector[i]));
        le_results.push_back(boost::int128::to_le_bytes(input_vector[i]));
        ne_results.push_back(boost::int128::to_ne_bytes(input_vector[i]));
    }
    double t = w.elapsed();

    for (int i = 0; i < numElements; ++i)
    {
        if (output_be[i] != be_results[i] || output_le[i] != le_results[i] || output_ne[i] != ne_results[i])
        {
            std::cerr << "Result verification failed at element " << i << "!" << std::endl;
            return EXIT_FAILURE;
        }

        const std::uint8_t *bytes = host_bytes + i * static_cast<int>(sizeof(test_type));

        if (output_from_be[i] != boost::int128::from_be_bytes<test_type>(bytes) ||
            output_from_le[i] != boost::int128::from_le_bytes<test_type>(bytes) ||
            output_from_ne[i] != boost::int128::from_ne_bytes<test_type>(bytes))
        {
            std::cerr << "Result verification failed at element " << i << "!" << std::endl;
            return EXIT_FAILURE;
        }

        // Reading back what was written recovers the value. The pointer overload is used
        // rather than the std::array one because nvcc instantiates the device side of a
        // __host__ __device__ template even for a host call, and the std::array overload
        // reads its argument through std::array::operator[], a constexpr host function.
        if (boost::int128::from_be_bytes<test_type>(output_be[i].data()) != input_vector[i] ||
            boost::int128::from_le_bytes<test_type>(output_le[i].data()) != input_vector[i] ||
            boost::int128::from_ne_bytes<test_type>(output_ne[i].data()) != input_vector[i])
        {
            std::cerr << "Round trip verification failed at element " << i << "!" << std::endl;
            return EXIT_FAILURE;
        }
    }

    std::cout << "Test PASSED, normal calculation time: " << t << "s" << std::endl;
    std::cout << "Done\n";

    return 0;
}
