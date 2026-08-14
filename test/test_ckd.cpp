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
using boost::int128::int128;
using boost::int128::uint128;

constexpr std::size_t N {4096};
static std::mt19937_64 rng {42};
static std::uniform_int_distribution<std::uint64_t> dist {0, UINT64_MAX};

// Small magnitudes exercise the no-overflow path for narrow targets, where a
// purely full-range distribution would almost always overflow.
static std::uniform_int_distribution<int> small_dist {-1000, 1000};

//
// Oracle-based testing for the standard integer types. Addition and subtraction
// are checked against __builtin_add_overflow / __builtin_sub_overflow, which
// implement the C23 contract exactly (exact result, wrapped into the
// destination, true on overflow) and so are an independent reference.
//
// Multiplication uses a hand-rolled reference instead. __builtin_mul_overflow
// returns the wrong result for signed operands with an unsigned destination on
// GCC 7, and on Clang it lowers a 128-bit checked multiply to __muloti4, a
// compiler-rt symbol that is not always linked. ref_std_mul_overflow forms the
// exact product from 32-bit limbs (no 128-bit type, no runtime helper) so it is
// correct and links on every supported toolchain.
//
#if defined(__GNUC__) || defined(__clang__)

// 64x64 -> 128 bit unsigned product, returned as hi:lo, built from 32-bit limbs.
// This needs neither a 128-bit type nor a runtime helper such as __muloti4, so
// it links on every target including 32-bit ones.
static void mul_64_to_128(const std::uint64_t a, const std::uint64_t b,
                          std::uint64_t& hi, std::uint64_t& lo) noexcept
{
    const std::uint64_t mask {UINT64_C(0xFFFFFFFF)};
    const std::uint64_t a0 {a & mask};
    const std::uint64_t a1 {a >> 32};
    const std::uint64_t b0 {b & mask};
    const std::uint64_t b1 {b >> 32};

    const std::uint64_t p00 {a0 * b0};
    const std::uint64_t p01 {a0 * b1};
    const std::uint64_t p10 {a1 * b0};
    const std::uint64_t p11 {a1 * b1};

    const std::uint64_t mid {(p00 >> 32) + (p01 & mask) + (p10 & mask)};
    lo = (p00 & mask) | (mid << 32);
    hi = p11 + (p01 >> 32) + (p10 >> 32) + (mid >> 32);
}

// Signedness usable for the standard integer types and, through the
// specializations in the 128-bit section below, the native extended types.
template <typename T>
struct oracle_is_signed : std::is_signed<T> {};

template <typename T, std::enable_if_t<oracle_is_signed<T>::value, int> = 0>
std::uint64_t std_magnitude(const T value, bool& negative) noexcept
{
    negative = value < 0;
    const std::uint64_t image {static_cast<std::uint64_t>(value)};
    return negative ? (std::uint64_t{0} - image) : image;
}

template <typename T, std::enable_if_t<!oracle_is_signed<T>::value, int> = 0>
std::uint64_t std_magnitude(const T value, bool& negative) noexcept
{
    negative = false;
    return static_cast<std::uint64_t>(value);
}

template <typename R, std::enable_if_t<!oracle_is_signed<R>::value, int> = 0>
bool oracle_overflows_std(const std::uint64_t magnitude, const bool negative) noexcept
{
    const std::uint64_t r_max {static_cast<std::uint64_t>((std::numeric_limits<R>::max)())};
    return negative ? (magnitude != 0U) : (magnitude > r_max);
}

template <typename R, std::enable_if_t<oracle_is_signed<R>::value, int> = 0>
bool oracle_overflows_std(const std::uint64_t magnitude, const bool negative) noexcept
{
    const std::uint64_t r_max {static_cast<std::uint64_t>((std::numeric_limits<R>::max)())};
    const std::uint64_t min_magnitude {r_max + 1U};
    return negative ? (magnitude > min_magnitude) : (magnitude > r_max);
}

