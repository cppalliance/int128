// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
//
// Mixed operations between the library types and the built-in floating point types.
//
// The usual arithmetic conversions convert the integer operand to the floating point type
// before the operation is applied, so every expression here must produce exactly what the
// built-in __int128 / unsigned __int128 produces for the same expression, down to the bit
// pattern of the result. The operators the built-in does not allow on a floating point
// operand (modulo, bitwise and shift) must be ill-formed here as well.

// The built-in comparison is the oracle for the comparison operators, so this file compares
// floating point values with == and != on purpose. The pragmas must cover lightweight_test.hpp
// so that BOOST_TEST_EQ can be used on them
#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wfloat-equal"
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wfloat-equal"
#endif

#include <boost/int128.hpp>
#include <boost/core/lightweight_test.hpp>
#include <cstdint>
#include <cstring>
#include <limits>
#include <random>
#include <type_traits>

using boost::int128::int128;
using boost::int128::uint128;

// long double{} does not parse, so the macros below need a single token type name
using long_double = long double;

// =========================================================================
// Result types, on every platform
// =========================================================================

// A floating point operand yields the floating point type, exactly as it does for a built-in
#define BOOST_INT128_TEST_RESULT_TYPES(Int128, Float)                                          \
    static_assert(std::is_same<decltype(Int128{} + Float{}), Float>::value, "int + float");     \
    static_assert(std::is_same<decltype(Float{} + Int128{}), Float>::value, "float + int");     \
    static_assert(std::is_same<decltype(Int128{} - Float{}), Float>::value, "int - float");     \
    static_assert(std::is_same<decltype(Float{} - Int128{}), Float>::value, "float - int");     \
    static_assert(std::is_same<decltype(Int128{} * Float{}), Float>::value, "int * float");     \
    static_assert(std::is_same<decltype(Float{} * Int128{}), Float>::value, "float * int");     \
    static_assert(std::is_same<decltype(Int128{} / Float{}), Float>::value, "int / float");     \
    static_assert(std::is_same<decltype(Float{} / Int128{}), Float>::value, "float / int");     \
    static_assert(std::is_same<decltype(Int128{} == Float{}), bool>::value, "int == float");    \
    static_assert(std::is_same<decltype(Float{} == Int128{}), bool>::value, "float == int");    \
    static_assert(std::is_same<decltype(Int128{} != Float{}), bool>::value, "int != float");    \
    static_assert(std::is_same<decltype(Int128{} <  Float{}), bool>::value, "int < float");     \
    static_assert(std::is_same<decltype(Int128{} <= Float{}), bool>::value, "int <= float");    \
    static_assert(std::is_same<decltype(Int128{} >  Float{}), bool>::value, "int > float");     \
    static_assert(std::is_same<decltype(Int128{} >= Float{}), bool>::value, "int >= float");    \
    static_assert(std::is_same<decltype(Float{} <  Int128{}), bool>::value, "float < int");     \
    static_assert(std::is_same<decltype(Float{} >= Int128{}), bool>::value, "float >= int");

BOOST_INT128_TEST_RESULT_TYPES(uint128, float)
BOOST_INT128_TEST_RESULT_TYPES(uint128, double)
BOOST_INT128_TEST_RESULT_TYPES(int128, float)
BOOST_INT128_TEST_RESULT_TYPES(int128, double)

#ifndef BOOST_INT128_HAS_GPU_SUPPORT
BOOST_INT128_TEST_RESULT_TYPES(uint128, long_double)
BOOST_INT128_TEST_RESULT_TYPES(int128, long_double)
#endif

#undef BOOST_INT128_TEST_RESULT_TYPES

