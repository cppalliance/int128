// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128/int128.hpp>

#if defined(NDEBUG) && !defined(UBSAN) && !defined(ASAN) && !defined(__SANITIZE_ADDRESS__) && !defined(BOOST_INT128_DISABLE_EXCEPTIONS)
#define BOOST_INT128_BENCHMARK_U128
#endif // NDEBUG

// GCC-5 support breaks down with errors propagating from Boost.Multiprecision
#if defined(BOOST_INT128_BENCHMARK_U128) && defined(__GNUC__) && __GNUC__ == 5
#  undef BOOST_INT128_BENCHMARK_U128
#endif

#include <iostream>

#ifdef BOOST_INT128_BENCHMARK_U128

// This only works when running on latest
#if __has_include(<__msvc_int128.hpp>) && _MSVC_LANG >= 202002L
#include <__msvc_int128.hpp>
#define BOOST_INT128_HAS_MSVC_INTERNAL_I128
#endif

// Abseil requires at least C++17 (at time of writing)
#if __has_include(<absl/numeric/int128.h>) && defined(__cplusplus) && __cplusplus >= 201703L
#  include <absl/numeric/int128.h>
#  ifndef __i386__
#    define BOOST_INT128_BENCHMARK_ABSL
#  endif
#endif

#include <boost/int128/numeric.hpp>
#include "benchmark_results.hpp"
#include <chrono>
#include <random>
#include <vector>
#include <type_traits>
#include <iomanip>
#include <string>
#include <cmath>
#include <cstring>
#include <functional>
#include <numeric>

#if defined(__cpp_lib_gcd_lcm) && __cpp_lib_gcd_lcm >= 201606L && !defined(__STRICT_ANSI__) && !defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
#  define BOOST_INT128_BENCHMARK_BUILTIN_GCD
#endif

// Element and repetition counts. They start at the defaults of the shared
// options and main() refreshes them from --elements and --repetitions.
std::size_t N {bench::config().elements};
unsigned K {bench::config().repetitions};

using namespace std::chrono_literals;

#ifdef __clang__
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wfloat-equal"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#  pragma clang diagnostic ignored "-Wdouble-promotion"
#  pragma clang diagnostic ignored "-Wsign-conversion"
#  pragma clang diagnostic ignored "-Wdeprecated-declarations"
#  pragma clang diagnostic ignored "-Wundef"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wunused-variable"
#  if __clang_major__ >= 20
#    pragma clang diagnostic ignored "-Wdeprecated-missing-comma-variadic-parameter"
#    pragma clang diagnostic ignored "-Wfortify-source"
#  endif
#  define BOOST_INT128_NO_INLINE __attribute__ ((__noinline__))
#elif defined(_MSC_VER)
#  define BOOST_INT128_NO_INLINE __declspec(noinline)
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wfloat-equal"
#  pragma GCC diagnostic ignored "-Wdouble-promotion"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsign-conversion"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wcast-qual"
#  define BOOST_INT128_NO_INLINE __attribute__ ((__noinline__))
#  if __GNUC__ >= 11
#    pragma GCC diagnostic ignored "-Wstringop-overread"
#  endif
#  if __GNUC__ > 6
#    pragma GCC diagnostic ignored "-Wduplicated-branches"
#  endif
#endif

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/random/uniform_int_distribution.hpp>
using mp_u128 = boost::multiprecision::uint128_t;

// Names of the implementations under test. The generated documentation tables and
// the plot legends use these labels verbatim, so each one spells the type the way
// a user would write it.
template <typename T>
const char* impl_label() noexcept;

template <>
const char* impl_label<boost::int128::uint128>() noexcept
{
    return "uint128";
}

template <>
const char* impl_label<mp_u128>() noexcept
{
    return "boost::mp::uint128";
}

#if defined(BOOST_INT128_HAS_INT128)

template <>
const char* impl_label<boost::int128::detail::builtin_u128>() noexcept
{
    return "unsigned __int128";
}

using baseline_type = boost::int128::detail::builtin_u128;

#elif defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)

template <>
const char* impl_label<std::_Unsigned128>() noexcept
{
    return "std::_Unsigned128";
}

using baseline_type = std::_Unsigned128;

#else

// No hardware type on this platform, so Boost.Multiprecision is the reference.
using baseline_type = mp_u128;

#endif

#ifdef BOOST_INT128_BENCHMARK_ABSL

template <>
const char* impl_label<absl::uint128>() noexcept
{
    return "absl::uint128";
}

#endif

// 0 = 2 words
// 1 = 1 word
// 2 = 2 word / 1 word alternating
// 3 = 1 word / 2 word alternating
// 4 = Random width

template <typename T>
T from_uint128(const boost::int128::uint128 value)
{
    return static_cast<T>(value);
}