// Independent reference for the C23 ckd_mul contract on the standard integer
// types: forms the exact product, wraps it into *r, and reports whether the
// destination cannot represent the exact value.
template <typename A, typename B, typename R>
bool ref_std_mul_overflow(const A a, const B b, R* r) noexcept
{
    bool a_negative {};
    bool b_negative {};
    const std::uint64_t a_magnitude {std_magnitude(a, a_negative)};
    const std::uint64_t b_magnitude {std_magnitude(b, b_negative)};

    std::uint64_t hi {};
    std::uint64_t lo {};
    mul_64_to_128(a_magnitude, b_magnitude, hi, lo);

    const bool negative {a_negative != b_negative};
    const std::uint64_t wrapped {negative ? (std::uint64_t{0} - lo) : lo};
    *r = static_cast<R>(wrapped);

    if (hi != 0U)
    {
        return true;
    }

    return oracle_overflows_std<R>(lo, negative);
}

template <typename T1, typename T2, typename T3, typename Ref, typename Ckd>
void check_op(const T2 lhs, const T3 rhs, Ref ref_overflow, Ckd ckd_overflow)
{
    T1 expected {};
    const bool expected_overflow {ref_overflow(lhs, rhs, &expected)};

    T1 got {};
    const bool got_overflow {ckd_overflow(&got, lhs, rhs)};

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
        [](auto a, auto b, auto* r) { return ref_std_mul_overflow(a, b, r); },
        [](auto* r, auto a, auto b) { return ckd_mul(r, a, b); });
}

#else

void test_standard_oracle() {}

#endif

//
// Oracle-based testing at the full 128-bit width using the native compiler
// type. Addition and subtraction again use the builtins; multiplication uses
// ref_native_mul_overflow, which assembles the 256-bit product from 64-bit limb
// products so that no 128-bit multiply (hence no __muloti4) is emitted. This is
// the only place products genuinely exceed 128 bits, exercising the width check.
//
#if defined(__SIZEOF_INT128__) && (defined(__GNUC__) || defined(__clang__))

static uint128 lib_u(const unsigned __int128 v)
{
    return uint128{static_cast<std::uint64_t>(v >> 64), static_cast<std::uint64_t>(v)};
}

static int128 lib_s(const __int128 v)
{
    return static_cast<int128>(lib_u(static_cast<unsigned __int128>(v)));
}

static unsigned __int128 rand_native()
{
    return (static_cast<unsigned __int128>(dist(rng)) << 64) | static_cast<unsigned __int128>(dist(rng));
}

// The extended integer types are not guaranteed entries in std::is_signed under
// a strict -std flag, so their signedness is stated explicitly.
template <>
struct oracle_is_signed<__int128> : std::true_type {};

template <>
struct oracle_is_signed<unsigned __int128> : std::false_type {};

template <typename T, std::enable_if_t<oracle_is_signed<T>::value, int> = 0>
unsigned __int128 native_magnitude(const T value, bool& negative) noexcept
{
    negative = value < 0;
    const unsigned __int128 image {static_cast<unsigned __int128>(value)};
    return negative ? (static_cast<unsigned __int128>(0) - image) : image;
}

template <typename T, std::enable_if_t<!oracle_is_signed<T>::value, int> = 0>
unsigned __int128 native_magnitude(const T value, bool& negative) noexcept
{
    negative = false;
    return static_cast<unsigned __int128>(value);
}

template <typename R, std::enable_if_t<!oracle_is_signed<R>::value, int> = 0>
bool oracle_overflows_128(const unsigned __int128 magnitude, const bool negative) noexcept
{
    // A magnitude that fits in 128 bits fits an unsigned 128-bit target exactly;
    // only a non-zero negative value is unrepresentable.
    return negative && magnitude != 0U;
}