// Compound assignment keeps the type of the left operand
static_assert(std::is_same<decltype(std::declval<uint128&>() += 1.0), uint128&>::value, "u += d");
static_assert(std::is_same<decltype(std::declval<uint128&>() -= 1.0F), uint128&>::value, "u -= f");
static_assert(std::is_same<decltype(std::declval<int128&>() *= 1.0), int128&>::value, "i *= d");
static_assert(std::is_same<decltype(std::declval<int128&>() /= 1.0F), int128&>::value, "i /= f");
static_assert(std::is_same<decltype(std::declval<double&>() += uint128{}), double&>::value, "d += u");
static_assert(std::is_same<decltype(std::declval<float&>() *= int128{}), float&>::value, "f *= i");

#ifdef BOOST_INT128_HAS_SPACESHIP_OPERATOR

// Mixing an integer and a floating point type yields a partial ordering because of NaN
static_assert(std::is_same<decltype(uint128{} <=> 1.0), std::partial_ordering>::value, "u <=> d");
static_assert(std::is_same<decltype(1.0 <=> uint128{}), std::partial_ordering>::value, "d <=> u");
static_assert(std::is_same<decltype(int128{} <=> 1.0F), std::partial_ordering>::value, "i <=> f");
static_assert(std::is_same<decltype(1.0F <=> int128{}), std::partial_ordering>::value, "f <=> i");

#endif // BOOST_INT128_HAS_SPACESHIP_OPERATOR

// =========================================================================
// The operators the built-in forbids must be ill-formed, on every platform
// =========================================================================

