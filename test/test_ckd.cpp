// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128.hpp>
#include <boost/core/lightweight_test.hpp>
#include <random>
#include <cstdint>
#include <limits>

using boost::int128::ckd_add;
using boost::int128::ckd_sub;
using boost::int128::ckd_mul;
using boost::int128::int128_t;
using boost::int128::uint128_t;

constexpr std::size_t N {4096};
static std::mt19937_64 rng {42};
static std::uniform_int_distribution<std::uint64_t> dist {0, UINT64_MAX};

// Small magnitudes exercise the no-overflow path for narrow targets, where a
// purely full-range distribution would almost always overflow.
static std::uniform_int_distribution<int> small_dist {-1000, 1000};

//
// Oracle-based testing for the standard integer types. The __builtin_*_overflow
// intrinsics implement exactly the C23 contract (exact result, wrap into the
// destination, return true on overflow), so they are an independent reference.
//
#if defined(__GNUC__) || defined(__clang__)

template <typename T1, typename T2, typename T3, typename Ref, typename Ckd>
void check_op(const T2 a, const T3 b, Ref ref_overflow, Ckd ckd_overflow)
{
    T1 expected {};
    const bool expected_overflow {ref_overflow(a, b, &expected)};

    T1 got {};
    const bool got_overflow {ckd_overflow(&got, a, b)};

    BOOST_TEST_EQ(got_overflow, expected_overflow);
    BOOST_TEST(got == expected);
}

template <typename T1, typename T2, typename T3, typename Ref, typename Ckd>
void fuzz_op(Ref ref_overflow, Ckd ckd_overflow)
{
    for (std::size_t i {0}; i < N; ++i)
    {
        check_op<T1, T2, T3>(static_cast<T2>(dist(rng)),       static_cast<T3>(dist(rng)),       ref_overflow, ckd_overflow);
        check_op<T1, T2, T3>(static_cast<T2>(small_dist(rng)), static_cast<T3>(small_dist(rng)), ref_overflow, ckd_overflow);
        check_op<T1, T2, T3>(static_cast<T2>(dist(rng)),       static_cast<T3>(small_dist(rng)), ref_overflow, ckd_overflow);
        check_op<T1, T2, T3>(static_cast<T2>(small_dist(rng)), static_cast<T3>(dist(rng)),       ref_overflow, ckd_overflow);
    }
}

template <typename Ref, typename Ckd>
void fuzz_all_triples(Ref ref_overflow, Ckd ckd_overflow)
{
    fuzz_op<std::int32_t,  std::int32_t,  std::int32_t >(ref_overflow, ckd_overflow);
    fuzz_op<std::uint32_t, std::uint32_t, std::uint32_t>(ref_overflow, ckd_overflow);
    fuzz_op<std::int8_t,   std::int32_t,  std::int32_t >(ref_overflow, ckd_overflow);
    fuzz_op<std::uint8_t,  std::int32_t,  std::int32_t >(ref_overflow, ckd_overflow);
    fuzz_op<std::int16_t,  std::int16_t,  std::uint16_t>(ref_overflow, ckd_overflow);
    fuzz_op<std::int64_t,  std::int32_t,  std::uint32_t>(ref_overflow, ckd_overflow);
    fuzz_op<std::uint64_t, std::int64_t,  std::int64_t >(ref_overflow, ckd_overflow);
    fuzz_op<std::int32_t,  std::int64_t,  std::int64_t >(ref_overflow, ckd_overflow);
    fuzz_op<std::uint32_t, std::int8_t,   std::int8_t  >(ref_overflow, ckd_overflow);
    fuzz_op<std::int64_t,  std::uint64_t, std::uint64_t>(ref_overflow, ckd_overflow);
    fuzz_op<std::uint16_t, std::int64_t,  std::int32_t >(ref_overflow, ckd_overflow);
}

void test_standard_oracle()
{
    fuzz_all_triples(
        [](auto a, auto b, auto* r) { return __builtin_add_overflow(a, b, r); },
        [](auto* r, auto a, auto b) { return ckd_add(r, a, b); });

    fuzz_all_triples(
        [](auto a, auto b, auto* r) { return __builtin_sub_overflow(a, b, r); },
        [](auto* r, auto a, auto b) { return ckd_sub(r, a, b); });

    fuzz_all_triples(
        [](auto a, auto b, auto* r) { return __builtin_mul_overflow(a, b, r); },
        [](auto* r, auto a, auto b) { return ckd_mul(r, a, b); });
}

#else

void test_standard_oracle() {}

#endif

//
// Oracle-based testing at the full 128-bit width using the native compiler
// type, which again matches the C23 contract exactly. This is the only place
// products genuinely exceed 128 bits, exercising the multiply width check.
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