template <typename R, std::enable_if_t<oracle_is_signed<R>::value, int> = 0>
bool oracle_overflows_128(const unsigned __int128 magnitude, const bool negative) noexcept
{
    const unsigned __int128 positive_max {(static_cast<unsigned __int128>(1) << 127) - 1};
    const unsigned __int128 negative_max {static_cast<unsigned __int128>(1) << 127};
    return negative ? (magnitude > negative_max) : (magnitude > positive_max);
}

// Independent reference for the C23 ckd_mul contract at the full 128-bit width.
// The 256-bit product is assembled from 64-bit limb products so that no 128-bit
// multiply (and therefore no __muloti4) is emitted; only native add, shift, and
// compare on unsigned __int128 are used.
template <typename A, typename B, typename R>
bool ref_native_mul_overflow(const A a, const B b, R* r) noexcept
{
    bool a_negative {};
    bool b_negative {};
    const unsigned __int128 a_magnitude {native_magnitude(a, a_negative)};
    const unsigned __int128 b_magnitude {native_magnitude(b, b_negative)};

    const std::uint64_t a0 {static_cast<std::uint64_t>(a_magnitude)};
    const std::uint64_t a1 {static_cast<std::uint64_t>(a_magnitude >> 64)};
    const std::uint64_t b0 {static_cast<std::uint64_t>(b_magnitude)};
    const std::uint64_t b1 {static_cast<std::uint64_t>(b_magnitude >> 64)};

    std::uint64_t h00 {};
    std::uint64_t l00 {};
    std::uint64_t h01 {};
    std::uint64_t l01 {};
    std::uint64_t h10 {};
    std::uint64_t l10 {};
    std::uint64_t h11 {};
    std::uint64_t l11 {};
    mul_64_to_128(a0, b0, h00, l00);
    mul_64_to_128(a0, b1, h01, l01);
    mul_64_to_128(a1, b0, h10, l10);
    mul_64_to_128(a1, b1, h11, l11);

    const unsigned __int128 p00 {(static_cast<unsigned __int128>(h00) << 64) | l00};
    const unsigned __int128 p01 {(static_cast<unsigned __int128>(h01) << 64) | l01};
    const unsigned __int128 p10 {(static_cast<unsigned __int128>(h10) << 64) | l10};
    const unsigned __int128 p11 {(static_cast<unsigned __int128>(h11) << 64) | l11};

    // product = p11 * 2^128 + (p01 + p10) * 2^64 + p00, split into a low and a
    // high 128-bit half with the carries tracked explicitly.
    const unsigned __int128 cross {p01 + p10};
    const bool cross_carry {cross < p01};
    const unsigned __int128 low128 {p00 + (cross << 64)};
    const bool low_carry {low128 < p00};
    const unsigned __int128 high128 {p11 + (cross >> 64) +
                                     (static_cast<unsigned __int128>(cross_carry) << 64) +
                                     static_cast<unsigned __int128>(low_carry)};

    const bool negative {a_negative != b_negative};
    const unsigned __int128 wrapped {negative ? (static_cast<unsigned __int128>(0) - low128) : low128};
    *r = static_cast<R>(wrapped);

    if (high128 != 0U)
    {
        return true;
    }

    return oracle_overflows_128<R>(low128, negative);
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

        // uint128 target, unsigned operands
        {
            unsigned __int128 ref {};
            const bool ref_of {ref_overflow(ua, ub, &ref)};
            uint128 got {};
            const bool got_of {ckd_overflow(&got, lib_u(ua), lib_u(ub))};
            BOOST_TEST_EQ(got_of, ref_of);
            BOOST_TEST(got == lib_u(ref));
        }

        // int128 target, signed operands
        {
            __int128 ref {};
            const bool ref_of {ref_overflow(sa, sb, &ref)};
            int128 got {};
            const bool got_of {ckd_overflow(&got, lib_s(sa), lib_s(sb))};
            BOOST_TEST_EQ(got_of, ref_of);
            BOOST_TEST(got == lib_s(ref));
        }

        // int128 target, mixed-sign operands (unsigned + signed)
        {
            __int128 ref {};
            const bool ref_of {ref_overflow(ua, sb, &ref)};
            int128 got {};
            const bool got_of {ckd_overflow(&got, lib_u(ua), lib_s(sb))};
            BOOST_TEST_EQ(got_of, ref_of);
            BOOST_TEST(got == lib_s(ref));
        }

        // uint128 target, mixed-sign operands (signed + unsigned)
        {
            unsigned __int128 ref {};
            const bool ref_of {ref_overflow(sa, ub, &ref)};
            uint128 got {};
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
        [](auto a, auto b, auto* r) { return ref_native_mul_overflow(a, b, r); },
        [](auto* r, auto a, auto b) { return ckd_mul(r, a, b); });
}

