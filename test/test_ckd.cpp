// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128.hpp>
#include <boost/core/lightweight_test.hpp>
#include <random>
#include <cstdint>
#include <limits>

using boost::int128::ckd_add;
using boost::int128::int128_t;
using boost::int128::uint128_t;

constexpr std::size_t N {4096};
static std::mt19937_64 rng {42};
static std::uniform_int_distribution<std::uint64_t> dist {0, UINT64_MAX};

// Small magnitudes exercise the no-overflow path for narrow targets, where a
// purely full-range distribution would almost always overflow.
static std::uniform_int_distribution<int> small_dist {-1000, 1000};

//
// Oracle-based testing for the standard integer types. __builtin_add_overflow
// implements exactly the C23 contract (exact sum, wrap into the destination,
// return true on overflow), so it is an independent reference.
//
#if defined(__GNUC__) || defined(__clang__)

template <typename T1, typename T2, typename T3>
void check(const T2 a, const T3 b)
{
    T1 expected {};
    const bool expected_overflow {__builtin_add_overflow(a, b, &expected)};

    T1 got {};
    const bool got_overflow {ckd_add(&got, a, b)};

    BOOST_TEST_EQ(got_overflow, expected_overflow);
    BOOST_TEST(got == expected);
}

template <typename T1, typename T2, typename T3>
void fuzz_standard()
{
    for (std::size_t i {0}; i < N; ++i)
    {
        check<T1, T2, T3>(static_cast<T2>(dist(rng)), static_cast<T3>(dist(rng)));
        check<T1, T2, T3>(static_cast<T2>(small_dist(rng)), static_cast<T3>(small_dist(rng)));
        check<T1, T2, T3>(static_cast<T2>(dist(rng)), static_cast<T3>(small_dist(rng)));
        check<T1, T2, T3>(static_cast<T2>(small_dist(rng)), static_cast<T3>(dist(rng)));
    }
}

void test_standard_oracle()
{
    fuzz_standard<std::int32_t,  std::int32_t,  std::int32_t>();
    fuzz_standard<std::uint32_t, std::uint32_t, std::uint32_t>();
    fuzz_standard<std::int8_t,   std::int32_t,  std::int32_t>();
    fuzz_standard<std::uint8_t,  std::int32_t,  std::int32_t>();
    fuzz_standard<std::int16_t,  std::int16_t,  std::uint16_t>();
    fuzz_standard<std::int64_t,  std::int32_t,  std::uint32_t>();
    fuzz_standard<std::uint64_t, std::int64_t,  std::int64_t>();
    fuzz_standard<std::int32_t,  std::int64_t,  std::int64_t>();
    fuzz_standard<std::uint32_t, std::int8_t,   std::int8_t>();
    fuzz_standard<std::int64_t,  std::uint64_t, std::uint64_t>();
    fuzz_standard<std::uint16_t, std::int64_t,  std::int32_t>();
}

#else

void test_standard_oracle() {}

#endif

//
// Oracle-based testing at the full 128-bit width using the native compiler
// type, which again matches the C23 contract exactly.
//
#if defined(__SIZEOF_INT128__) && (defined(__GNUC__) || defined(__clang__))

static uint128_t lib_u(const unsigned __int128 v)
{
    return uint128_t{static_cast<std::uint64_t>(v >> 64), static_cast<std::uint64_t>(v)};
}

static int128_t lib_s(const __int128 v)
{
    return static_cast<int128_t>(lib_u(static_cast<unsigned __int128>(v)));
}

static unsigned __int128 rand_native()
{
    return (static_cast<unsigned __int128>(dist(rng)) << 64) | static_cast<unsigned __int128>(dist(rng));
}

void test_native_oracle()
{
    for (std::size_t i {0}; i < N; ++i)
    {
        const unsigned __int128 ua {rand_native()};
        const unsigned __int128 ub {rand_native()};
        const __int128 sa {static_cast<__int128>(ua)};
        const __int128 sb {static_cast<__int128>(ub)};

        // uint128_t target, unsigned operands
        {
            unsigned __int128 ref {};
            const bool ref_of {__builtin_add_overflow(ua, ub, &ref)};
            uint128_t got {};
            const bool got_of {ckd_add(&got, lib_u(ua), lib_u(ub))};
            BOOST_TEST_EQ(got_of, ref_of);
            BOOST_TEST(got == lib_u(ref));
        }

        // int128_t target, signed operands
        {
            __int128 ref {};
            const bool ref_of {__builtin_add_overflow(sa, sb, &ref)};
            int128_t got {};
            const bool got_of {ckd_add(&got, lib_s(sa), lib_s(sb))};
            BOOST_TEST_EQ(got_of, ref_of);
            BOOST_TEST(got == lib_s(ref));
        }

        // int128_t target, mixed-sign operands (unsigned + signed)
        {
            __int128 ref {};
            const bool ref_of {__builtin_add_overflow(ua, sb, &ref)};
            int128_t got {};
            const bool got_of {ckd_add(&got, lib_u(ua), lib_s(sb))};
            BOOST_TEST_EQ(got_of, ref_of);
            BOOST_TEST(got == lib_s(ref));
        }

        // uint128_t target, mixed-sign operands
        {
            unsigned __int128 ref {};
            const bool ref_of {__builtin_add_overflow(sa, ub, &ref)};
            uint128_t got {};
            const bool got_of {ckd_add(&got, lib_s(sa), lib_u(ub))};
            BOOST_TEST_EQ(got_of, ref_of);
            BOOST_TEST(got == lib_u(ref));
        }
    }
}