namespace {

template <typename...>
struct make_void { using type = void; };

template <typename... Ts>
using void_t = typename make_void<Ts...>::type;

#define BOOST_INT128_TEST_DETECT_BINARY(name, op)                                                \
    template <typename T, typename U, typename = void>                                           \
    struct name : std::false_type {};                                                            \
                                                                                                 \
    template <typename T, typename U>                                                            \
    struct name<T, U, void_t<decltype(std::declval<T>() op std::declval<U>())>> : std::true_type {};

#define BOOST_INT128_TEST_DETECT_COMPOUND(name, op)                                              \
    template <typename T, typename U, typename = void>                                           \
    struct name : std::false_type {};                                                            \
                                                                                                 \
    template <typename T, typename U>                                                            \
    struct name<T, U, void_t<decltype(std::declval<T&>() op std::declval<U>())>> : std::true_type {};

BOOST_INT128_TEST_DETECT_BINARY(has_mod, %)
BOOST_INT128_TEST_DETECT_BINARY(has_and, &)
BOOST_INT128_TEST_DETECT_BINARY(has_or, |)
BOOST_INT128_TEST_DETECT_BINARY(has_xor, ^)
BOOST_INT128_TEST_DETECT_BINARY(has_shl, <<)
BOOST_INT128_TEST_DETECT_BINARY(has_shr, >>)
BOOST_INT128_TEST_DETECT_BINARY(has_add, +)

BOOST_INT128_TEST_DETECT_COMPOUND(has_mod_eq, %=)
BOOST_INT128_TEST_DETECT_COMPOUND(has_and_eq, &=)
BOOST_INT128_TEST_DETECT_COMPOUND(has_or_eq, |=)
BOOST_INT128_TEST_DETECT_COMPOUND(has_xor_eq, ^=)
BOOST_INT128_TEST_DETECT_COMPOUND(has_shl_eq, <<=)
BOOST_INT128_TEST_DETECT_COMPOUND(has_shr_eq, >>=)
BOOST_INT128_TEST_DETECT_COMPOUND(has_add_eq, +=)

#undef BOOST_INT128_TEST_DETECT_BINARY
#undef BOOST_INT128_TEST_DETECT_COMPOUND

// The valid ones are detected, so a false below means genuinely forbidden and not a broken idiom
static_assert(has_add<uint128, double>::value, "u + d is valid");
static_assert(has_add_eq<uint128, double>::value, "u += d is valid");
static_assert(has_mod<uint128, int>::value, "u % int is still valid");
static_assert(has_shl_eq<uint128, int>::value, "u <<= int is still valid");

#define BOOST_INT128_TEST_FORBIDDEN(Int128, Float)                                               \
    static_assert(!has_mod<Int128, Float>::value, "no int % float");                             \
    static_assert(!has_mod<Float, Int128>::value, "no float % int");                             \
    static_assert(!has_and<Int128, Float>::value, "no int & float");                             \
    static_assert(!has_and<Float, Int128>::value, "no float & int");                             \
    static_assert(!has_or<Int128, Float>::value, "no int | float");                              \
    static_assert(!has_or<Float, Int128>::value, "no float | int");                              \
    static_assert(!has_xor<Int128, Float>::value, "no int ^ float");                             \
    static_assert(!has_xor<Float, Int128>::value, "no float ^ int");                             \
    static_assert(!has_shl<Int128, Float>::value, "no int << float");                            \
    static_assert(!has_shl<Float, Int128>::value, "no float << int");                            \
    static_assert(!has_shr<Int128, Float>::value, "no int >> float");                            \
    static_assert(!has_shr<Float, Int128>::value, "no float >> int");                            \
    static_assert(!has_mod_eq<Int128, Float>::value, "no int %= float");                         \
    static_assert(!has_and_eq<Int128, Float>::value, "no int &= float");                         \
    static_assert(!has_or_eq<Int128, Float>::value, "no int |= float");                          \
    static_assert(!has_xor_eq<Int128, Float>::value, "no int ^= float");                         \
    static_assert(!has_shl_eq<Int128, Float>::value, "no int <<= float");                        \
    static_assert(!has_shr_eq<Int128, Float>::value, "no int >>= float");                        \
    static_assert(!has_mod_eq<Float, Int128>::value, "no float %= int");                         \
    static_assert(!has_and_eq<Float, Int128>::value, "no float &= int");                         \
    static_assert(!has_shl_eq<Float, Int128>::value, "no float <<= int");

BOOST_INT128_TEST_FORBIDDEN(uint128, float)
BOOST_INT128_TEST_FORBIDDEN(uint128, double)
BOOST_INT128_TEST_FORBIDDEN(int128, float)
BOOST_INT128_TEST_FORBIDDEN(int128, double)

#ifndef BOOST_INT128_HAS_GPU_SUPPORT
BOOST_INT128_TEST_FORBIDDEN(uint128, long_double)
BOOST_INT128_TEST_FORBIDDEN(int128, long_double)
#endif

#undef BOOST_INT128_TEST_FORBIDDEN

// The same expressions are forbidden for the built-in, which is what we are matching
#ifdef BOOST_INT128_HAS_INT128

static_assert(!has_mod<boost::int128::detail::builtin_u128, double>::value, "builtin forbids % too");
static_assert(!has_and<boost::int128::detail::builtin_i128, double>::value, "builtin forbids & too");
static_assert(!has_shl_eq<boost::int128::detail::builtin_u128, double>::value, "builtin forbids <<= too");
static_assert(has_add<boost::int128::detail::builtin_u128, double>::value, "builtin allows + ");
static_assert(has_add_eq<boost::int128::detail::builtin_u128, double>::value, "builtin allows +=");

#endif // BOOST_INT128_HAS_INT128

// =========================================================================
// Known answers, on every platform
// =========================================================================

// Compares the value bytes of two floating point values. Results are compared as bit patterns
// rather than with == so that a NaN result matches a NaN result and -0.0 does not match 0.0.
// x87 long double occupies a 12 or 16 byte object with only 10 value bytes, so the padding must
// not take part in the comparison
template <typename Float>
bool same_bits(const Float lhs, const Float rhs) noexcept
{
    constexpr std::size_t value_bytes {std::numeric_limits<Float>::digits == 64 ? 10U : sizeof(Float)};

    unsigned char l[sizeof(Float)] {};
    unsigned char r[sizeof(Float)] {};
    std::memcpy(l, &lhs, sizeof(Float));
    std::memcpy(r, &rhs, sizeof(Float));

    return std::memcmp(l, r, value_bytes) == 0;
}

// IBM double-double, the default long double on ppc64le, is the one format here that a bit
// comparison cannot be applied to. A multiplication of that type is expanded either inline or
// through the libgcc routine depending on the surrounding code, and the two disagree in the
// last bits of the low double, so two built-in expressions computing the same product are not
// guaranteed to agree either. Its 106 bit significand identifies it: every other format in
// play has 24, 53, 64, or 113 bits
template <typename Float>
struct bitwise_reproducible
{
    static constexpr bool value {std::numeric_limits<Float>::digits != 106};
};

// Agreement well inside the last bits of the format, used only where the above does not hold.
// A wrong conversion or a truncated operand would be off by far more than this
template <typename Float>
bool nearly_same(const Float lhs, const Float rhs) noexcept
{
    if (lhs != lhs || rhs != rhs)
    {
        return (lhs != lhs) && (rhs != rhs);
    }

    if (lhs == rhs)
    {
        return true;
    }

    const Float diff {lhs > rhs ? lhs - rhs : rhs - lhs};
    const Float scale {lhs > Float{0} ? lhs : -lhs};

    return diff <= scale * static_cast<Float>(1e-30L);
}

template <typename Float>
bool same_result(const Float lhs, const Float rhs) noexcept
{
    if (bitwise_reproducible<Float>::value)
    {
        return same_bits(lhs, rhs);
    }

    return nearly_same(lhs, rhs);
}

// Keeps a floating point value out of the constant folder so that the x87 80-bit registers
// cannot make a comparison pass or fail differently from the stored value
template <typename Float>
Float opaque(const Float v) noexcept
{
    volatile Float s {v};
    return s;
}

// The set of floating point operands each parity sweep runs against: exactly representable
// small values, values past the significand in both directions, and the special values
template <typename Float>
struct float_operands
{
    static constexpr std::size_t size {14U};

