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

using boost::int128::int128_t;
using namespace boost::int128::literals;

// Number of test cases: we test each literal operator with several values
// Operators:
//   1. operator""_i128(const char*)              - raw literal, decimal
//   2. operator""_I128(const char*)              - raw literal, decimal
//   3. operator""_i128(const char*, size_t)      - cooked string literal
//   4. operator""_I128(const char*, size_t)      - cooked string literal
//   5. operator""_i128(unsigned long long)        - integer literal
//   6. operator""_I128(unsigned long long)        - integer literal

constexpr int NUM_TESTS = 30;

__global__ void cuda_test(int128_t *out)
{
    int i = threadIdx.x;

    // operator""_i128(const char*) - raw literal (values must fit unsigned long long to avoid NVCC warnings)
    if (i == 0) { out[i] = 0_i128; }
    if (i == 1) { out[i] = 1_i128; }
    if (i == 2) { out[i] = 18446744073709551615_i128; }
    if (i == 3) { out[i] = 999999999999999999_i128; }
    if (i == 4) { out[i] = 42_i128; }

    // operator""_I128(const char*) - raw literal (values must fit unsigned long long to avoid NVCC warnings)
    if (i == 5) { out[i] = 0_I128; }
    if (i == 6) { out[i] = 1_I128; }
    if (i == 7) { out[i] = 18446744073709551615_I128; }
    if (i == 8) { out[i] = 999999999999999999_I128; }
    if (i == 9) { out[i] = 42_I128; }

    // operator""_i128(const char*, size_t) - string literal (supports negative)
    if (i == 10) { out[i] = "0"_i128; }
    if (i == 11) { out[i] = "1"_i128; }
    if (i == 12) { out[i] = "170141183460469231731687303715884105727"_i128; }
    if (i == 13) { out[i] = "-1"_i128; }
    if (i == 14) { out[i] = "-170141183460469231731687303715884105727"_i128; }

    // operator""_I128(const char*, size_t) - string literal (supports negative)
    if (i == 15) { out[i] = "0"_I128; }
    if (i == 16) { out[i] = "1"_I128; }
    if (i == 17) { out[i] = "170141183460469231731687303715884105727"_I128; }
    if (i == 18) { out[i] = "-1"_I128; }
    if (i == 19) { out[i] = "-170141183460469231731687303715884105727"_I128; }

    // operator""_i128(unsigned long long) - integer literal
    if (i == 20) { out[i] = 0_i128; }
    if (i == 21) { out[i] = 1_i128; }
    if (i == 22) { out[i] = 18446744073709551615_i128; }
    if (i == 23) { out[i] = 42_i128; }
    if (i == 24) { out[i] = 100_i128; }

    // operator""_I128(unsigned long long) - integer literal
    if (i == 25) { out[i] = 0_I128; }
    if (i == 26) { out[i] = 1_I128; }
    if (i == 27) { out[i] = 18446744073709551615_I128; }
    if (i == 28) { out[i] = 42_I128; }
    if (i == 29) { out[i] = 100_I128; }
}

int main(void)
{
    cudaError_t err = cudaSuccess;

    std::cout << "[Signed literal tests: " << NUM_TESTS << " cases]" << std::endl;

    cuda_managed_ptr<int128_t> output(NUM_TESTS);

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
    int128_t expected[NUM_TESTS];

    // operator""_i128(const char*) - raw literal (values must fit unsigned long long to avoid NVCC warnings)
    expected[0]  = 0_i128;
    expected[1]  = 1_i128;
    expected[2]  = 18446744073709551615_i128;
    expected[3]  = 999999999999999999_i128;
    expected[4]  = 42_i128;

    // operator""_I128(const char*) - raw literal (values must fit unsigned long long to avoid NVCC warnings)
    expected[5]  = 0_I128;
    expected[6]  = 1_I128;
    expected[7]  = 18446744073709551615_I128;
    expected[8]  = 999999999999999999_I128;
    expected[9]  = 42_I128;

    // operator""_i128(const char*, size_t) - string literal (supports negative)
    expected[10] = "0"_i128;
    expected[11] = "1"_i128;
    expected[12] = "170141183460469231731687303715884105727"_i128;
    expected[13] = "-1"_i128;
    expected[14] = "-170141183460469231731687303715884105727"_i128;

    // operator""_I128(const char*, size_t) - string literal (supports negative)
    expected[15] = "0"_I128;
    expected[16] = "1"_I128;
    expected[17] = "170141183460469231731687303715884105727"_I128;
    expected[18] = "-1"_I128;
    expected[19] = "-170141183460469231731687303715884105727"_I128;

    // operator""_i128(unsigned long long) - integer literal
    expected[20] = 0_i128;
    expected[21] = 1_i128;
    expected[22] = 18446744073709551615_i128;
    expected[23] = 42_i128;
    expected[24] = 100_i128;

    // operator""_I128(unsigned long long) - integer literal
    expected[25] = 0_I128;
    expected[26] = 1_I128;
    expected[27] = 18446744073709551615_I128;
    expected[28] = 42_I128;
    expected[29] = 100_I128;

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