#else

void test_native_oracle() {}

#endif

//
// Hand-verified edge cases that run on every platform, including those without
// a native 128-bit type.
//
void test_u128_edges()
{
    constexpr auto u_max {(std::numeric_limits<uint128_t>::max)()};
    uint128_t r {0};

    // 2^128 - 1 + 1 == 2^128 -> wraps to 0.
    BOOST_TEST_EQ(ckd_add(&r, u_max, uint128_t{1}), true);
    BOOST_TEST(r == uint128_t{0});

    // 2^128 - 1 + 0 fits.
    BOOST_TEST_EQ(ckd_add(&r, u_max, uint128_t{0}), false);
    BOOST_TEST(r == u_max);

    // 5 + (-3) == 2 fits.
    BOOST_TEST_EQ(ckd_add(&r, uint128_t{5}, int128_t{-3}), false);
    BOOST_TEST(r == uint128_t{2});

    // 3 + (-5) == -2 cannot be represented; wraps to 2^128 - 2.
    BOOST_TEST_EQ(ckd_add(&r, uint128_t{3}, int128_t{-5}), true);
    BOOST_TEST(r == u_max - uint128_t{1});

    // (2^128 - 1) + (-1) == 2^128 - 2 fits.
    BOOST_TEST_EQ(ckd_add(&r, u_max, int128_t{-1}), false);
    BOOST_TEST(r == u_max - uint128_t{1});

    // (2^128 - 1) + (2^128 - 1) == 2^129 - 2 -> carry, wraps to 2^128 - 2.
    BOOST_TEST_EQ(ckd_add(&r, u_max, u_max), true);
    BOOST_TEST(r == u_max - uint128_t{1});
}

void test_i128_edges()
{
    constexpr auto i_max {(std::numeric_limits<int128_t>::max)()};
    constexpr auto i_min {(std::numeric_limits<int128_t>::min)()};
    constexpr auto u_max {(std::numeric_limits<uint128_t>::max)()};
    int128_t r {0};

    // INT128_MAX + 1 overflows, wraps to INT128_MIN.
    BOOST_TEST_EQ(ckd_add(&r, i_max, int128_t{1}), true);
    BOOST_TEST(r == i_min);

    // INT128_MAX + 0 fits.
    BOOST_TEST_EQ(ckd_add(&r, i_max, int128_t{0}), false);
    BOOST_TEST(r == i_max);

    // INT128_MIN + (-1) overflows, wraps to INT128_MAX.
    BOOST_TEST_EQ(ckd_add(&r, i_min, int128_t{-1}), true);
    BOOST_TEST(r == i_max);

    // INT128_MAX + INT128_MIN == -1 fits.
    BOOST_TEST_EQ(ckd_add(&r, i_max, i_min), false);
    BOOST_TEST(r == int128_t{-1});

    // (2^128 - 1) [unsigned] + 1 == 2^128 cannot fit a signed target; wraps to 0.
    BOOST_TEST_EQ(ckd_add(&r, u_max, int128_t{1}), true);
    BOOST_TEST(r == int128_t{0});

    // (2^128 - 1) [unsigned] + 0 == 2^128 - 1 cannot fit; reinterprets to -1.
    BOOST_TEST_EQ(ckd_add(&r, u_max, uint128_t{0}), true);
    BOOST_TEST(r == int128_t{-1});
}

void test_narrow_target_wide_operands()
{
    constexpr auto u_max {(std::numeric_limits<uint128_t>::max)()};
    constexpr auto i_min {(std::numeric_limits<int128_t>::min)()};

    // Small sum into a narrow signed target fits.
    std::int32_t r32 {0};
    BOOST_TEST_EQ(ckd_add(&r32, int128_t{1000}, int128_t{2000}), false);
    BOOST_TEST_EQ(r32, 3000);

    // (2^128 - 1) + 2 == 2^128 + 1 -> wraps mod 256 to 1 in a uint8_t target.
    std::uint8_t r8 {0};
    BOOST_TEST_EQ(ckd_add(&r8, u_max, uint128_t{2}), true);
    BOOST_TEST_EQ(static_cast<int>(r8), 1);

    // INT128_MIN out of int32_t range; its low 32 bits are zero.
    BOOST_TEST_EQ(ckd_add(&r32, i_min, int128_t{0}), true);
    BOOST_TEST_EQ(r32, 0);
}

//
// constexpr usability.
//
constexpr bool ckd_overflows_int_max()
{
    int r {0};
    return ckd_add(&r, (std::numeric_limits<int>::max)(), 1);
}

constexpr int ckd_value()
{
    int r {0};
    ckd_add(&r, 2, 3);
    return r;
}

constexpr bool ckd_overflows_u128_max()
{
    uint128_t r {0};
    return ckd_add(&r, (std::numeric_limits<uint128_t>::max)(), uint128_t{1});
}

void test_constexpr()
{
    static_assert(ckd_overflows_int_max(), "INT_MAX + 1 overflows int");
    static_assert(ckd_value() == 5, "2 + 3 == 5");
    static_assert(ckd_overflows_u128_max(), "UINT128_MAX + 1 overflows uint128_t");
}

int main()
{
    test_standard_oracle();
    test_native_oracle();
    test_u128_edges();
    test_i128_edges();
    test_narrow_target_wide_operands();
    test_constexpr();

    return boost::report_errors();
}