template <>
mp_u128 from_uint128(const boost::int128::uint128 value)
{
    return static_cast<mp_u128>(value.high) << 64 | value.low;
}

#ifdef BOOST_INT128_HAS_MSVC_INTERNAL_I128

template <>
std::_Unsigned128 from_uint128(const boost::int128::uint128 value)
{
    return static_cast<std::_Unsigned128>(value.high) << static_cast<std::_Unsigned128>(64) | static_cast<std::_Unsigned128>(value.low);
}

#endif

#ifdef BOOST_INT128_BENCHMARK_ABSL

template <>
absl::uint128 from_uint128(const boost::int128::uint128 value)
{
    return static_cast<absl::uint128>(value.high) << 64U | static_cast<absl::uint128>(value.low);
}

#endif

template <int words, typename T>
std::vector<T> generate_random_vector(std::size_t size = N, unsigned seed = 42U)
{
    using boost::int128::uint128;

    if (seed == 0)
    {
        std::random_device rd;
        seed = rd();
    }

    std::mt19937_64 gen(seed);
    std::uniform_int_distribution<std::uint64_t> dist_low(UINT64_C(0), UINT64_MAX);
    std::uniform_int_distribution<std::uint64_t> dist_high(UINT64_C(0), UINT64_MAX);
    std::uniform_int_distribution<std::uint32_t> dist_small(UINT32_C(0), UINT32_MAX);
    std::uniform_int_distribution<int> size_dist(0, 3);

    std::vector<T> result(size);
    for (std::size_t i = 0; i < size; ++i)
    {
        switch (words)
        {
            case 0:
                result[i] = from_uint128<T>(uint128{dist_high(gen), dist_low(gen)});
                break;

            case 1:
                result[i] = from_uint128<T>(uint128{ dist_low(gen) });
                break;

            case 2:
                if (i % 2 == 0)
                {
                    result[i] = from_uint128<T>(uint128{dist_high(gen), dist_low(gen)});
                }
                else
                {
                    result[i] = from_uint128<T>(uint128{dist_low(gen)});
                }
            break;

            case 3:
                if (i % 2 == 1)
                {
                    result[i] = from_uint128<T>(uint128{dist_high(gen), dist_low(gen)});
                }
                else
                {
                    result[i] = from_uint128<T>(uint128{dist_low(gen)});
                }
            break;

            case 4:
                // 32, 64, 96, or 128 bits in length
                switch (size_dist(gen))
                {
                    case 0:
                        result[i] = from_uint128<T>(uint128{dist_low(gen)});
                        break;
                    case 1:
                        result[i] = from_uint128<T>(uint128{dist_high(gen), dist_low(gen)});
                        break;
                    case 2:
                        result[i] = from_uint128<T>(uint128{dist_small(gen), dist_low(gen)});
                        break;
                    case 3:
                        result[i] = from_uint128<T>(uint128{dist_small(gen)});
                        break;
                    default:
                        BOOST_INT128_UNREACHABLE;
                }
            break;
        }
    }

    return result;
}