    Float values[size];

    float_operands() noexcept
    {
        // Every one of these is an explicit cast so that neither the narrowing to float nor
        // the widening to long double trips -Wconversion or -Wdouble-promotion
        values[0]  = static_cast<Float>(0.0L);
        values[1]  = static_cast<Float>(1.0L);
        values[2]  = static_cast<Float>(-1.0L);
        values[3]  = static_cast<Float>(0.5L);
        values[4]  = static_cast<Float>(-0.5L);
        values[5]  = static_cast<Float>(3.0L);
        values[6]  = static_cast<Float>(1e18L);
        values[7]  = static_cast<Float>(-1e18L);
        values[8]  = static_cast<Float>(1e30L);
        values[9]  = std::numeric_limits<Float>::denorm_min();
        values[10] = std::numeric_limits<Float>::max();
        values[11] = std::numeric_limits<Float>::infinity();
        values[12] = -std::numeric_limits<Float>::infinity();
        values[13] = std::numeric_limits<Float>::quiet_NaN();
    }
};

void test_known_answers()
{
    // The expression from the original report
    BOOST_TEST(same_bits(uint128{5} + 1.0, 6.0));
    BOOST_TEST(same_bits(1.0 + uint128{5}, 6.0));
    BOOST_TEST(same_bits(int128{-5} + 1.0, -4.0));
    BOOST_TEST(same_bits(1.0 + int128{-5}, -4.0));

    // A fractional operand is not truncated before the operation
    BOOST_TEST(same_bits(uint128{1} + 0.5, 1.5));
    BOOST_TEST(same_bits(uint128{5} * 0.5, 2.5));
    BOOST_TEST(same_bits(uint128{5} / 2.0, 2.5));
    BOOST_TEST(same_bits(2.0F - uint128{5}, -3.0F));
    BOOST_TEST(same_bits(int128{-1} * 0.5, -0.5));

    // Values beyond the significand round on conversion, exactly as a cast would
    BOOST_TEST(same_bits(uint128{UINT64_MAX} * 2.0, 18446744073709551615.0 * 2.0));
    BOOST_TEST(same_bits((uint128{1} << 100U) + 0.0, 1267650600228229401496703205376.0));

    // The comparison happens after the integer is converted, so it is lossy in exactly the
    // way the built-in comparison is: 2^64 + 1 rounds to 2^64 as a double
    const uint128 two_64 {uint128{1} << 64U};
    BOOST_TEST((two_64 + uint128{1}) == 18446744073709551616.0);
    BOOST_TEST(!((two_64 + uint128{1}) != 18446744073709551616.0));

    // Ordering against the special values
    const double inf {opaque(std::numeric_limits<double>::infinity())};
    const double nan_v {opaque(std::numeric_limits<double>::quiet_NaN())};

    BOOST_TEST(uint128{1} < inf);
    BOOST_TEST(!(uint128{1} > inf));
    BOOST_TEST(uint128{1} > -inf);
    BOOST_TEST(int128{-1} > -inf);

    // -ffinite-math-only promises the compiler that no NaN can occur, so it folds these away
    #if !defined(__FINITE_MATH_ONLY__) || __FINITE_MATH_ONLY__ == 0

    BOOST_TEST(!(uint128{1} < nan_v));
    BOOST_TEST(!(uint128{1} > nan_v));
    BOOST_TEST(!(uint128{1} <= nan_v));
    BOOST_TEST(!(uint128{1} >= nan_v));
    BOOST_TEST(!(uint128{1} == nan_v));
    BOOST_TEST(uint128{1} != nan_v);
    BOOST_TEST(!(nan_v < int128{1}));
    BOOST_TEST(nan_v != int128{1});

    #endif

    #ifdef BOOST_INT128_HAS_SPACESHIP_OPERATOR

    BOOST_TEST((uint128{1} <=> 2.0) == std::partial_ordering::less);
    BOOST_TEST((uint128{2} <=> 2.0) == std::partial_ordering::equivalent);
    BOOST_TEST((uint128{3} <=> 2.0) == std::partial_ordering::greater);
    BOOST_TEST((2.0 <=> uint128{1}) == std::partial_ordering::greater);
    BOOST_TEST((int128{-1} <=> 0.0) == std::partial_ordering::less);

    #if !defined(__FINITE_MATH_ONLY__) || __FINITE_MATH_ONLY__ == 0
    BOOST_TEST((uint128{1} <=> nan_v) == std::partial_ordering::unordered);
    BOOST_TEST((nan_v <=> int128{1}) == std::partial_ordering::unordered);
    #endif

    #endif // BOOST_INT128_HAS_SPACESHIP_OPERATOR
}

void test_compound_assignment()
{
    // The value is converted to the floating point type before the operation, so a fractional
    // operand contributes. Previously the operand was truncated first and these were wrong
    uint128 a {4};
    a *= 2.5;
    BOOST_TEST_EQ(a, uint128{10});

    uint128 b {3};
    b /= 0.5;
    BOOST_TEST_EQ(b, uint128{6});

    uint128 c {1};
    c += 0.75;
    BOOST_TEST_EQ(c, uint128{1});

    int128 d {5};
    d += -1.5;
    BOOST_TEST_EQ(d, int128{3});

    int128 e {-7};
    e *= 2.5;
    BOOST_TEST_EQ(e, int128{-17});

    int128 f {10};
    f -= 2.5F;
    BOOST_TEST_EQ(f, int128{7});

    // Where the built-in would be undefined, the floating point constructor's saturating
    // behavior applies instead
    uint128 g {0};
    g -= 1.5;
    BOOST_TEST_EQ(g, uint128{0});

    // A floating point left operand keeps its own type
    double h {1.0};
    h += uint128{2};
    BOOST_TEST(same_bits(h, 3.0));

    float i {10.0F};
    i /= int128{-4};
    BOOST_TEST(same_bits(i, -2.5F));

    long double j {1.5L};
    j *= uint128{2};
    BOOST_TEST(same_bits(j, 3.0L));
}

void test_constexpr()
{
    static_assert(uint128{2} + 0.5 == 2.5, "constexpr add");
    static_assert(uint128{5} * 2.0 == 10.0, "constexpr mul");
    static_assert(int128{-2} - 0.5 == -2.5, "constexpr sub");
    static_assert(1.0 / int128{-2} == -0.5, "constexpr div");
    static_assert(uint128{3} < 3.5, "constexpr less");
    static_assert(!(int128{-1} >= 0.0), "constexpr ge");
}

} // namespace