#else

void test_native_oracle() {}

#endif

//
// Hand-verified edge cases that run on every platform, including those without
// a native 128-bit type.
//
constexpr auto u_max {(std::numeric_limits<uint128>::max)()};
constexpr auto i_max {(std::numeric_limits<int128>::max)()};
constexpr auto i_min {(std::numeric_limits<int128>::min)()};

void test_add_edges()
{
    uint128 u {0};
    BOOST_TEST_EQ(ckd_add(&u, u_max, uint128{1}), true);   // 2^128 wraps to 0
    BOOST_TEST(u == uint128{0});
    BOOST_TEST_EQ(ckd_add(&u, uint128{5}, int128{-3}), false);
    BOOST_TEST(u == uint128{2});
    BOOST_TEST_EQ(ckd_add(&u, uint128{3}, int128{-5}), true);   // -2 wraps
    BOOST_TEST(u == u_max - uint128{1});
    BOOST_TEST_EQ(ckd_add(&u, u_max, u_max), true);          // carry past 2^128

    int128 i {0};
    BOOST_TEST_EQ(ckd_add(&i, i_max, int128{1}), true);    // INT128_MAX + 1 -> INT128_MIN
    BOOST_TEST(i == i_min);
    BOOST_TEST_EQ(ckd_add(&i, i_max, i_min), false);
    BOOST_TEST(i == int128{-1});
    BOOST_TEST_EQ(ckd_add(&i, u_max, uint128{0}), true);   // 2^128 - 1 unfit in signed
    BOOST_TEST(i == int128{-1});
}

void test_sub_edges()
{
    uint128 u {0};
    BOOST_TEST_EQ(ckd_sub(&u, uint128{0}, uint128{1}), true);   // -1 wraps to 2^128 - 1
    BOOST_TEST(u == u_max);
    BOOST_TEST_EQ(ckd_sub(&u, uint128{5}, uint128{3}), false);
    BOOST_TEST(u == uint128{2});
    BOOST_TEST_EQ(ckd_sub(&u, u_max, int128{-1}), true);          // 2^128 wraps to 0
    BOOST_TEST(u == uint128{0});

    int128 i {0};
    BOOST_TEST_EQ(ckd_sub(&i, i_min, int128{1}), true);           // INT128_MIN - 1 -> INT128_MAX
    BOOST_TEST(i == i_max);
    BOOST_TEST_EQ(ckd_sub(&i, i_max, int128{-1}), true);          // -> INT128_MIN
    BOOST_TEST(i == i_min);
    BOOST_TEST_EQ(ckd_sub(&i, i_max, i_max), false);
    BOOST_TEST(i == int128{0});
    BOOST_TEST_EQ(ckd_sub(&i, i_min, i_min), false);
    BOOST_TEST(i == int128{0});

    // Narrow targets.
    std::int32_t r32 {0};
    BOOST_TEST_EQ(ckd_sub(&r32, int128{1000}, int128{2000}), false);
    BOOST_TEST_EQ(r32, -1000);

    std::uint8_t r8 {0};
    BOOST_TEST_EQ(ckd_sub(&r8, uint128{0}, uint128{1}), true);
    BOOST_TEST_EQ(static_cast<int>(r8), 255);
}