template <typename T>
BOOST_INT128_NO_INLINE void test_comparisons(const std::vector<T>& data_vec, const char* label)
{
    auto t_total = std::chrono::steady_clock::now();
    auto t1 = std::chrono::steady_clock::now();
    std::size_t s = 0; // discard variable

    for (std::size_t k {}; k < K; ++k)
    {
        for (std::size_t i {}; i < data_vec.size() - 1U; ++i)
        {
            const auto val1 = data_vec[i];
            const auto val2 = data_vec[i + 1];
            s += static_cast<std::uint64_t>(val1 == val2);
        }
    }

    auto t2 = std::chrono::steady_clock::now();
    auto us = bench::elapsed_us(t1, t2);
    bench::record("eq", impl_label<T>(), us);

    std::cerr << "EQ <" << std::left << std::setw(11) << label << ">: " << std::setw( 10 ) << us << " us (s=" << s << ")\n";

    t1 = std::chrono::steady_clock::now();
    s = 0;

    for (std::size_t k {}; k < K; ++k)
    {
        for (std::size_t i {}; i < data_vec.size() - 1U; ++i)
        {
            const auto val1 = data_vec[i];
            const auto val2 = data_vec[i + 1];
            s += static_cast<std::uint64_t>(val1 != val2);
        }
    }

    t2 = std::chrono::steady_clock::now();

    us = bench::elapsed_us(t1, t2);
    bench::record("ne", impl_label<T>(), us);

    std::cerr << "NE <" << std::left << std::setw(11) << label << ">: " << std::setw( 10 ) << us << " us (s=" << s << ")\n";

    t1 = std::chrono::steady_clock::now();
    s = 0;

    for (std::size_t k {}; k < K; ++k)
    {
        for (std::size_t i {}; i < data_vec.size() - 1U; ++i)
        {
            const auto val1 = data_vec[i];
            const auto val2 = data_vec[i + 1];
            s += static_cast<std::uint64_t>(val1 < val2);
        }
    }

    t2 = std::chrono::steady_clock::now();

    us = bench::elapsed_us(t1, t2);
    bench::record("lt", impl_label<T>(), us);

    std::cerr << "LT <" << std::left << std::setw(11) << label << ">: " << std::setw( 10 ) << us << " us (s=" << s << ")\n";

    t1 = std::chrono::steady_clock::now();
    s = 0;

    for (std::size_t k {}; k < K; ++k)
    {
        for (std::size_t i {}; i < data_vec.size() - 1U; ++i)
        {
            const auto val1 = data_vec[i];
            const auto val2 = data_vec[i + 1];
            s += static_cast<std::uint64_t>(val1 <= val2);
        }
    }

    t2 = std::chrono::steady_clock::now();

    us = bench::elapsed_us(t1, t2);
    bench::record("le", impl_label<T>(), us);

    std::cerr << "LE <" << std::left << std::setw(11) << label << ">: " << std::setw( 10 ) << us << " us (s=" << s << ")\n";

    t1 = std::chrono::steady_clock::now();
    s = 0;

    for (std::size_t k {}; k < K; ++k)
    {
        for (std::size_t i {}; i < data_vec.size() - 1U; ++i)
        {
            const auto val1 = data_vec[i];
            const auto val2 = data_vec[i + 1];
            s += static_cast<std::uint64_t>(val1 > val2);
        }
    }

    t2 = std::chrono::steady_clock::now();

    us = bench::elapsed_us(t1, t2);
    bench::record("gt", impl_label<T>(), us);

    std::cerr << "GT <" << std::left << std::setw(11) << label << ">: " << std::setw( 10 ) << us << " us (s=" << s << ")\n";

    t1 = std::chrono::steady_clock::now();
    s = 0;

    for (std::size_t k {}; k < K; ++k)
    {
        for (std::size_t i {}; i < data_vec.size() - 1U; ++i)
        {
            const auto val1 = data_vec[i];
            const auto val2 = data_vec[i + 1];
            s += static_cast<std::uint64_t>(val1 >= val2);
        }
    }

    t2 = std::chrono::steady_clock::now();

    us = bench::elapsed_us(t1, t2);
    bench::record("ge", impl_label<T>(), us);

    std::cerr << "GE <" << std::left << std::setw(11) << label << ">: " << std::setw( 10 ) << us << " us (s=" << s << ")\n";

    // The comparison row of the documentation tables is this total: every
    // relational operator over the whole vector.
    const auto total_us = bench::elapsed_us(t_total, t2);
    bench::record("comparisons", impl_label<T>(), total_us);

    std::cerr << "SUM<" << std::left << std::setw(11) << label << ">: " << std::setw( 10 ) << total_us << " us (s=" << s << ")\n\n";
}

template <typename T, typename Func>
BOOST_INT128_NO_INLINE void test_two_element_operation(const std::vector<T>& data_vec, Func op, const char* operation, const char* type)
{
    const auto t1 = std::chrono::steady_clock::now();
    std::uint64_t s = 0; // discard variable

    for (std::size_t k {}; k < K; ++k)
    {
        for (std::size_t i {}; i < data_vec.size() - 1U; ++i)
        {
            const auto val1 = data_vec[i];
            const auto val2 = data_vec[i + 1];
            s += static_cast<std::uint64_t>((op(val1, val2)));
        }
    }

    const auto t2 = std::chrono::steady_clock::now();
    const auto us = bench::elapsed_us(t1, t2);
    bench::record(operation, impl_label<T>(), us);

    std::cerr << operation << "<" << std::left << std::setw(11) << type << ">: " << std::setw( 10 ) << us << " us (s=" << s << ")\n";
}

// Benchmarks the narrow division overloads (128-bit divided by a 64-bit or 32-bit value),
// which exercise the hardware-accelerated one_word_div path rather than the full 128/128 divide.
template <bool HalfWord, typename T>
BOOST_INT128_NO_INLINE void test_narrow_division(const std::vector<T>& data_vec, const char* operation, const char* type)
{
    const auto t1 = std::chrono::steady_clock::now();
    std::uint64_t s = 0; // discard variable

    for (std::size_t k {}; k < K; ++k)
    {
        for (std::size_t i {}; i < data_vec.size() - 1U; ++i)
        {
            if (HalfWord)
            {
                const auto divisor = static_cast<std::uint32_t>(data_vec[i + 1]) | 1U;
                s += static_cast<std::uint64_t>(data_vec[i] / divisor);
            }
            else
            {
                const auto divisor = static_cast<std::uint64_t>(data_vec[i + 1]) | UINT64_C(1);
                s += static_cast<std::uint64_t>(data_vec[i] / divisor);
            }
        }
    }

    const auto t2 = std::chrono::steady_clock::now();
    const auto us = bench::elapsed_us(t1, t2);
    bench::record(operation, impl_label<T>(), us);

    std::cerr << operation << "<" << std::left << std::setw(11) << type << ">: " << std::setw( 10 ) << us << " us (s=" << s << ")\n";
}

