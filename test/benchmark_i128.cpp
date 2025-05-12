// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#if defined(NDEBUG) && !defined(UBSAN) && !defined(ASAN) && !defined(__SANITIZE_ADDRESS__)
#define BOOST_INT128_BENCHMARK_I128
#endif // NDEBUG

#include <iostream>

#ifdef BOOST_INT128_BENCHMARK_I128

// This only works when running on latest
#if __has_include(<__msvc_int128.hpp>) && _MSVC_LANG >= 202002L
#include <__msvc_int128.hpp>
#define BOOST_INT128_HAS_MSVC_INTERNAL_I128
#endif

#include <boost/int128/int128.hpp>
#include <chrono>
#include <random>
#include <vector>
#include <type_traits>
#include <iomanip>
#include <string>
#include <cmath>
#include <cstring>
#include <functional>

constexpr unsigned N = 20'000'000;
constexpr unsigned K = 5;

using namespace boost::int128;
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
#  pragma GCC diagnostic ignored "-Wduplicated-branches"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wcast-qual"
#  define BOOST_INT128_NO_INLINE __attribute__ ((__noinline__))
#  if __GNUC__ >= 11
#    pragma GCC diagnostic ignored "-Wstringop-overread"
#  endif
#endif

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/random/uniform_int_distribution.hpp>
using mp_i128 = boost::multiprecision::int128_t;

// 0 = 1 word
// 1 = 2 words
// 2 = 2 word / 1 word alternating
// 3 = 1 word / 2 word alternating
// 4 = Random width

template <typename T>
constexpr T from_int128(const int128_t value)
{
    return static_cast<T>(value);
}

template <>
constexpr mp_i128 from_int128(const int128_t value)
{
    return static_cast<mp_i128>(value.high) << 64 | value.low;
}

#ifdef BOOST_INT128_HAS_MSVC_INTERNAL_I128

template <>
constexpr std::_Signed128 from_int128(const int128_t value)
{
    return static_cast<std::_Signed128>(value.high) << 64 | value.low;
}

#endif