template <typename Ref, typename Ckd>
void native_fuzz(Ref ref_overflow, Ckd ckd_overflow)
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
            const bool ref_of {ref_overflow(ua, ub, &ref)};
            uint128_t got {};
            const bool got_of {ckd_overflow(&got, lib_u(ua), lib_u(ub))};
            BOOST_TEST_EQ(got_of, ref_of);
            BOOST_TEST(got == lib_u(ref));
        }

        // int128_t target, signed operands
        {
            __int128 ref {};
            const bool ref_of {ref_overflow(sa, sb, &ref)};
            int128_t got {};
            const bool got_of {ckd_overflow(&got, lib_s(sa), lib_s(sb))};
            BOOST_TEST_EQ(got_of, ref_of);
            BOOST_TEST(got == lib_s(ref));
        }

        // int128_t target, mixed-sign operands (unsigned + signed)
        {
            __int128 ref {};
            const bool ref_of {ref_overflow(ua, sb, &ref)};
            int128_t got {};
            const bool got_of {ckd_overflow(&got, lib_u(ua), lib_s(sb))};
            BOOST_TEST_EQ(got_of, ref_of);
            BOOST_TEST(got == lib_s(ref));
        }

        // uint128_t target, mixed-sign operands (signed + unsigned)
        {
            unsigned __int128 ref {};
            const bool ref_of {ref_overflow(sa, ub, &ref)};
            uint128_t got {};
            const bool got_of {ckd_overflow(&got, lib_s(sa), lib_u(ub))};
            BOOST_TEST_EQ(got_of, ref_of);
            BOOST_TEST(got == lib_u(ref));
        }
    }
}

void test_native_oracle()
{
    native_fuzz(
        [](auto a, auto b, auto* r) { return __builtin_add_overflow(a, b, r); },
        [](auto* r, auto a, auto b) { return ckd_add(r, a, b); });

    native_fuzz(
        [](auto a, auto b, auto* r) { return __builtin_sub_overflow(a, b, r); },
        [](auto* r, auto a, auto b) { return ckd_sub(r, a, b); });

    native_fuzz(
        [](auto a, auto b, auto* r) { return __builtin_mul_overflow(a, b, r); },
        [](auto* r, auto a, auto b) { return ckd_mul(r, a, b); });
}

#else

void test_native_oracle() {}

#endif

//
// Hand-verified edge cases that run on every platform, including those without
// a native 128-bit type.
//
constexpr auto u_max {(std::numeric_limits<uint128_t>::max)()};
constexpr auto i_max {(std::numeric_limits<int128_t>::max)()};
constexpr auto i_min {(std::numeric_limits<int128_t>::min)()};

void test_add_edges()
{
    uint128_t u {0};
    BOOST_TEST_EQ(ckd_add(&u, u_max, uint128_t{1}), true);   // 2^128 wraps to 0
    BOOST_TEST(u == uint128_t{0});
    BOOST_TEST_EQ(ckd_add(&u, uint128_t{5}, int128_t{-3}), false);
    BOOST_TEST(u == uint128_t{2});
    BOOST_TEST_EQ(ckd_add(&u, uint128_t{3}, int128_t{-5}), true);   // -2 wraps
    BOOST_TEST(u == u_max - uint128_t{1});
    BOOST_TEST_EQ(ckd_add(&u, u_max, u_max), true);          // carry past 2^128

    int128_t i {0};
    BOOST_TEST_EQ(ckd_add(&i, i_max, int128_t{1}), true);    // INT128_MAX + 1 -> INT128_MIN
    BOOST_TEST(i == i_min);
    BOOST_TEST_EQ(ckd_add(&i, i_max, i_min), false);
    BOOST_TEST(i == int128_t{-1});
    BOOST_TEST_EQ(ckd_add(&i, u_max, uint128_t{0}), true);   // 2^128 - 1 unfit in signed
    BOOST_TEST(i == int128_t{-1});
}

void test_sub_edges()
{
    uint128_t u {0};
    BOOST_TEST_EQ(ckd_sub(&u, uint128_t{0}, uint128_t{1}), true);   // -1 wraps to 2^128 - 1
    BOOST_TEST(u == u_max);
    BOOST_TEST_EQ(ckd_sub(&u, uint128_t{5}, uint128_t{3}), false);
    BOOST_TEST(u == uint128_t{2});
    BOOST_TEST_EQ(ckd_sub(&u, u_max, int128_t{-1}), true);          // 2^128 wraps to 0
    BOOST_TEST(u == uint128_t{0});

    int128_t i {0};
    BOOST_TEST_EQ(ckd_sub(&i, i_min, int128_t{1}), true);           // INT128_MIN - 1 -> INT128_MAX
    BOOST_TEST(i == i_max);
    BOOST_TEST_EQ(ckd_sub(&i, i_max, int128_t{-1}), true);          // -> INT128_MIN
    BOOST_TEST(i == i_min);
    BOOST_TEST_EQ(ckd_sub(&i, i_max, i_max), false);
    BOOST_TEST(i == int128_t{0});
    BOOST_TEST_EQ(ckd_sub(&i, i_min, i_min), false);
    BOOST_TEST(i == int128_t{0});

    // Narrow targets.
    std::int32_t r32 {0};
    BOOST_TEST_EQ(ckd_sub(&r32, int128_t{1000}, int128_t{2000}), false);
    BOOST_TEST_EQ(r32, -1000);

    std::uint8_t r8 {0};
    BOOST_TEST_EQ(ckd_sub(&r8, uint128_t{0}, uint128_t{1}), true);
    BOOST_TEST_EQ(static_cast<int>(r8), 255);
}

