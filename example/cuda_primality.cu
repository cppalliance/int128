//  Copyright Matt Borland 2026.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// Deterministic Miller-Rabin primality testing on the GPU.
//
// Every step of the test reduces a product of two residues modulo n. For a
// 64-bit n both operands can approach 2^64, so the product needs a full 128
// bits before it can be reduced. uint128 supplies that intermediate in
// device code, where unsigned __int128 is not portably available.

#include <boost/int128.hpp>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

#include <cuda_runtime.h>

using boost::int128::uint128;

// Number of bases that make Miller-Rabin deterministic for every 64-bit n
constexpr int num_bases {12};

// Computes (a * b) % n for 64-bit operands. The 128-bit product is formed
// first so that no high-order bits are discarded before the reduction.
__host__ __device__ std::uint64_t mulmod(const std::uint64_t a, const std::uint64_t b, const std::uint64_t n) noexcept
{
    return static_cast<std::uint64_t>((uint128{a} * uint128{b}) % uint128{n});
}

// Deterministic Miller-Rabin. Exact for the whole 64-bit range.
__host__ __device__ bool is_prime(const std::uint64_t n) noexcept
{
    const std::uint64_t bases[num_bases] {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};

    for (int i {0}; i < num_bases; ++i)
    {
        if (n == bases[i])
        {
            return true;
        }

        if (n % bases[i] == 0U)
        {
            return false;
        }
    }

    if (n < 41U)
    {
        return false;
    }

    // Write n - 1 as d * 2^s with d odd
    std::uint64_t d {n - 1U};
    int s {0};
    while ((d & 1U) == 0U)
    {
        d >>= 1U;
        ++s;
    }

    for (int i {0}; i < num_bases; ++i)
    {
        // powm keeps every intermediate in a uint128, so a^d mod n is exact
        auto x {static_cast<std::uint64_t>(boost::int128::powm(uint128{bases[i]}, uint128{d}, uint128{n}))};

        if (x == 1U || x == n - 1U)
        {
            continue;
        }

        bool composite {true};
        for (int r {1}; r < s; ++r)
        {
            x = mulmod(x, x, n);

            if (x == n - 1U)
            {
                composite = false;
                break;
            }
        }

        if (composite)
        {
            return false;
        }
    }

    return true;
}

// One candidate per thread. Writes the candidate back when it is prime, 0 otherwise.
__global__ void cuda_find_primes(const std::uint64_t first, std::uint64_t* out, const int numCandidates)
{
    const auto i {static_cast<int>(blockDim.x * blockIdx.x + threadIdx.x)};

    if (i < numCandidates)
    {
        const std::uint64_t candidate {first + 2ULL * static_cast<std::uint64_t>(i)};
        out[i] = is_prime(candidate) ? candidate : 0ULL;
    }
}

// Allocate managed space so that the array can be used on both host and device
void allocate(std::uint64_t** out, int numElements)
{
    const cudaError_t err {cudaMallocManaged(out, numElements * sizeof(std::uint64_t))};
    if (err != cudaSuccess)
    {
        throw std::runtime_error(cudaGetErrorString(err));
    }

    cudaDeviceSynchronize();
}

void cleanup(std::uint64_t** out)
{
    if (*out != nullptr)
    {
        cudaFree(*out);
        *out = nullptr;
    }

    cudaDeviceReset();
}

int main()
{
    // A 64-bit multiply is not enough, even though both operands are below n
    const std::uint64_t a {11400714819323198485ULL};
    const std::uint64_t b {14029467366897019727ULL};
    const std::uint64_t n {18446744073709551557ULL};

    std::cout << "Full 128-bit product a * b: " << uint128{a} * uint128{b} << std::endl;
    std::cout << "  (a * b) % n, 64-bit math: " << (a * b) % n << "  <- the product wrapped" << std::endl;
    std::cout << "  (a * b) % n via uint128: " << mulmod(a, b, n) << std::endl;
    std::cout << std::endl;

    // Test the largest odd values that fit in 64 bits
    const int numCandidates {1 << 16};
    const std::uint64_t last {(std::numeric_limits<std::uint64_t>::max)()};
    const std::uint64_t first {last - 2ULL * static_cast<std::uint64_t>(numCandidates - 1)};

    std::cout << "[Miller-Rabin on " << numCandidates << " odd candidates from " << first << " to " << last << "]" << std::endl;

    std::uint64_t* out {nullptr};
    allocate(&out, numCandidates);

    const int threadsPerBlock {256};
    const int blocksPerGrid {(numCandidates + threadsPerBlock - 1) / threadsPerBlock};
    std::cout << "CUDA kernel launch with " << blocksPerGrid << " blocks of " << threadsPerBlock << " threads" << std::endl;

    cuda_find_primes<<<blocksPerGrid, threadsPerBlock>>>(first, out, numCandidates);
    cudaDeviceSynchronize();

    const cudaError_t err {cudaGetLastError()};
    if (err != cudaSuccess)
    {
        std::cerr << "Failed to launch kernel (error code " << cudaGetErrorString(err) << ")!" << std::endl;
        cleanup(&out);
        return EXIT_FAILURE;
    }

    // Run the identical test on the host for comparison
    std::vector<std::uint64_t> results;
    results.reserve(numCandidates);

    for (int i {0}; i < numCandidates; ++i)
    {
        const std::uint64_t candidate {first + 2ULL * static_cast<std::uint64_t>(i)};
        results.emplace_back(is_prime(candidate) ? candidate : 0ULL);
    }

    for (int i {0}; i < numCandidates; ++i)
    {
        if (out[i] != results[i])
        {
            std::cerr << "Result verification failed at element: " << i << "!" << std::endl;
            cleanup(&out);
            return EXIT_FAILURE;
        }
    }

    int found {0};
    std::uint64_t largest {0};
    for (int i {0}; i < numCandidates; ++i)
    {
        if (out[i] != 0U)
        {
            ++found;
            largest = out[i];
        }
    }

    std::cout << "Primes found on device: " << found << std::endl;
    std::cout << "Largest prime below 2^64: " << largest << std::endl;

    cleanup(&out);

    std::cout << "All CPU and GPU computed elements match!" << std::endl;

    return 0;
}
