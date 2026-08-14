// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
//
// Pins the storage layout of int128 and uint128. Both halves of int128 are
// deliberately std::uint64_t so that the two words have the same type; when they
// differ the vectorizer cannot merge them and loops over int128 are scalarized.
// Sign is recovered through signed_high(), which these tests also exercise.

#ifndef BOOST_INT128_BUILD_MODULE

#include <boost/int128.hpp>

#else

import boost.int128;
#include <boost/int128/detail/literal_macros.hpp>

#endif

#include <boost/core/lightweight_test.hpp>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <type_traits>

using boost::int128::int128;
using boost::int128::uint128;

// Both words of both types are unsigned. Changing either of these regresses the
// codegen this layout exists to enable.
static_assert(std::is_same<decltype(int128{}.high), std::uint64_t>::value,
              "int128::high must be std::uint64_t; use signed_high() to read it as signed");
static_assert(std::is_same<decltype(int128{}.low), std::uint64_t>::value,
              "int128::low must be std::uint64_t");
static_assert(std::is_same<decltype(uint128{}.high), std::uint64_t>::value,
              "uint128::high must be std::uint64_t");
static_assert(std::is_same<decltype(uint128{}.low), std::uint64_t>::value,
              "uint128::low must be std::uint64_t");

static_assert(std::is_same<decltype(int128{}.signed_high()), std::int64_t>::value,
              "signed_high() must yield std::int64_t");

static_assert(sizeof(int128) == 16, "int128 must be exactly 128 bits wide");
static_assert(sizeof(uint128) == 16, "uint128 must be exactly 128 bits wide");
static_assert(alignof(int128) == alignof(uint128), "both types must agree on alignment");

static_assert(std::is_trivially_copyable<int128>::value, "int128 must be trivially copyable");
static_assert(std::is_trivially_copyable<uint128>::value, "uint128 must be trivially copyable");
static_assert(std::is_standard_layout<int128>::value, "int128 must be standard layout");
static_assert(std::is_standard_layout<uint128>::value, "uint128 must be standard layout");

// The words sit in memory in the platform's own order, so a memcpy of the object
// matches a memcpy of the two words in that order.
void test_word_order()
{
    const int128 v {INT64_C(-2), UINT64_C(0x0123456789ABCDEF)};

    unsigned char from_object[sizeof(int128)] {};
    std::memcpy(from_object, &v, sizeof(v));

    unsigned char from_words[sizeof(int128)] {};
    #if BOOST_INT128_ENDIAN_LITTLE_BYTE
    std::memcpy(from_words, &v.low, sizeof(v.low));
    std::memcpy(from_words + sizeof(v.low), &v.high, sizeof(v.high));
    #else
    std::memcpy(from_words, &v.high, sizeof(v.high));
    std::memcpy(from_words + sizeof(v.high), &v.low, sizeof(v.low));
    #endif

    BOOST_TEST_EQ(std::memcmp(from_object, from_words, sizeof(int128)), 0);
}

// signed_high() is a pure reinterpretation of the stored bits.
void test_signed_high_round_trip()
{
    const std::int64_t highs[] {
        INT64_C(0), INT64_C(1), INT64_C(-1), INT64_C(2), INT64_C(-2),
        INT64_MAX, INT64_MIN, INT64_MIN + 1, INT64_MAX - 1, INT64_C(-1000003)
    };

    for (const auto h : highs)
    {
        const int128 v {h, UINT64_C(0xDEADBEEFCAFEF00D)};

        BOOST_TEST_EQ(v.signed_high(), h);
        BOOST_TEST_EQ(v.high, static_cast<std::uint64_t>(h));
        BOOST_TEST_EQ(static_cast<std::int64_t>(v.high), h);

        // The sign of the value is the sign of the high word.
        BOOST_TEST_EQ(v < 0, h < 0);
        BOOST_TEST_EQ(v.signed_high() < 0, h < 0);
    }
}

// The two types share a representation, so converting between them moves no bits.
void test_interconversion_is_bit_preserving()
{
    const int128 signed_values[] {
        int128{0, 0},
        int128{-1, UINT64_MAX},
        int128{INT64_MIN, 0},
        int128{INT64_MAX, UINT64_MAX},
        int128{-1, 0},
        int128{0, UINT64_MAX}
    };

    for (const auto& s : signed_values)
    {
        const uint128 u {s};
        BOOST_TEST_EQ(u.high, s.high);
        BOOST_TEST_EQ(u.low, s.low);

        const int128 back {u};
        BOOST_TEST_EQ(back.high, s.high);
        BOOST_TEST_EQ(back.low, s.low);
    }
}

// The two word constructor still takes its high word signed, so brace initialization
// from a negative literal keeps working.
void test_two_word_construction()
{
    constexpr int128 minus_one {-1, UINT64_MAX};
    static_assert(minus_one.high == UINT64_MAX, "-1 sign extends into the high word");
    static_assert(minus_one.signed_high() == -1, "signed_high sees the sign");

    constexpr int128 int128_min {INT64_MIN, 0};
    static_assert(int128_min.high == (UINT64_C(1) << 63), "INT128_MIN high word");
    static_assert(int128_min.signed_high() == INT64_MIN, "INT128_MIN reads back signed");

    BOOST_TEST(minus_one == -1);
    BOOST_TEST(int128_min == (std::numeric_limits<int128>::min)());
}

int main()
{
    test_word_order();
    test_signed_high_round_trip();
    test_interconversion_is_bit_preserving();
    test_two_word_construction();

    return boost::report_errors();
}