template <typename T>
BOOST_INT128_NO_INLINE void test_gcd(const std::vector<T>& data_vec, const char* type)
{
    #ifdef BOOST_INT128_BENCHMARK_BUILTIN_GCD
    using std::gcd;
    #endif

    using boost::multiprecision::gcd;
    using boost::int128::gcd;

    const auto t1 = std::chrono::steady_clock::now();
    std::uint64_t s = 0; // discard variable

    for (std::size_t k {}; k < K; ++k)
    {
        for (std::size_t i {}; i < data_vec.size() - 1U; ++i)
        {
            const auto val1 = data_vec[i];
            const auto val2 = data_vec[i + 1];
            s += static_cast<std::uint64_t>(gcd(val1, val2));
        }
    }

    const auto t2 = std::chrono::steady_clock::now();

    std::cerr << "gcd" << "<" << std::left << std::setw(11) << type << ">: " << std::setw( 10 ) << ( t2 - t1 ) / 1us << " us (s=" << s << ")\n";
}

std::vector<int> generate_shift_vector()
{
    std::random_device rd;
    const auto seed = rd();
    std::mt19937_64 gen(seed);
    std::uniform_int_distribution<int> dist(0, 127);

    std::vector<int> data_vec;
    data_vec.reserve(N);
    for (std::size_t i {}; i < N; ++i)
    {
        data_vec.emplace_back(dist(gen));
    }

    return data_vec;
}

template <typename T>
BOOST_INT128_NO_INLINE void test_right_shift(const std::vector<T>& data_vec, const std::vector<int>& shift_vector, const char* type)
{
    const auto t1 = std::chrono::steady_clock::now();
    std::uint64_t s = 0; // discard variable

    for (std::size_t k {}; k < K; ++k)
    {
        for (std::size_t i {}; i < data_vec.size(); ++i)
        {
            s += static_cast<std::uint64_t>(data_vec[i] >> shift_vector[i]);
        }
    }

    const auto t2 = std::chrono::steady_clock::now();
    const auto us = bench::elapsed_us(t1, t2);

    bench::record("shr", impl_label<T>(), us);

    std::cerr << "rs" << " <" << std::left << std::setw(11) << type << ">: " << std::setw( 10 ) << us << " us (s=" << s << ")\n";
}

template <typename T>
BOOST_INT128_NO_INLINE void test_left_shift(const std::vector<T>& data_vec, const std::vector<int>& shift_vector, const char* type)
{
    const auto t1 = std::chrono::steady_clock::now();
    std::uint64_t s = 0; // discard variable

    for (std::size_t k {}; k < K; ++k)
    {
        for (std::size_t i {}; i < data_vec.size(); ++i)
        {
            s += static_cast<std::uint64_t>(data_vec[i] << shift_vector[i]);
        }
    }

    const auto t2 = std::chrono::steady_clock::now();
    const auto us = bench::elapsed_us(t1, t2);

    bench::record("shl", impl_label<T>(), us);

    std::cerr << "ls" << " <" << std::left << std::setw(11) << type << ">: " << std::setw( 10 ) << us << " us (s=" << s << ")\n";
}

enum class bitwise_operation
{
    b_and,
    b_or,
    b_xor
};

// Name of one bitwise operation in the data set.
const char* bitwise_key(const bitwise_operation op) noexcept
{
    switch (op)
    {
        case bitwise_operation::b_and:
            return "and";
        case bitwise_operation::b_or:
            return "or";
        case bitwise_operation::b_xor:
            return "xor";
    }

    return "unknown";
}