void test_mul_edges()
{
    int128_t i {0};
    BOOST_TEST_EQ(ckd_mul(&i, i_min, int128_t{-1}), true);          // 2^127 wraps to INT128_MIN
    BOOST_TEST(i == i_min);
    BOOST_TEST_EQ(ckd_mul(&i, i_max, int128_t{2}), true);           // 2^128 - 2 -> -2
    BOOST_TEST(i == int128_t{-2});
    BOOST_TEST_EQ(ckd_mul(&i, i_min, int128_t{1}), false);
    BOOST_TEST(i == i_min);
    BOOST_TEST_EQ(ckd_mul(&i, int128_t{-3}, int128_t{4}), false);
    BOOST_TEST(i == int128_t{-12});
    BOOST_TEST_EQ(ckd_mul(&i, int128_t{0}, int128_t{-7}), false);
    BOOST_TEST(i == int128_t{0});

    uint128_t u {0};
    BOOST_TEST_EQ(ckd_mul(&u, u_max, uint128_t{2}), true);          // 2^129 - 2 wraps
    BOOST_TEST(u == u_max - uint128_t{1});
    const uint128_t two_64 {1U, 0U};                                // 2^64
    BOOST_TEST_EQ(ckd_mul(&u, two_64, two_64), true);               // 2^128 wraps to 0
    BOOST_TEST(u == uint128_t{0});
    BOOST_TEST_EQ(ckd_mul(&u, uint128_t{0}, u_max), false);
    BOOST_TEST(u == uint128_t{0});
    BOOST_TEST_EQ(ckd_mul(&u, uint128_t{6}, uint128_t{7}), false);
    BOOST_TEST(u == uint128_t{42});
    BOOST_TEST_EQ(ckd_mul(&u, u_max, int128_t{-1}), true);          // negative result in unsigned
    BOOST_TEST(u == uint128_t{1});

    // Narrow targets.
    std::int32_t r32 {0};
    BOOST_TEST_EQ(ckd_mul(&r32, int128_t{1000}, int128_t{1000}), false);
    BOOST_TEST_EQ(r32, 1000000);

    std::uint8_t r8 {0};
    BOOST_TEST_EQ(ckd_mul(&r8, uint128_t{20}, uint128_t{20}), true);    // 400 wraps mod 256
    BOOST_TEST_EQ(static_cast<int>(r8), 144);

    std::int8_t r8s {0};
    BOOST_TEST_EQ(ckd_mul(&r8s, int128_t{-5}, int128_t{20}), false);
    BOOST_TEST_EQ(static_cast<int>(r8s), -100);
}

//
// constexpr usability for all three operations.
//
constexpr bool add_overflows_int_max()
{
    int r {0};
    return ckd_add(&r, (std::numeric_limits<int>::max)(), 1);
}

constexpr bool sub_overflows_int_min()
{
    int r {0};
    return ckd_sub(&r, (std::numeric_limits<int>::min)(), 1);
}

constexpr bool mul_overflows_int_max()
{
    int r {0};
    return ckd_mul(&r, (std::numeric_limits<int>::max)(), 2);
}

constexpr int sub_value()
{
    int r {0};
    ckd_sub(&r, 5, 3);
    return r;
}

constexpr int mul_value()
{
    int r {0};
    ckd_mul(&r, 6, 7);
    return r;
}

constexpr bool mul_overflows_i128_min()
{
    int128_t r {0};
    return ckd_mul(&r, (std::numeric_limits<int128_t>::min)(), int128_t{-1});
}

void test_constexpr()
{
    static_assert(add_overflows_int_max(),  "INT_MAX + 1 overflows int");
    static_assert(sub_overflows_int_min(),  "INT_MIN - 1 overflows int");
    static_assert(mul_overflows_int_max(),  "INT_MAX * 2 overflows int");
    static_assert(sub_value() == 2,         "5 - 3 == 2");
    static_assert(mul_value() == 42,        "6 * 7 == 42");
    static_assert(mul_overflows_i128_min(), "INT128_MIN * -1 overflows int128_t");
}

int main()
{
    test_standard_oracle();
    test_native_oracle();
    test_add_edges();
    test_sub_edges();
    test_mul_edges();
    test_constexpr();

    return boost::report_errors();
}