// =========================================================================
// Bit exact parity against the built-in
// =========================================================================

#ifdef BOOST_INT128_HAS_INT128

namespace {

using boost::int128::detail::builtin_i128;
using boost::int128::detail::builtin_u128;

std::mt19937_64 rng {42};
constexpr std::size_t N {512U};

builtin_u128 random_u128()
{
    return (static_cast<builtin_u128>(rng()) << 64) | static_cast<builtin_u128>(rng());
}

// Every operand order and operator, compared against the identical built-in expression
template <typename Float, typename Lib, typename Builtin>
void check_one(const Lib lib, const Builtin raw, const Float f)
{
    const Float converted {static_cast<Float>(raw)};

    BOOST_TEST(same_result<Float>(lib + f, converted + f));
    BOOST_TEST(same_result<Float>(f + lib, f + converted));
    BOOST_TEST(same_result<Float>(lib - f, converted - f));
    BOOST_TEST(same_result<Float>(f - lib, f - converted));
    BOOST_TEST(same_result<Float>(lib * f, converted * f));
    BOOST_TEST(same_result<Float>(f * lib, f * converted));
    BOOST_TEST(same_result<Float>(lib / f, converted / f));
    BOOST_TEST(same_result<Float>(f / lib, f / converted));

    BOOST_TEST_EQ(lib == f, converted == f);
    BOOST_TEST_EQ(lib != f, converted != f);
    BOOST_TEST_EQ(lib <  f, converted <  f);
    BOOST_TEST_EQ(lib <= f, converted <= f);
    BOOST_TEST_EQ(lib >  f, converted >  f);
    BOOST_TEST_EQ(lib >= f, converted >= f);

    BOOST_TEST_EQ(f == lib, f == converted);
    BOOST_TEST_EQ(f != lib, f != converted);
    BOOST_TEST_EQ(f <  lib, f <  converted);
    BOOST_TEST_EQ(f <= lib, f <= converted);
    BOOST_TEST_EQ(f >  lib, f >  converted);
    BOOST_TEST_EQ(f >= lib, f >= converted);

    #ifdef BOOST_INT128_HAS_SPACESHIP_OPERATOR
    BOOST_TEST((lib <=> f) == (converted <=> f));
    BOOST_TEST((f <=> lib) == (f <=> converted));
    #endif
}

// A spread of magnitudes: below the significand, straddling 2^53 and 2^64 where the rounding
// of the conversion starts to matter, and up against the limits of the type
template <typename Float>
void test_parity_unsigned()
{
    const float_operands<Float> operands {};

    const builtin_u128 fixed[] {
        0,
        1,
        (static_cast<builtin_u128>(1) << 53) - 1,
        static_cast<builtin_u128>(1) << 53,
        (static_cast<builtin_u128>(1) << 53) + 1,
        (static_cast<builtin_u128>(1) << 64) - 1,
        static_cast<builtin_u128>(1) << 64,
        (static_cast<builtin_u128>(1) << 64) + 1,
        static_cast<builtin_u128>(1) << 127,
        ~static_cast<builtin_u128>(0)
    };

    for (const builtin_u128 raw : fixed)
    {
        for (const Float f : operands.values)
        {
            check_one(uint128{raw}, raw, f);
        }
    }

    for (std::size_t i {0}; i < N; ++i)
    {
        builtin_u128 raw {random_u128()};
        raw >>= (rng() % 128U);

        for (const Float f : operands.values)
        {
            check_one(uint128{raw}, raw, f);
        }
    }
}

template <typename Float>
void test_parity_signed()
{
    const float_operands<Float> operands {};

    const builtin_i128 fixed[] {
        0,
        1,
        -1,
        static_cast<builtin_i128>((static_cast<builtin_u128>(1) << 53) + 1),
        -static_cast<builtin_i128>((static_cast<builtin_u128>(1) << 53) + 1),
        static_cast<builtin_i128>((static_cast<builtin_u128>(1) << 64) + 1),
        -static_cast<builtin_i128>((static_cast<builtin_u128>(1) << 64) + 1),
        static_cast<builtin_i128>(~(static_cast<builtin_u128>(1) << 127)),
        static_cast<builtin_i128>(static_cast<builtin_u128>(1) << 127)
    };

    for (const builtin_i128 raw : fixed)
    {
        for (const Float f : operands.values)
        {
            check_one(int128{raw}, raw, f);
        }
    }

    for (std::size_t i {0}; i < N; ++i)
    {
        builtin_u128 bits {random_u128()};
        bits >>= (rng() % 128U);
        const builtin_i128 raw {static_cast<builtin_i128>(i % 2U == 0U ? bits : ~bits)};

        for (const Float f : operands.values)
        {
            check_one(int128{raw}, raw, f);
        }
    }
}

// Compound assignment against the built-in, restricted to the range where the built-in is
// defined so that its undefined out of range conversions are not the oracle
// Converting a floating point value that is negative, NaN, or too large back to the integer
// type is undefined for the built-in, and what it actually produces varies by architecture:
// x86-64 yields the integer indefinite value while aarch64 saturates in hardware. The library
// always saturates, per the floating point constructor, so those inputs cannot be compared
// against the built-in and are covered by test_compound_assignment instead
bool in_unsigned_range(const double v) noexcept
{
    // 2^128 exactly, the first value that does not fit
    return v >= 0.0 && v < 340282366920938463463374607431768211456.0;
}

void test_parity_compound()
{
    const double operands[] {0.0, 1.0, -1.0, 0.5, -0.5, 3.0, 1e18};

    for (std::size_t i {0}; i < N; ++i)
    {
        const builtin_u128 raw {random_u128() >> 4U};
        const double converted {static_cast<double>(raw)};

        for (const double f : operands)
        {
            if (in_unsigned_range(converted + f))
            {
                uint128 lib {raw};
                lib += f;
                BOOST_TEST_EQ(lib, uint128{static_cast<builtin_u128>(converted + f)});
            }

            if (in_unsigned_range(converted * f))
            {
                uint128 lib {raw};
                lib *= f;
                BOOST_TEST_EQ(lib, uint128{static_cast<builtin_u128>(converted * f)});
            }

            if (in_unsigned_range(converted - f))
            {
                uint128 lib {raw};
                lib -= f;
                BOOST_TEST_EQ(lib, uint128{static_cast<builtin_u128>(converted - f)});
            }

            // A floating point left operand always keeps its own type, so it is always in range
            double lib {f};
            double oracle {f};
            lib += uint128{raw};
            oracle += converted;
            BOOST_TEST(same_bits(lib, oracle));
        }
    }
}

} // namespace

#endif // BOOST_INT128_HAS_INT128

int main()
{
    test_known_answers();
    test_compound_assignment();
    test_constexpr();

    #ifdef BOOST_INT128_HAS_INT128

    test_parity_unsigned<float>();
    test_parity_unsigned<double>();
    test_parity_signed<float>();
    test_parity_signed<double>();

    #ifndef BOOST_INT128_HAS_GPU_SUPPORT
    test_parity_unsigned<long double>();
    test_parity_signed<long double>();
    #endif

    test_parity_compound();

    #endif // BOOST_INT128_HAS_INT128

    return boost::report_errors();
}

#if defined(__clang__)
#  pragma clang diagnostic pop
#elif defined(__GNUC__)
#  pragma GCC diagnostic pop
#endif
