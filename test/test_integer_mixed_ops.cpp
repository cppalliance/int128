// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
//
// Mixed operations between the library types and the built-in integer types, with the
// built-in integer on the left of a compound assignment.
//
// The builtin applies the operation to the common type of the two operands and converts the
// result back to the type of the left operand, so every expression here must produce exactly
// what the built-in __int128 / unsigned __int128 produces for the same expression. The shift
// operators are the exception the language makes: they take the value and the result type
// from the left operand alone, and only the count from the right.

#include <boost/int128.hpp>
#include <boost/core/lightweight_test.hpp>
#include <cstdint>
#include <limits>
#include <random>
#include <type_traits>

using boost::int128::int128;
using boost::int128::uint128;

// =========================================================================
// Result types, on every platform
// =========================================================================

// Compound assignment keeps the type of the left operand and yields an lvalue reference to it
#define BOOST_INT128_TEST_COMPOUND_TYPE(Integer, Int128)                                                          \
    static_assert(std::is_same<decltype(std::declval<Integer&>() |= Int128{}), Integer&>::value, "i |= v");        \
    static_assert(std::is_same<decltype(std::declval<Integer&>() &= Int128{}), Integer&>::value, "i &= v");        \
    static_assert(std::is_same<decltype(std::declval<Integer&>() ^= Int128{}), Integer&>::value, "i ^= v");        \
    static_assert(std::is_same<decltype(std::declval<Integer&>() += Int128{}), Integer&>::value, "i += v");        \
    static_assert(std::is_same<decltype(std::declval<Integer&>() -= Int128{}), Integer&>::value, "i -= v");        \
    static_assert(std::is_same<decltype(std::declval<Integer&>() *= Int128{}), Integer&>::value, "i *= v");        \
    static_assert(std::is_same<decltype(std::declval<Integer&>() /= Int128{}), Integer&>::value, "i /= v");        \
    static_assert(std::is_same<decltype(std::declval<Integer&>() %= Int128{}), Integer&>::value, "i %= v");        \
    static_assert(std::is_same<decltype(std::declval<Integer&>() <<= Int128{}), Integer&>::value, "i <<= v");      \
    static_assert(std::is_same<decltype(std::declval<Integer&>() >>= Int128{}), Integer&>::value, "i >>= v");

#define BOOST_INT128_TEST_COMPOUND_TYPES(Integer)      \
    BOOST_INT128_TEST_COMPOUND_TYPE(Integer, uint128)  \
    BOOST_INT128_TEST_COMPOUND_TYPE(Integer, int128)

// long long{} does not parse, so the macros need a single token type name
using signed_char = signed char;
using unsigned_char = unsigned char;
using unsigned_short = unsigned short;
using unsigned_int = unsigned int;
using unsigned_long = unsigned long;
using long_long = long long;
using unsigned_long_long = unsigned long long;

BOOST_INT128_TEST_COMPOUND_TYPES(bool)
BOOST_INT128_TEST_COMPOUND_TYPES(char)
BOOST_INT128_TEST_COMPOUND_TYPES(signed_char)
BOOST_INT128_TEST_COMPOUND_TYPES(unsigned_char)
BOOST_INT128_TEST_COMPOUND_TYPES(short)
BOOST_INT128_TEST_COMPOUND_TYPES(unsigned_short)
BOOST_INT128_TEST_COMPOUND_TYPES(int)
BOOST_INT128_TEST_COMPOUND_TYPES(unsigned_int)
BOOST_INT128_TEST_COMPOUND_TYPES(long)
BOOST_INT128_TEST_COMPOUND_TYPES(unsigned_long)
BOOST_INT128_TEST_COMPOUND_TYPES(long_long)
BOOST_INT128_TEST_COMPOUND_TYPES(unsigned_long_long)

#ifdef BOOST_INT128_HAS_INT128

using builtin_i128 = boost::int128::detail::builtin_i128;
using builtin_u128 = boost::int128::detail::builtin_u128;

BOOST_INT128_TEST_COMPOUND_TYPES(builtin_i128)
BOOST_INT128_TEST_COMPOUND_TYPES(builtin_u128)

#endif // BOOST_INT128_HAS_INT128

#undef BOOST_INT128_TEST_COMPOUND_TYPES
#undef BOOST_INT128_TEST_COMPOUND_TYPE

// A shift takes its result type from the left operand, so a type that integral promotion
// leaves alone keeps its own type
static_assert(std::is_same<decltype(int{} << uint128{}), int>::value, "int << u");
static_assert(std::is_same<decltype(unsigned_int{} >> uint128{}), unsigned int>::value, "unsigned >> u");
static_assert(std::is_same<decltype(long_long{} << int128{}), long long>::value, "long long << i");
static_assert(std::is_same<decltype(unsigned_long_long{} >> int128{}), unsigned long long>::value, "unsigned long long >> i");

