//  Copyright Matt Borland 2024 - 2026.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_INT128_ALLOW_SIGN_CONVERSION

#include <iostream>
#include <iomanip>
#include <boost/int128.hpp>
#include <boost/int128/literals.hpp>
#include "cuda_managed_ptr.hpp"
#include "stopwatch.hpp"

// For the CUDA runtime routines (prefixed with "cuda_")
#include <cuda_runtime.h>

using boost::int128::uint128_t;
using namespace boost::int128::literals;

// Number of test cases: we test each literal operator with several values
// Operators:
//   1. operator""_u128(const char*)              - raw literal, decimal
//   2. operator""_U128(const char*)              - raw literal, decimal
//   3. operator""_u128(const char*, size_t)      - cooked string literal
//   4. operator""_U128(const char*, size_t)      - cooked string literal
//   5. operator""_u128(unsigned long long)        - integer literal
//   6. operator""_U128(unsigned long long)        - integer literal

constexpr int NUM_TESTS = 24;

__global__ void cuda_test(uint128_t *out)
{
    int i = threadIdx.x;

    // operator""_u128(const char*) - raw literal
    if (i == 0) { out[i] = 0_u128; }
    if (i == 1) { out[i] = 1_u128; }
    if (i == 2) { out[i] = 18446744073709551615_u128; }
    if (i == 3) { out[i] = 999999999999999999_u128; }

    // operator""_U128(const char*) - raw literal
    if (i == 4) { out[i] = 0_U128; }
    if (i == 5) { out[i] = 1_U128; }
    if (i == 6) { out[i] = 18446744073709551615_U128; }
    if (i == 7) { out[i] = 999999999999999999_U128; }

    // operator""_u128(const char*, size_t) - string literal
    if (i == 8)  { out[i] = "0"_u128; }
    if (i == 9)  { out[i] = "1"_u128; }
    if (i == 10) { out[i] = "340282366920938463463374607431768211455"_u128; }
    if (i == 11) { out[i] = "999999999999999999"_u128; }

    // operator""_U128(const char*, size_t) - string literal
    if (i == 12) { out[i] = "0"_U128; }
    if (i == 13) { out[i] = "1"_U128; }
    if (i == 14) { out[i] = "340282366920938463463374607431768211455"_U128; }
    if (i == 15) { out[i] = "999999999999999999"_U128; }

    // operator""_u128(unsigned long long) - integer literal
    if (i == 16) { out[i] = 0_u128; }
    if (i == 17) { out[i] = 1_u128; }
    if (i == 18) { out[i] = 18446744073709551615_u128; }
    if (i == 19) { out[i] = 42_u128; }

    // operator""_U128(unsigned long long) - integer literal
    if (i == 20) { out[i] = 0_U128; }
    if (i == 21) { out[i] = 1_U128; }
    if (i == 22) { out[i] = 18446744073709551615_U128; }
    if (i == 23) { out[i] = 42_U128; }
}

int main(void)
{
    cudaError_t err = cudaSuccess;

    std::cout << "[Unsigned literal tests: " << NUM_TESTS << " cases]" << std::endl;

    cuda_managed_ptr<uint128_t> output(NUM_TESTS);

    // Launch with 1 block of NUM_TESTS threads
    watch w;

    cuda_test<<<1, NUM_TESTS>>>(output.get());
    cudaDeviceSynchronize();

    std::cout << "CUDA kernal done in: " << w.elapsed() << "s" << std::endl;

    err = cudaGetLastError();
    if (err != cudaSuccess)
    {
        std::cerr << "Failed to launch kernel (error code " << cudaGetErrorString(err) << ")!" << std::endl;
        return EXIT_FAILURE;
    }

    // Build expected values on host using the same literals
    uint128_t expected[NUM_TESTS];

    // operator""_u128(const char*) - raw literal
    expected[0]  = 0_u128;
    expected[1]  = 1_u128;
    expected[2]  = 18446744073709551615_u128;
    expected[3]  = 999999999999999999_u128;

    // operator""_U128(const char*) - raw literal
    expected[4]  = 0_U128;
    expected[5]  = 1_U128;
    expected[6]  = 18446744073709551615_U128;
    expected[7]  = 999999999999999999_U128;

    // operator""_u128(const char*, size_t) - string literal
    expected[8]  = "0"_u128;
    expected[9]  = "1"_u128;
    expected[10] = "340282366920938463463374607431768211455"_u128;
    expected[11] = "999999999999999999"_u128;

    // operator""_U128(const char*, size_t) - string literal
    expected[12] = "0"_U128;
    expected[13] = "1"_U128;
    expected[14] = "340282366920938463463374607431768211455"_U128;
    expected[15] = "999999999999999999"_U128;

    // operator""_u128(unsigned long long) - integer literal
    expected[16] = 0_u128;
    expected[17] = 1_u128;
    expected[18] = 18446744073709551615_u128;
    expected[19] = 42_u128;

    // operator""_U128(unsigned long long) - integer literal
    expected[20] = 0_U128;
    expected[21] = 1_U128;
    expected[22] = 18446744073709551615_U128;
    expected[23] = 42_U128;

    // Verify
    for (int i = 0; i < NUM_TESTS; ++i)
    {
        if (output[i] != expected[i])
        {
            std::cerr << "Result verification failed at test case " << i << "!" << std::endl;
            return EXIT_FAILURE;
        }
    }

    std::cout << "Test PASSED" << std::endl;
    std::cout << "Done\n";

    return 0;
}