template <typename T>
BOOST_INT128_NO_INLINE void test_operator_bitwise(const std::vector<T>& data_vec, const bitwise_operation op, const char* type)
{
    const auto t1 = std::chrono::steady_clock::now();
    std::uint64_t s = 0; // discard variable

    switch (op)
    {
        case bitwise_operation::b_and:
            for (std::size_t k {}; k < K; ++k)
            {
                for (std::size_t i {}; i < data_vec.size() - 1; ++i)
                {
                    s += static_cast<std::uint64_t>(data_vec[i] & data_vec[i + 1]);
                }
            }
            break;
        case bitwise_operation::b_or:
            for (std::size_t k {}; k < K; ++k)
            {
                for (std::size_t i {}; i < data_vec.size() - 1; ++i)
                {
                    s += static_cast<std::uint64_t>(data_vec[i] | data_vec[i + 1]);
                }
            }
            break;
        case bitwise_operation::b_xor:
            for (std::size_t k {}; k < K; ++k)
            {
                for (std::size_t i {}; i < data_vec.size() - 1; ++i)
                {
                    s += static_cast<std::uint64_t>(data_vec[i] ^ data_vec[i + 1]);
                }
            }
            break;
    }

    const auto t2 = std::chrono::steady_clock::now();
    const auto us = bench::elapsed_us(t1, t2);
    bench::record(bitwise_key(op), impl_label<T>(), us);

    switch (op)
    {
        case bitwise_operation::b_and:
            std::cerr << "and";
            break;
        case bitwise_operation::b_or:
            std::cerr << "or ";
            break;
        case bitwise_operation::b_xor:
            std::cerr << "xor";
            break;
    }

    std::cerr << " <" << std::left << std::setw(11) << type << ">: " << std::setw( 10 ) << us << " us (s=" << s << ")\n";
}

