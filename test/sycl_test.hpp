//  Copyright 2026 Matt Borland
//  Distributed under the Boost Software License, Version 1.0.
//  https://www.boost.org/LICENSE_1_0.txt
//
//  Shared harness for the Boost.int128 SYCL tests. Each test runs an operation
//  element-wise on the SYCL device over random inputs and verifies that the
//  device results match a host recomputation of the same operation.

#ifndef BOOST_int128EST_SYCL_TEST_HPP
#define BOOST_int128EST_SYCL_TEST_HPP

#include <sycl/sycl.hpp>
#include <boost/int128.hpp>
#include <boost/int128/charconv.hpp>
#include <iostream>
#include <limits>
#include <random>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <type_traits>

namespace int128_sycl_test {

constexpr int num_elements {20000};
// Matches the buffer size mini_to_chars expects (large enough for base 2).
constexpr int buf_size {static_cast<int>(boost::int128::detail::mini_to_chars_buffer_size)};

// Produces a full-range random value of the int128 type T from a 64-bit engine. This avoids
// pulling Boost.Random (whose headers are host-only) into the single-source SYCL translation
// unit, where the device pass would still have to parse them.
template <typename T>
T random_value(std::mt19937_64& rng)
{
    // Both types store the high word unsigned, but the signed type's two word
    // constructor still takes it signed, so this is not decltype(T{}.high).
    using high_type = typename std::conditional<std::numeric_limits<T>::is_signed,
                                                std::int64_t, std::uint64_t>::type;
    return T{static_cast<high_type>(rng()), static_cast<std::uint64_t>(rng())};
}

// Returns a divisor that avoids the divide-by-zero and INT_MIN / -1 cases, so division
// stays well defined. Applied identically on host and device.
template <typename T>
inline T safe_divisor(const T a, const T b) noexcept
{
    if (b == T{0})
    {
        return T{1};
    }
    if (std::numeric_limits<T>::is_signed && a == (std::numeric_limits<T>::min)() && b == T{-1})
    {
        return T{1};
    }
    return b;
}

template <typename T>
inline T safe_div(const T a, const T b) noexcept
{
    return a / safe_divisor(a, b);
}

template <typename T>
inline T safe_mod(const T a, const T b) noexcept
{
    return a % safe_divisor(a, b);
}

// Generic runner. op(a, b, i) is evaluated on the device for every element and then
// re-evaluated on the host; any mismatch fails the test. op must depend only on
// device-enabled int128 facilities.
template <typename InT, typename OutT, typename Op>
int run(Op op)
{
    sycl::queue q;
    std::cout << "SYCL device: "
              << q.get_device().get_info<sycl::info::device::name>() << "\n";

    InT* a {sycl::malloc_shared<InT>(num_elements, q)};
    InT* b {sycl::malloc_shared<InT>(num_elements, q)};
    OutT* out {sycl::malloc_shared<OutT>(num_elements, q)};

    std::mt19937_64 rng {42};
    for (int i {0}; i < num_elements; ++i)
    {
        a[i] = random_value<InT>(rng);
        b[i] = random_value<InT>(rng);
    }

    q.submit([&](sycl::handler& h)
    {
        h.parallel_for(sycl::range<1>(num_elements), [=](sycl::id<1> idx)
        {
            const int i {static_cast<int>(idx[0])};
            out[i] = op(a[i], b[i], i);
        });
    }).wait();

    int failures {0};
    for (int i {0}; i < num_elements; ++i)
    {
        if (out[i] != op(a[i], b[i], i))
        {
            if (failures < 5)
            {
                std::cerr << "Mismatch at element " << i << "\n";
            }
            ++failures;
        }
    }

    sycl::free(a, q);
    sycl::free(b, q);
    sycl::free(out, q);

    if (failures == 0)
    {
        std::cout << "Test PASSED\n";
        return EXIT_SUCCESS;
    }

    std::cerr << "Test FAILED with " << failures << " mismatches\n";
    return EXIT_FAILURE;
}

// Comparison runner. Half of the second operands are forced equal to the first so the
// equal/less-equal/greater-equal branches are exercised alongside the strict ones.
template <typename InT, typename Pred>
int run_compare(Pred pred)
{
    sycl::queue q;
    std::cout << "SYCL device: "
              << q.get_device().get_info<sycl::info::device::name>() << "\n";

    InT* a {sycl::malloc_shared<InT>(num_elements, q)};
    InT* b {sycl::malloc_shared<InT>(num_elements, q)};
    int* out {sycl::malloc_shared<int>(num_elements, q)};

    std::mt19937_64 rng {42};
    for (int i {0}; i < num_elements; ++i)
    {
        a[i] = random_value<InT>(rng);
        b[i] = (i % 2 == 0) ? a[i] : random_value<InT>(rng);
    }

    q.submit([&](sycl::handler& h)
    {
        h.parallel_for(sycl::range<1>(num_elements), [=](sycl::id<1> idx)
        {
            const int i {static_cast<int>(idx[0])};
            out[i] = static_cast<int>(pred(a[i], b[i]));
        });
    }).wait();

    int failures {0};
    for (int i {0}; i < num_elements; ++i)
    {
        if (out[i] != static_cast<int>(pred(a[i], b[i])))
        {
            if (failures < 5)
            {
                std::cerr << "Mismatch at element " << i << "\n";
            }
            ++failures;
        }
    }

    sycl::free(a, q);
    sycl::free(b, q);
    sycl::free(out, q);

    if (failures == 0)
    {
        std::cout << "Test PASSED\n";
        return EXIT_SUCCESS;
    }

    std::cerr << "Test FAILED with " << failures << " mismatches\n";
    return EXIT_FAILURE;
}

// to_chars runner. Formats each value on the device via boost::charconv::to_chars (now
// SYCL device-capable) and compares the produced text against the host.
template <typename InT>
int run_to_chars(const int base)
{
    using boost::charconv::to_chars;

    sycl::queue q;
    std::cout << "SYCL device: "
              << q.get_device().get_info<sycl::info::device::name>() << "\n";

    InT* in {sycl::malloc_shared<InT>(num_elements, q)};
    char* out_str {sycl::malloc_shared<char>(num_elements * buf_size, q)};
    int* out_len {sycl::malloc_shared<int>(num_elements, q)};

    std::mt19937_64 rng {42};
    for (int i {0}; i < num_elements; ++i)
    {
        in[i] = random_value<InT>(rng);
    }

    q.submit([&](sycl::handler& h)
    {
        h.parallel_for(sycl::range<1>(num_elements), [=](sycl::id<1> idx)
        {
            const int i {static_cast<int>(idx[0])};
            char buf[buf_size];
            const auto r {to_chars(buf, buf + buf_size, in[i], base)};
            const int n {static_cast<int>(r.ptr - buf)};
            char* dest {out_str + i * buf_size};
            for (int k {0}; k < n; ++k)
            {
                dest[k] = buf[k];
            }
            out_len[i] = n;
        });
    }).wait();

    int failures {0};
    for (int i {0}; i < num_elements; ++i)
    {
        char buf[buf_size];
        const auto r {to_chars(buf, buf + buf_size, in[i], base)};
        const int n {static_cast<int>(r.ptr - buf)};
        if (out_len[i] != n || std::memcmp(out_str + i * buf_size, buf, static_cast<std::size_t>(n)) != 0)
        {
            if (failures < 5)
            {
                std::cerr << "Mismatch at element " << i << "\n";
            }
            ++failures;
        }
    }

    sycl::free(in, q);
    sycl::free(out_str, q);
    sycl::free(out_len, q);

    if (failures == 0)
    {
        std::cout << "Test PASSED\n";
        return EXIT_SUCCESS;
    }

    std::cerr << "Test FAILED with " << failures << " mismatches\n";
    return EXIT_FAILURE;
}

// from_chars runner. Formats each value on the host, parses it back on the device via
// boost::charconv::from_chars (now SYCL device-capable), and checks the round trip.
template <typename InT>
int run_from_chars(const int base)
{
    using boost::charconv::to_chars;
    using boost::charconv::from_chars;

    sycl::queue q;
    std::cout << "SYCL device: "
              << q.get_device().get_info<sycl::info::device::name>() << "\n";

    char* in_str {sycl::malloc_shared<char>(num_elements * buf_size, q)};
    int* in_len {sycl::malloc_shared<int>(num_elements, q)};
    InT* out {sycl::malloc_shared<InT>(num_elements, q)};
    InT* expected {sycl::malloc_shared<InT>(num_elements, q)};

    std::mt19937_64 rng {42};
    for (int i {0}; i < num_elements; ++i)
    {
        expected[i] = random_value<InT>(rng);
        char buf[buf_size];
        const auto r {to_chars(buf, buf + buf_size, expected[i], base)};
        const int n {static_cast<int>(r.ptr - buf)};
        std::memcpy(in_str + i * buf_size, buf, static_cast<std::size_t>(n));
        in_len[i] = n;
    }

    q.submit([&](sycl::handler& h)
    {
        h.parallel_for(sycl::range<1>(num_elements), [=](sycl::id<1> idx)
        {
            const int i {static_cast<int>(idx[0])};
            const char* first {in_str + i * buf_size};
            InT value {};
            from_chars(first, first + in_len[i], value, base);
            out[i] = value;
        });
    }).wait();

    int failures {0};
    for (int i {0}; i < num_elements; ++i)
    {
        if (out[i] != expected[i])
        {
            if (failures < 5)
            {
                std::cerr << "Mismatch at element " << i << "\n";
            }
            ++failures;
        }
    }

    sycl::free(in_str, q);
    sycl::free(in_len, q);
    sycl::free(out, q);
    sycl::free(expected, q);

    if (failures == 0)
    {
        std::cout << "Test PASSED\n";
        return EXIT_SUCCESS;
    }

    std::cerr << "Test FAILED with " << failures << " mismatches\n";
    return EXIT_FAILURE;
}

} // namespace int128_sycl_test

#endif // BOOST_int128EST_SYCL_TEST_HPP