// Every other operator yields the common type, which is the 128-bit type
static_assert(std::is_same<decltype(int{} + uint128{}), uint128>::value, "int + u");
static_assert(std::is_same<decltype(unsigned_long_long{} / uint128{}), uint128>::value, "unsigned long long / u");
static_assert(std::is_same<decltype(int{} | int128{}), int128>::value, "int | i");

// A floating point left operand still resolves to the floating point overloads
static_assert(std::is_same<decltype(std::declval<double&>() += uint128{}), double&>::value, "d += u");
static_assert(std::is_same<decltype(std::declval<float&>() *= int128{}), float&>::value, "f *= i");

namespace {

// =========================================================================
// The operators the built-in forbids on a floating point operand stay forbidden
// =========================================================================

template <typename...>
struct make_void { using type = void; };

template <typename... Ts>
using void_t = typename make_void<Ts...>::type;

template <typename T, typename U, typename = void>
struct has_mod_eq : std::false_type {};

template <typename T, typename U>
struct has_mod_eq<T, U, void_t<decltype(std::declval<T&>() %= std::declval<U>())>> : std::true_type {};

template <typename T, typename U, typename = void>
struct has_shl_eq : std::false_type {};

template <typename T, typename U>
struct has_shl_eq<T, U, void_t<decltype(std::declval<T&>() <<= std::declval<U>())>> : std::true_type {};

// The integer forms are detected, so a false below means genuinely forbidden and not a broken idiom
static_assert(has_mod_eq<int, uint128>::value, "int %= u is valid");
static_assert(has_shl_eq<int, uint128>::value, "int <<= u is valid");
static_assert(has_mod_eq<unsigned_int, int128>::value, "unsigned %= i is valid");

static_assert(!has_mod_eq<uint128, double>::value, "no u %= d");
static_assert(!has_shl_eq<uint128, double>::value, "no u <<= d");
static_assert(!has_mod_eq<double, uint128>::value, "no d %= u");
static_assert(!has_shl_eq<double, int128>::value, "no d <<= i");

// =========================================================================
// Known answers, on every platform
// =========================================================================

void test_known_answers()
{
    // The expression from the original report
    {
        unsigned i {0};
        i |= uint128{1};
        BOOST_TEST_EQ(i, 1U);
    }

    // Every operator, on a value that needs no truncation
    {
        int i {12};
        i |= uint128{3};
        BOOST_TEST_EQ(i, 15);

        i = 12;
        i &= uint128{10};
        BOOST_TEST_EQ(i, 8);

        i = 12;
        i ^= uint128{10};
        BOOST_TEST_EQ(i, 6);

        i = 12;
        i += int128{5};
        BOOST_TEST_EQ(i, 17);

        i = 12;
        i -= int128{5};
        BOOST_TEST_EQ(i, 7);

        i = 12;
        i *= int128{5};
        BOOST_TEST_EQ(i, 60);

        i = 12;
        i /= int128{5};
        BOOST_TEST_EQ(i, 2);

        i = 12;
        i %= int128{5};
        BOOST_TEST_EQ(i, 2);

        i = 12;
        i <<= uint128{2};
        BOOST_TEST_EQ(i, 48);

        i = 12;
        i >>= uint128{2};
        BOOST_TEST_EQ(i, 3);
    }

    // The operation happens in the common type and only the result is converted back, so a
    // result outside the range of the left operand wraps exactly as the builtin wraps
    {
        std::uint8_t byte {200};
        byte += uint128{100};
        BOOST_TEST_EQ(byte, 44U);           // 300 modulo 256

        std::uint32_t word {1};
        word -= uint128{2};
        BOOST_TEST_EQ(word, UINT32_MAX);

        std::uint64_t big {0};
        big -= uint128{1};
        BOOST_TEST_EQ(big, UINT64_MAX);     // the low word of the 128-bit result
    }

    // A right operand too large for the left operand is not truncated before the operation
    {
        std::uint32_t word {0};
        word += (uint128{1} << 64U) + uint128{7};
        BOOST_TEST_EQ(word, 7U);            // the low word of 2^64 + 7

        std::uint64_t big {8};
        big /= (uint128{1} << 64U);
        BOOST_TEST_EQ(big, UINT64_C(0));    // 8 / 2^64 is zero, not a division by 2^64 truncated to 0
    }

    // A signed left operand is sign extended into the common type first
    {
        int i {-1};
        i /= uint128{2};
        // -1 becomes 2^128 - 1, so the quotient is 2^127 - 1 and its low word is all ones
        BOOST_TEST_EQ(i, -1);

        int j {-8};
        j /= int128{2};
        BOOST_TEST_EQ(j, -4);               // the signed common type divides as a signed type

        int k {-1};
        k += uint128{1};
        BOOST_TEST_EQ(k, 0);
    }

    // The shift takes the count from the 128-bit operand and the value from the left operand
    {
        std::uint32_t word {1};
        word <<= uint128{31};
        BOOST_TEST_EQ(word, UINT32_C(0x80000000));

        std::uint64_t big {UINT64_MAX};
        big >>= int128{63};
        BOOST_TEST_EQ(big, UINT64_C(1));

        // A count whose value is far larger than 64 bits still only reads the low word,
        // matching the builtin, which is undefined for any count at or above the width
        std::uint32_t narrow {0xFFFFFFFFU};
        narrow >>= uint128{16};
        BOOST_TEST_EQ(narrow, UINT32_C(0xFFFF));
    }

    // A bool left operand is promoted and the result converted back, again as for the builtin
    {
        bool b {false};
        b |= uint128{2};
        BOOST_TEST_EQ(b, true);

        bool c {true};
        c &= uint128{2};
        BOOST_TEST_EQ(c, false);            // 1 & 2 is 0
    }

    // The 128-bit left operands lose nothing
    {
        uint128 wide {0};
        wide += uint128{5};
        BOOST_TEST_EQ(wide, uint128{5});

        #ifdef BOOST_INT128_HAS_INT128

        builtin_u128 native {0};
        native |= (uint128{1} << 100U);
        BOOST_TEST(native == (static_cast<builtin_u128>(1) << 100));

        builtin_i128 signed_native {-8};
        signed_native /= int128{2};
        BOOST_TEST(signed_native == -4);

        #endif // BOOST_INT128_HAS_INT128
    }
}

// =========================================================================
// Constant evaluation
// =========================================================================

constexpr unsigned constexpr_or(unsigned seed) noexcept
{
    unsigned value {seed};
    value |= uint128{1};
    return value;
}

constexpr int constexpr_chain(int seed) noexcept
{
    int value {seed};
    value += int128{10};
    value *= uint128{3};
    value -= int128{2};
    value <<= uint128{1};
    value >>= int128{1};
    value /= uint128{4};
    value %= int128{7};
    return value;
}

void test_constexpr()
{
    static_assert(constexpr_or(0U) == 1U, "constexpr |=");
    static_assert(constexpr_or(6U) == 7U, "constexpr |=");

    // ((5 + 10) * 3 - 2) = 43, unchanged by the shifts, 43 / 4 = 10, 10 % 7 = 3
    static_assert(constexpr_chain(5) == 3, "constexpr chain");

    BOOST_TEST_EQ(constexpr_or(0U), 1U);
    BOOST_TEST_EQ(constexpr_chain(5), 3);
}

// =========================================================================
// Parity with the builtin, where the compiler has one
// =========================================================================

#ifdef BOOST_INT128_HAS_INT128

// The oracle expressions below are the built-in compound assignments this file exists to match.
// Converting the 128-bit result back to a narrower left operand, and converting a signed operand
// to the unsigned common type, are the behavior under test rather than defects
#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wsign-conversion"
#  pragma clang diagnostic ignored "-Wconversion"
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wsign-conversion"
#  pragma GCC diagnostic ignored "-Wconversion"
#endif

// Compares as a bit pattern, which is exact for every type here and prints on failure
template <typename Integer>
bool same_value(const Integer lib_value, const Integer oracle_value) noexcept
{
    return uint128{lib_value} == uint128{oracle_value};
}

// The width the builtin shifts in, which is the width of the promoted left operand
template <typename Integer>
constexpr unsigned promoted_width() noexcept
{
    return sizeof(Integer) < sizeof(int) ? sizeof(int) * 8U : sizeof(Integer) * 8U;
}

// Runs one compound assignment through the library type and through the builtin and compares.
// Integer is the left operand type, Library is uint128 or int128 and Builtin is the built-in
// type of the same signedness
#define BOOST_INT128_TEST_PARITY(Integer, Library, Builtin, compound_op, a, b)             \
    {                                                                                      \
        Integer lib_value {static_cast<Integer>(a)};                                       \
        Integer oracle_value {static_cast<Integer>(a)};                                    \
        lib_value compound_op Library{b};                                                  \
        oracle_value compound_op static_cast<Builtin>(b);                                  \
        BOOST_TEST(same_value(lib_value, oracle_value));                                   \
    }

#define BOOST_INT128_TEST_PARITY_BOTH(Integer, compound_op, a, b)                          \
    BOOST_INT128_TEST_PARITY(Integer, uint128, builtin_u128, compound_op, a, b)            \
    BOOST_INT128_TEST_PARITY(Integer, int128, builtin_i128, compound_op, a, b)

// Values chosen to cross every word and sign boundary the operators care about
const builtin_i128 parity_values[] {
    0,
    1,
    2,
    3,
    42,
    -1,
    -2,
    -42,
    1000003,
    static_cast<builtin_i128>(INT64_MAX),
    static_cast<builtin_i128>(INT64_MIN),
    static_cast<builtin_i128>(UINT64_MAX),
    (static_cast<builtin_i128>(1) << 100) + 7,
    -((static_cast<builtin_i128>(1) << 100) + 7)
};

template <typename Integer>
void test_parity_arithmetic()
{
    for (const auto a : parity_values)
    {
        for (const auto b : parity_values)
        {
            BOOST_INT128_TEST_PARITY_BOTH(Integer, |=, a, b)
            BOOST_INT128_TEST_PARITY_BOTH(Integer, &=, a, b)
            BOOST_INT128_TEST_PARITY_BOTH(Integer, ^=, a, b)
            BOOST_INT128_TEST_PARITY_BOTH(Integer, +=, a, b)
            BOOST_INT128_TEST_PARITY_BOTH(Integer, -=, a, b)
            BOOST_INT128_TEST_PARITY_BOTH(Integer, *=, a, b)

            // Division and remainder by zero is undefined for the builtin, so it is here too
            if (b != 0)
            {
                BOOST_INT128_TEST_PARITY_BOTH(Integer, /=, a, b)
                BOOST_INT128_TEST_PARITY_BOTH(Integer, %=, a, b)
            }
        }
    }
}

template <typename Integer>
void test_parity_shifts()
{
    constexpr unsigned width {promoted_width<Integer>()};

    // A signed left operand may not shift a bit into the sign bit, so stop one short of it
    const unsigned max_left_count {std::numeric_limits<Integer>::is_signed ? width - 2U : width - 1U};

    for (unsigned count {0}; count <= max_left_count; ++count)
    {
        // 42 needs six bits, so use 1 where the shifted result would leave the promoted type
        const builtin_i128 a {count + 6U < width ? builtin_i128{42} : builtin_i128{1}};

        BOOST_INT128_TEST_PARITY_BOTH(Integer, <<=, a, count)
    }

    for (const auto a : parity_values)
    {
        for (unsigned count {0}; count < width; ++count)
        {
            BOOST_INT128_TEST_PARITY_BOTH(Integer, >>=, a, count)
        }
    }
}

template <typename Integer>
void test_parity_random()
{
    std::mt19937_64 rng {42};
    std::uniform_int_distribution<std::uint64_t> dist {0, UINT64_MAX};

    for (int i {0}; i < 512; ++i)
    {
        const auto a = static_cast<builtin_i128>((static_cast<builtin_u128>(dist(rng)) << 64) | dist(rng));
        const auto b = static_cast<builtin_i128>((static_cast<builtin_u128>(dist(rng)) << 64) | dist(rng));

        BOOST_INT128_TEST_PARITY_BOTH(Integer, |=, a, b)
        BOOST_INT128_TEST_PARITY_BOTH(Integer, &=, a, b)
        BOOST_INT128_TEST_PARITY_BOTH(Integer, ^=, a, b)
        BOOST_INT128_TEST_PARITY_BOTH(Integer, +=, a, b)
        BOOST_INT128_TEST_PARITY_BOTH(Integer, -=, a, b)
        BOOST_INT128_TEST_PARITY_BOTH(Integer, *=, a, b)

        if (b != 0)
        {
            BOOST_INT128_TEST_PARITY_BOTH(Integer, /=, a, b)
            BOOST_INT128_TEST_PARITY_BOTH(Integer, %=, a, b)
        }
    }
}

template <typename Integer>
void test_parity()
{
    test_parity_arithmetic<Integer>();
    test_parity_shifts<Integer>();
    test_parity_random<Integer>();
}

#undef BOOST_INT128_TEST_PARITY_BOTH
#undef BOOST_INT128_TEST_PARITY

#if defined(__clang__)
#  pragma clang diagnostic pop
#elif defined(__GNUC__)
#  pragma GCC diagnostic pop
#endif

#endif // BOOST_INT128_HAS_INT128

} // namespace

int main()
{
    test_known_answers();
    test_constexpr();

    #ifdef BOOST_INT128_HAS_INT128

    test_parity<char>();
    test_parity<signed char>();
    test_parity<unsigned char>();
    test_parity<short>();
    test_parity<unsigned short>();
    test_parity<int>();
    test_parity<unsigned int>();
    test_parity<long>();
    test_parity<unsigned long>();
    test_parity<long long>();
    test_parity<unsigned long long>();
    test_parity<builtin_i128>();
    test_parity<builtin_u128>();

    #endif // BOOST_INT128_HAS_INT128

    return boost::report_errors();
}