void test_mul_edges()
{
    int128 i {0};
    BOOST_TEST_EQ(ckd_mul(&i, i_min, int128{-1}), true);          // 2^127 wraps to INT128_MIN
    BOOST_TEST(i == i_min);
    BOOST_TEST_EQ(ckd_mul(&i, i_max, int128{2}), true);           // 2^128 - 2 -> -2
    BOOST_TEST(i == int128{-2});
    BOOST_TEST_EQ(ckd_mul(&i, i_min, int128{1}), false);
    BOOST_TEST(i == i_min);
    BOOST_TEST_EQ(ckd_mul(&i, int128{-3}, int128{4}), false);
    BOOST_TEST(i == int128{-12});
    BOOST_TEST_EQ(ckd_mul(&i, int128{0}, int128{-7}), false);
    BOOST_TEST(i == int128{0});

    uint128 u {0};
    BOOST_TEST_EQ(ckd_mul(&u, u_max, uint128{2}), true);          // 2^129 - 2 wraps
    BOOST_TEST(u == u_max - uint128{1});
    const uint128 two_64 {1U, 0U};                                // 2^64
    BOOST_TEST_EQ(ckd_mul(&u, two_64, two_64), true);               // 2^128 wraps to 0
    BOOST_TEST(u == uint128{0});
    BOOST_TEST_EQ(ckd_mul(&u, uint128{0}, u_max), false);
    BOOST_TEST(u == uint128{0});
    BOOST_TEST_EQ(ckd_mul(&u, uint128{6}, uint128{7}), false);
    BOOST_TEST(u == uint128{42});
    BOOST_TEST_EQ(ckd_mul(&u, u_max, int128{-1}), true);          // negative result in unsigned
    BOOST_TEST(u == uint128{1});

    // Narrow targets.
    std::int32_t r32 {0};
    BOOST_TEST_EQ(ckd_mul(&r32, int128{1000}, int128{1000}), false);
    BOOST_TEST_EQ(r32, 1000000);

    std::uint8_t r8 {0};
    BOOST_TEST_EQ(ckd_mul(&r8, uint128{20}, uint128{20}), true);    // 400 wraps mod 256
    BOOST_TEST_EQ(static_cast<int>(r8), 144);

    std::int8_t r8s {0};
    BOOST_TEST_EQ(ckd_mul(&r8s, int128{-5}, int128{20}), false);
    BOOST_TEST_EQ(static_cast<int>(r8s), -100);
}

//
// constexpr usability for all three operations.
//

#if defined(__GNUC__) && __GNUC__ <= 7 && !defined(__clang__) && !defined(__SIZEOF_INT128__)
#  define BOOST_int128EST_CKD_NO_CONSTEXPR_128
#endif

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
    int128 r {0};
    return ckd_mul(&r, (std::numeric_limits<int128>::min)(), int128{-1});
}

#ifndef BOOST_int128EST_CKD_NO_CONSTEXPR_128

// MSVC 14.1 warns of integral overflow
#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable: 4307)
#endif

void test_constexpr()
{
    static_assert(add_overflows_int_max(),  "INT_MAX + 1 overflows int");
    static_assert(sub_overflows_int_min(),  "INT_MIN - 1 overflows int");
    static_assert(mul_overflows_int_max(),  "INT_MAX * 2 overflows int");
    static_assert(sub_value() == 2,         "5 - 3 == 2");
    static_assert(mul_value() == 42,        "6 * 7 == 42");
    static_assert(mul_overflows_i128_min(), "INT128_MIN * -1 overflows int128");
}

#ifdef _MSC_VER
#  pragma warning(pop)
#endif

#endif

int main()
{
    test_standard_oracle();
    test_native_oracle();
    test_add_edges();
    test_sub_edges();
    test_mul_edges();

    #ifndef BOOST_int128EST_CKD_NO_CONSTEXPR_128
    test_constexpr();
    #endif

    return boost::report_errors();
}