int main(int argc, char* argv[])
{
    using namespace boost::int128::detail;

    bench::parse_options(argc, argv);
    N = bench::config().elements;
    K = bench::config().repetitions;

    const auto shift_vector = generate_shift_vector();

    // Two word operations
    {
        bench::set_group("two_word");

        std::cerr << "\n---------------------------\n";
        std::cerr << "Two Word Operations\n";
        std::cerr << "---------------------------\n\n";

        const auto library_vector = generate_random_vector<0, boost::int128::uint128>();
        const auto mp_vector = generate_random_vector<0, mp_u128>();

        #if defined(BOOST_INT128_HAS_INT128)

        const auto builtin_vector = generate_random_vector<0, builtin_u128>();
        test_comparisons(builtin_vector, "builtin");

        #elif defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)

        const auto builtin_vector = generate_random_vector<0, std::_Unsigned128>();
        test_comparisons(builtin_vector, "builtin");

        #endif

        test_comparisons(library_vector, "library");
        test_comparisons(mp_vector, "mp::u128");

        #ifdef BOOST_INT128_BENCHMARK_ABSL

        const auto absl_vector = generate_random_vector<0, absl::uint128>();
        test_comparisons(absl_vector, "absl::u128");

        #endif

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::plus<>(), "add", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::plus<>(), "add", "Library");
        test_two_element_operation(mp_vector, std::plus<>(), "add", "mp::u128");

        #ifdef BOOST_INT128_BENCHMARK_ABSL
        test_two_element_operation(absl_vector, std::plus<>(), "add", "absl::u128");
        #endif

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::minus<>(), "sub", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::minus<>(), "sub", "Library");
        test_two_element_operation(mp_vector, std::minus<>(), "sub", "mp::u128");

        #ifdef BOOST_INT128_BENCHMARK_ABSL
        test_two_element_operation(absl_vector, std::minus<>(), "sub", "absl::u128");
        #endif

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::multiplies<>(), "mul", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::multiplies<>(), "mul", "Library");
        test_two_element_operation(mp_vector, std::multiplies<>(), "mul", "mp::u128");

        #ifdef BOOST_INT128_BENCHMARK_ABSL
        test_two_element_operation(absl_vector, std::multiplies<>(), "mul", "absl::u128");
        #endif

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::divides<>(), "div", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::divides<>(), "div", "Library");
        test_two_element_operation(mp_vector, std::divides<>(), "div", "mp::u128");

        #ifdef BOOST_INT128_BENCHMARK_ABSL
        test_two_element_operation(absl_vector, std::divides<>(), "div", "absl::u128");
        #endif

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::modulus<>(), "mod", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::modulus<>(), "mod", "Library");
        test_two_element_operation(mp_vector, std::modulus<>(), "mod", "mp::u128");

        #ifdef BOOST_INT128_BENCHMARK_ABSL
        test_two_element_operation(absl_vector, std::modulus<>(), "mod", "absl::u128");
        #endif

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_narrow_division<false>(builtin_vector, "div64", "Builtin");
        #endif

        test_narrow_division<false>(library_vector, "div64", "Library");
        test_narrow_division<false>(mp_vector, "div64", "mp::u128");

        #ifdef BOOST_INT128_BENCHMARK_ABSL
        test_narrow_division<false>(absl_vector, "div64", "absl::u128");
        #endif

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_narrow_division<true>(builtin_vector, "div32", "Builtin");
        #endif

        test_narrow_division<true>(library_vector, "div32", "Library");
        test_narrow_division<true>(mp_vector, "div32", "mp::u128");

        #ifdef BOOST_INT128_BENCHMARK_ABSL
        test_narrow_division<true>(absl_vector, "div32", "absl::u128");
        #endif

        std::cerr << std::endl;

        #if (defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)) && defined(BOOST_INT128_BENCHMARK_BUILTIN_GCD)
        //test_gcd(builtin_vector, "Builtin");
        #endif

        //test_gcd(library_vector,  "Library");
        //test_gcd(mp_vector,  "mp::u128");

        //std::cerr << std::endl;
    }
    // Single word operations
    {
        bench::set_group("one_word");

        std::cerr << "\n---------------------------\n";
        std::cerr << "One Word Operations\n";
        std::cerr << "---------------------------\n\n";

        const auto library_vector = generate_random_vector<1, boost::int128::uint128>();
        const auto mp_vector = generate_random_vector<1, mp_u128>();

        #if defined(BOOST_INT128_HAS_INT128)

        const auto builtin_vector = generate_random_vector<1, builtin_u128>();
        test_comparisons(builtin_vector, "builtin");

        #elif defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)

        const auto builtin_vector = generate_random_vector<1, std::_Unsigned128>();
        test_comparisons(builtin_vector, "builtin");

        #endif

        test_comparisons(library_vector, "library");
        test_comparisons(mp_vector, "mp::u128");

        #ifdef BOOST_INT128_BENCHMARK_ABSL

        const auto absl_vector = generate_random_vector<1, absl::uint128>();
        test_comparisons(absl_vector, "absl::u128");

        #endif

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::plus<>(), "add", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::plus<>(), "add", "Library");
        test_two_element_operation(mp_vector, std::plus<>(), "add", "mp::u128");

        #ifdef BOOST_INT128_BENCHMARK_ABSL
        test_two_element_operation(absl_vector, std::plus<>(), "add", "absl::u128");
        #endif

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::minus<>(), "sub", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::minus<>(), "sub", "Library");
        test_two_element_operation(mp_vector, std::minus<>(), "sub", "mp::u128");

        #ifdef BOOST_INT128_BENCHMARK_ABSL
        test_two_element_operation(absl_vector, std::minus<>(), "sub", "absl::u128");
        #endif

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::multiplies<>(), "mul", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::multiplies<>(), "mul", "Library");
        test_two_element_operation(mp_vector, std::multiplies<>(), "mul", "mp::u128");

        #ifdef BOOST_INT128_BENCHMARK_ABSL
        test_two_element_operation(absl_vector, std::multiplies<>(), "mul", "absl::u128");
        #endif

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::divides<>(), "div", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::divides<>(), "div", "Library");
        test_two_element_operation(mp_vector, std::divides<>(), "div", "mp::u128");

        #ifdef BOOST_INT128_BENCHMARK_ABSL
        test_two_element_operation(absl_vector, std::divides<>(), "div", "absl::u128");
        #endif

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::modulus<>(), "mod", "Builtin");
        #endif

        #ifdef BOOST_INT128_BENCHMARK_ABSL

        #endif

        test_two_element_operation(library_vector, std::modulus<>(), "mod", "Library");
        test_two_element_operation(mp_vector, std::modulus<>(), "mod", "mp::u128");

        std::cerr << std::endl;
    }
    {
        // Two word and one word operations Even = 2, odd = 1

        bench::set_group("two_one_word");

        std::cerr << "\n---------------------------\n";
        std::cerr << "Two-One Word Operations\n";
        std::cerr << "---------------------------\n\n";

        const auto library_vector = generate_random_vector<2, boost::int128::uint128>();
        const auto mp_vector = generate_random_vector<2, mp_u128>();

        #if defined(BOOST_INT128_HAS_INT128)

        const auto builtin_vector = generate_random_vector<2, builtin_u128>();
        test_comparisons(builtin_vector, "builtin");

        #elif defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)

        const auto builtin_vector = generate_random_vector<2, std::_Unsigned128>();
        test_comparisons(builtin_vector, "builtin");

        #endif

        test_comparisons(library_vector, "library");
        test_comparisons(mp_vector, "mp::u128");

        #ifdef BOOST_INT128_BENCHMARK_ABSL
        const auto absl_vector = generate_random_vector<2, absl::uint128>();
        test_comparisons(absl_vector, "absl::u128");
        #endif

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::plus<>(), "add", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::plus<>(), "add", "Library");
        test_two_element_operation(mp_vector, std::plus<>(), "add", "mp::u128");

        #ifdef BOOST_INT128_BENCHMARK_ABSL
        test_two_element_operation(absl_vector, std::plus<>(), "add", "absl::u128");
        #endif

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::minus<>(), "sub", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::minus<>(), "sub", "Library");
        test_two_element_operation(mp_vector, std::minus<>(), "sub", "mp::u128");

        #ifdef BOOST_INT128_BENCHMARK_ABSL
        test_two_element_operation(absl_vector, std::minus<>(), "sub", "absl::u128");
        #endif

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::multiplies<>(), "mul", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::multiplies<>(), "mul", "Library");
        test_two_element_operation(mp_vector, std::multiplies<>(), "mul", "mp::u128");

        #ifdef BOOST_INT128_BENCHMARK_ABSL
        test_two_element_operation(absl_vector, std::multiplies<>(), "mul", "absl::u128");
        #endif

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::divides<>(), "div", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::divides<>(), "div", "Library");
        test_two_element_operation(mp_vector, std::divides<>(), "div", "mp::u128");

        #ifdef BOOST_INT128_BENCHMARK_ABSL
        test_two_element_operation(absl_vector, std::divides<>(), "div", "absl::u128");
        #endif

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::modulus<>(), "mod", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::modulus<>(), "mod", "Library");
        test_two_element_operation(mp_vector, std::modulus<>(), "mod", "mp::u128");

        #ifdef BOOST_INT128_BENCHMARK_ABSL
        test_two_element_operation(absl_vector, std::modulus<>(), "mod", "absl::u128");
        #endif

        std::cerr << std::endl;
    }
    {
        // Two word and one word operations Even = 1, odd = 2

        bench::set_group("one_two_word");

        std::cerr << "\n---------------------------\n";
        std::cerr << "One-Two Word Operations\n";
        std::cerr << "---------------------------\n\n";

        const auto library_vector = generate_random_vector<3, boost::int128::uint128>();
        const auto mp_vector = generate_random_vector<3, mp_u128>();

        #if defined(BOOST_INT128_HAS_INT128)

        const auto builtin_vector = generate_random_vector<3, builtin_u128>();
        test_comparisons(builtin_vector, "builtin");

        #elif defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)

        const auto builtin_vector = generate_random_vector<3, std::_Unsigned128>();
        test_comparisons(builtin_vector, "builtin");

        #endif

        test_comparisons(library_vector, "library");
        test_comparisons(mp_vector, "mp::u128");

        #ifdef BOOST_INT128_BENCHMARK_ABSL
        const auto absl_vector = generate_random_vector<3, absl::uint128>();
        #endif

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::plus<>(), "add", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::plus<>(), "add", "Library");
        test_two_element_operation(mp_vector, std::plus<>(), "add", "mp::u128");

        #ifdef BOOST_INT128_BENCHMARK_ABSL
        test_two_element_operation(absl_vector, std::plus<>(), "add", "absl::u128");
        #endif

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::minus<>(), "sub", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::minus<>(), "sub", "Library");
        test_two_element_operation(mp_vector, std::minus<>(), "sub", "mp::u128");

        #ifdef BOOST_INT128_BENCHMARK_ABSL
        test_two_element_operation(absl_vector, std::minus<>(), "sub", "absl::u128");
        #endif

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::multiplies<>(), "mul", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::multiplies<>(), "mul", "Library");
        test_two_element_operation(mp_vector, std::multiplies<>(), "mul", "mp::u128");

        #ifdef BOOST_INT128_BENCHMARK_ABSL
        test_two_element_operation(absl_vector, std::multiplies<>(), "mul", "absl::u128");
        #endif

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::divides<>(), "div", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::divides<>(), "div", "Library");
        test_two_element_operation(mp_vector, std::divides<>(), "div", "mp::u128");

        #ifdef BOOST_INT128_BENCHMARK_ABSL
        test_two_element_operation(absl_vector, std::divides<>(), "div", "absl::u128");
        #endif

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::modulus<>(), "mod", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::modulus<>(), "mod", "Library");
        test_two_element_operation(mp_vector, std::modulus<>(), "mod", "mp::u128");

        #ifdef BOOST_INT128_BENCHMARK_ABSL
        test_two_element_operation(absl_vector, std::modulus<>(), "mod", "absl::u128");
        #endif

        std::cerr << std::endl;
    }
    {
        // Two word and one word operations Even = 1, odd = 2

        bench::set_group("random_width");

        std::cerr << "\n---------------------------\n";
        std::cerr << "Random Width Operations\n";
        std::cerr << "---------------------------\n\n";

        const auto library_vector = generate_random_vector<4, boost::int128::uint128>();
        const auto mp_vector = generate_random_vector<4, mp_u128>();

        #if defined(BOOST_INT128_HAS_INT128)

        const auto builtin_vector = generate_random_vector<4, builtin_u128>();
        test_comparisons(builtin_vector, "builtin");

        #elif defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)

        const auto builtin_vector = generate_random_vector<4, std::_Unsigned128>();
        test_comparisons(builtin_vector, "builtin");

        #endif

        test_comparisons(library_vector, "library");
        test_comparisons(mp_vector, "mp::u128");

        #ifdef BOOST_INT128_BENCHMARK_ABSL

        const auto absl_vector = generate_random_vector<4, absl::uint128>();
        test_comparisons(absl_vector, "absl::u128");

        #endif

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::plus<>(), "add", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::plus<>(), "add", "Library");
        test_two_element_operation(mp_vector, std::plus<>(), "add", "mp::u128");

        #ifdef BOOST_INT128_BENCHMARK_ABSL
        test_two_element_operation(absl_vector, std::plus<>(), "add", "absl::u128");
        #endif

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::minus<>(), "sub", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::minus<>(), "sub", "Library");
        test_two_element_operation(mp_vector, std::minus<>(), "sub", "mp::u128");

        #ifdef BOOST_INT128_BENCHMARK_ABSL
        test_two_element_operation(absl_vector, std::minus<>(), "sub", "absl::u128");
        #endif

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::multiplies<>(), "mul", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::multiplies<>(), "mul", "Library");
        test_two_element_operation(mp_vector, std::multiplies<>(), "mul", "mp::u128");

        #ifdef BOOST_INT128_BENCHMARK_ABSL
        test_two_element_operation(absl_vector, std::multiplies<>(), "mul", "absl::u128");
        #endif

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::divides<>(), "div", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::divides<>(), "div", "Library");
        test_two_element_operation(mp_vector, std::divides<>(), "div", "mp::u128");

        #ifdef BOOST_INT128_BENCHMARK_ABSL
        test_two_element_operation(absl_vector, std::divides<>(), "div", "absl::u128");
        #endif
        
        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::modulus<>(), "mod", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::modulus<>(), "mod", "Library");
        test_two_element_operation(mp_vector, std::modulus<>(), "mod", "mp::u128");

        #ifdef BOOST_INT128_BENCHMARK_ABSL
        test_two_element_operation(absl_vector, std::modulus<>(), "mod", "absl::u128");
        #endif

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_left_shift(builtin_vector, shift_vector, "Builtin");
        #endif

        test_left_shift(library_vector, shift_vector, "Library");
        test_left_shift(mp_vector, shift_vector, "mp::u128");

        #ifdef BOOST_INT128_BENCHMARK_ABSL
        test_left_shift(absl_vector, shift_vector, "absl::u128");
        #endif

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_right_shift(builtin_vector, shift_vector, "Builtin");
        #endif

        test_right_shift(library_vector, shift_vector, "Library");
        test_right_shift(mp_vector, shift_vector, "mp::u128");

        #ifdef BOOST_INT128_BENCHMARK_ABSL
        test_right_shift(absl_vector, shift_vector, "absl::u128");
        #endif

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_operator_bitwise(builtin_vector, bitwise_operation::b_and, "Builtin");
        #endif

        test_operator_bitwise(library_vector, bitwise_operation::b_and, "Library");
        test_operator_bitwise(mp_vector, bitwise_operation::b_and, "mp::u128");

        #ifdef BOOST_INT128_BENCHMARK_ABSL
        test_operator_bitwise(absl_vector, bitwise_operation::b_and, "absl::u128");
        #endif

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_operator_bitwise(builtin_vector, bitwise_operation::b_or, "Builtin");
        #endif

        test_operator_bitwise(library_vector, bitwise_operation::b_or, "Library");
        test_operator_bitwise(mp_vector, bitwise_operation::b_or, "mp::u128");

        #ifdef BOOST_INT128_BENCHMARK_ABSL
        test_operator_bitwise(absl_vector, bitwise_operation::b_or, "absl::u128");
        #endif

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_operator_bitwise(builtin_vector, bitwise_operation::b_xor, "Builtin");
        #endif

        test_operator_bitwise(library_vector, bitwise_operation::b_xor, "Library");
        test_operator_bitwise(mp_vector, bitwise_operation::b_xor, "mp::u128");

        #ifdef BOOST_INT128_BENCHMARK_ABSL
        test_operator_bitwise(absl_vector, bitwise_operation::b_xor, "absl::u128");
        #endif

        std::cerr << std::endl;

    }

    bench::write_json(bench::metadata{"uint128", "u128", impl_label<baseline_type>()});

    // The Jamfile declares this target with run-fail, so a successful run reports 1.
    return 1;
}

#else // No benchmarks

int main()
{
    std::cerr << "Benchmarks Not Run" << std::endl;
    return 1;
}

#endif