template <int words, typename T>
std::vector<T> generate_random_vector(std::size_t size = N, unsigned seed = 42U)
{
    if (seed == 0)
    {
        std::random_device rd;
        seed = rd();
    }

    std::mt19937_64 gen(seed);
    std::uniform_int_distribution<std::uint64_t> dist_low(UINT64_C(0), UINT64_MAX);
    std::uniform_int_distribution<std::int64_t> dist_high(INT64_C(0), INT64_MAX);
    std::uniform_int_distribution<int> size_dist(0, 1);

    std::vector<T> result(size);
    for (std::size_t i = 0; i < size; ++i)
    {
        switch (words)
        {
            case 0:
                result[i] = from_int128<T>(int128_t{dist_low(gen)});
            break;

            case 1:
                result[i] = from_int128<T>(int128_t{dist_high(gen), dist_low(gen)});
            break;

            case 2:
                if (i % 2 == 0)
                {
                    result[i] = from_int128<T>(int128_t{dist_high(gen), dist_low(gen)});
                }
                else
                {
                    result[i] = from_int128<T>(int128_t{dist_low(gen)});
                }
            break;

            case 3:
                if (i % 2 == 1)
                {
                    result[i] = from_int128<T>(int128_t{dist_high(gen), dist_low(gen)});
                }
                else
                {
                    result[i] = from_int128<T>(int128_t{dist_low(gen)});
                }
            break;

            case 4:
                if (size_dist(gen) == 1)
                {
                    result[i] = from_int128<T>(int128_t{dist_high(gen), dist_low(gen)});
                }
                else
                {
                    result[i] = from_int128<T>(int128_t{dist_low(gen)});
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
            s += static_cast<std::size_t>(val1 == val2);
        }
    }

    auto t2 = std::chrono::steady_clock::now();

    std::cerr << "EQ <" << std::left << std::setw(11) << label << ">: " << std::setw( 10 ) << ( t2 - t1 ) / 1us << " us (s=" << s << ")\n";

    t1 = std::chrono::steady_clock::now();
    s = 0;

    for (std::size_t k {}; k < K; ++k)
    {
        for (std::size_t i {}; i < data_vec.size() - 1U; ++i)
        {
            const auto val1 = data_vec[i];
            const auto val2 = data_vec[i + 1];
            s += static_cast<std::size_t>(val1 != val2);
        }
    }

    t2 = std::chrono::steady_clock::now();

    std::cerr << "NE <" << std::left << std::setw(11) << label << ">: " << std::setw( 10 ) << ( t2 - t1 ) / 1us << " us (s=" << s << ")\n";

    t1 = std::chrono::steady_clock::now();
    s = 0;

    for (std::size_t k {}; k < K; ++k)
    {
        for (std::size_t i {}; i < data_vec.size() - 1U; ++i)
        {
            const auto val1 = data_vec[i];
            const auto val2 = data_vec[i + 1];
            s += static_cast<std::size_t>(val1 < val2);
        }
    }

    t2 = std::chrono::steady_clock::now();

    std::cerr << "LT <" << std::left << std::setw(11) << label << ">: " << std::setw( 10 ) << ( t2 - t1 ) / 1us << " us (s=" << s << ")\n";

    t1 = std::chrono::steady_clock::now();
    s = 0;

    for (std::size_t k {}; k < K; ++k)
    {
        for (std::size_t i {}; i < data_vec.size() - 1U; ++i)
        {
            const auto val1 = data_vec[i];
            const auto val2 = data_vec[i + 1];
            s += static_cast<std::size_t>(val1 <= val2);
        }
    }

    t2 = std::chrono::steady_clock::now();

    std::cerr << "LE <" << std::left << std::setw(11) << label << ">: " << std::setw( 10 ) << ( t2 - t1 ) / 1us << " us (s=" << s << ")\n";

    t1 = std::chrono::steady_clock::now();
    s = 0;

    for (std::size_t k {}; k < K; ++k)
    {
        for (std::size_t i {}; i < data_vec.size() - 1U; ++i)
        {
            const auto val1 = data_vec[i];
            const auto val2 = data_vec[i + 1];
            s += static_cast<std::size_t>(val1 > val2);
        }
    }

    t2 = std::chrono::steady_clock::now();

    std::cerr << "GT <" << std::left << std::setw(11) << label << ">: " << std::setw( 10 ) << ( t2 - t1 ) / 1us << " us (s=" << s << ")\n";

    t1 = std::chrono::steady_clock::now();
    s = 0;

    for (std::size_t k {}; k < K; ++k)
    {
        for (std::size_t i {}; i < data_vec.size() - 1U; ++i)
        {
            const auto val1 = data_vec[i];
            const auto val2 = data_vec[i + 1];
            s += static_cast<std::size_t>(val1 >= val2);
        }
    }

    t2 = std::chrono::steady_clock::now();

    std::cerr << "GE <" << std::left << std::setw(11) << label << ">: " << std::setw( 10 ) << ( t2 - t1 ) / 1us << " us (s=" << s << ")\n";

    std::cerr << "SUM<" << std::left << std::setw(11) << label << ">: " << std::setw( 10 ) << ( t2 - t_total ) / 1us << " us (s=" << s << ")\n\n";
}

template <typename T, typename Func>
BOOST_INT128_NO_INLINE void test_two_element_operation(const std::vector<T>& data_vec, Func op, const char* operation, const char* type)
{
    const auto t1 = std::chrono::steady_clock::now();
    std::int64_t s = 0; // discard variable

    for (std::size_t k {}; k < K; ++k)
    {
        for (std::size_t i {}; i < data_vec.size() - 1U; ++i)
        {
            const auto val1 = data_vec[i];
            const auto val2 = data_vec[i + 1];
            s += static_cast<std::int64_t>((op(val1, val2)));
        }
    }

    const auto t2 = std::chrono::steady_clock::now();

    std::cerr << operation << "<" << std::left << std::setw(11) << type << ">: " << std::setw( 10 ) << ( t2 - t1 ) / 1us << " us (s=" << s << ")\n";
}

int main()
{
    using namespace boost::int128::detail;

    // Two word operations
    {
        std::cerr << "\n---------------------------\n";
        std::cerr << "Two Word Operations\n";
        std::cerr << "---------------------------\n\n";

        const auto library_vector = generate_random_vector<0, int128_t>();
        const auto mp_vector = generate_random_vector<0, mp_i128>();

        #if defined(BOOST_INT128_HAS_INT128)

        const auto builtin_vector = generate_random_vector<0, builtin_i128>();
        test_comparisons(builtin_vector, "builtin");

        #elif defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)

        const auto builtin_vector = generate_random_vector<0, std::_Signed128>();
        test_comparisons(builtin_vector, "builtin");

        #endif

        test_comparisons(library_vector, "library");
        test_comparisons(mp_vector, "mp::i128");

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::plus<>(), "add", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::plus<>(), "add", "Library");
        test_two_element_operation(mp_vector, std::plus<>(), "add", "mp::i128");

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::minus<>(), "sub", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::minus<>(), "sub", "Library");
        test_two_element_operation(mp_vector, std::minus<>(), "sub", "mp::i128");

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::multiplies<>(), "mul", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::multiplies<>(), "mul", "Library");
        test_two_element_operation(mp_vector, std::multiplies<>(), "mul", "mp::i128");

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::divides<>(), "div", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::divides<>(), "div", "Library");
        test_two_element_operation(mp_vector, std::divides<>(), "div", "mp::i128");

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::modulus<>(), "mod", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::modulus<>(), "mod", "Library");
        test_two_element_operation(mp_vector, std::modulus<>(), "mod", "mp::i128");

        std::cerr << std::endl;
    }
    // Single word operations
    {
        std::cerr << "\n---------------------------\n";
        std::cerr << "One Word Operations\n";
        std::cerr << "---------------------------\n\n";

        const auto library_vector = generate_random_vector<1, int128_t>();
        const auto mp_vector = generate_random_vector<1, mp_i128>();

        #if defined(BOOST_INT128_HAS_INT128)

        const auto builtin_vector = generate_random_vector<1, builtin_i128>();
        test_comparisons(builtin_vector, "builtin");

        #elif defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)

        const auto builtin_vector = generate_random_vector<1, std::_Signed128>();
        test_comparisons(builtin_vector, "builtin");

        #endif

        test_comparisons(library_vector, "library");
        test_comparisons(mp_vector, "mp::i128");

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::plus<>(), "add", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::plus<>(), "add", "Library");
        test_two_element_operation(mp_vector, std::plus<>(), "add", "mp::i128");

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::minus<>(), "sub", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::minus<>(), "sub", "Library");
        test_two_element_operation(mp_vector, std::minus<>(), "sub", "mp::i128");

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::multiplies<>(), "mul", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::multiplies<>(), "mul", "Library");
        test_two_element_operation(mp_vector, std::multiplies<>(), "mul", "mp::i128");

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::divides<>(), "div", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::divides<>(), "div", "Library");
        test_two_element_operation(mp_vector, std::divides<>(), "div", "mp::i128");

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::modulus<>(), "mod", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::modulus<>(), "mod", "Library");
        test_two_element_operation(mp_vector, std::modulus<>(), "mod", "mp::i128");

        std::cerr << std::endl;
    }
    {
        // Two word and one word operations Even = 2, odd = 1

        std::cerr << "\n---------------------------\n";
        std::cerr << "Two-One Word Operations\n";
        std::cerr << "---------------------------\n\n";

        const auto library_vector = generate_random_vector<2, int128_t>();
        const auto mp_vector = generate_random_vector<2, mp_i128>();

        #if defined(BOOST_INT128_HAS_INT128)

        const auto builtin_vector = generate_random_vector<2, builtin_i128>();
        test_comparisons(builtin_vector, "builtin");

        #elif defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)

        const auto builtin_vector = generate_random_vector<2, std::_Signed128>();
        test_comparisons(builtin_vector, "builtin");

        #endif

        test_comparisons(library_vector, "library");
        test_comparisons(mp_vector, "mp::i128");

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::plus<>(), "add", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::plus<>(), "add", "Library");
        test_two_element_operation(mp_vector, std::plus<>(), "add", "mp::i128");

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::minus<>(), "sub", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::minus<>(), "sub", "Library");
        test_two_element_operation(mp_vector, std::minus<>(), "sub", "mp::i128");

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::multiplies<>(), "mul", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::multiplies<>(), "mul", "Library");
        test_two_element_operation(mp_vector, std::multiplies<>(), "mul", "mp::i128");

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::divides<>(), "div", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::divides<>(), "div", "Library");
        test_two_element_operation(mp_vector, std::divides<>(), "div", "mp::i128");

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::modulus<>(), "mod", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::modulus<>(), "mod", "Library");
        test_two_element_operation(mp_vector, std::modulus<>(), "mod", "mp::i128");

        std::cerr << std::endl;
    }
    {
        // Two word and one word operations Even = 1, odd = 2

        std::cerr << "\n---------------------------\n";
        std::cerr << "One-Two Word Operations\n";
        std::cerr << "---------------------------\n\n";

        const auto library_vector = generate_random_vector<3, int128_t>();
        const auto mp_vector = generate_random_vector<3, mp_i128>();

        #if defined(BOOST_INT128_HAS_INT128)

        const auto builtin_vector = generate_random_vector<3, builtin_i128>();
        test_comparisons(builtin_vector, "builtin");

        #elif defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)

        const auto builtin_vector = generate_random_vector<3, std::_Signed128>();
        test_comparisons(builtin_vector, "builtin");

        #endif

        test_comparisons(library_vector, "library");
        test_comparisons(mp_vector, "mp::i128");

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::plus<>(), "add", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::plus<>(), "add", "Library");
        test_two_element_operation(mp_vector, std::plus<>(), "add", "mp::i128");

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::minus<>(), "sub", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::minus<>(), "sub", "Library");
        test_two_element_operation(mp_vector, std::minus<>(), "sub", "mp::i128");

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::multiplies<>(), "mul", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::multiplies<>(), "mul", "Library");
        test_two_element_operation(mp_vector, std::multiplies<>(), "mul", "mp::i128");

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::divides<>(), "div", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::divides<>(), "div", "Library");
        test_two_element_operation(mp_vector, std::divides<>(), "div", "mp::i128");

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::modulus<>(), "mod", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::modulus<>(), "mod", "Library");
        test_two_element_operation(mp_vector, std::modulus<>(), "mod", "mp::i128");

        std::cerr << std::endl;
    }
    {
        // Two word and one word operations Even = 1, odd = 2

        std::cerr << "\n---------------------------\n";
        std::cerr << "Random Width Operations\n";
        std::cerr << "---------------------------\n\n";

        const auto library_vector = generate_random_vector<4, int128_t>();
        const auto mp_vector = generate_random_vector<4, mp_i128>();

        #if defined(BOOST_INT128_HAS_INT128)

        const auto builtin_vector = generate_random_vector<4, builtin_i128>();
        test_comparisons(builtin_vector, "builtin");

        #elif defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)

        const auto builtin_vector = generate_random_vector<4, std::_Signed128>();
        test_comparisons(builtin_vector, "builtin");

        #endif

        test_comparisons(library_vector, "library");
        test_comparisons(mp_vector, "mp::i128");

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::plus<>(), "add", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::plus<>(), "add", "Library");
        test_two_element_operation(mp_vector, std::plus<>(), "add", "mp::i128");

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::minus<>(), "sub", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::minus<>(), "sub", "Library");
        test_two_element_operation(mp_vector, std::minus<>(), "sub", "mp::i128");

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::multiplies<>(), "mul", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::multiplies<>(), "mul", "Library");
        test_two_element_operation(mp_vector, std::multiplies<>(), "mul", "mp::i128");

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::divides<>(), "div", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::divides<>(), "div", "Library");
        test_two_element_operation(mp_vector, std::divides<>(), "div", "mp::i128");

        std::cerr << std::endl;

        #if defined(BOOST_INT128_HAS_INT128) || defined(BOOST_INT128_HAS_MSVC_INTERNAL_I128)
        test_two_element_operation(builtin_vector, std::modulus<>(), "mod", "Builtin");
        #endif

        test_two_element_operation(library_vector, std::modulus<>(), "mod", "Library");
        test_two_element_operation(mp_vector, std::modulus<>(), "mod", "mp::i128");

        std::cerr << std::endl;
    }

    return 1;
}

#else // No benchmarks

int main()
{
    std::cerr << "Benchmarks Not Run" << std::endl;
    return 1;
}

#endif
