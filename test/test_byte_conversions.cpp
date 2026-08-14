// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_BUILD_MODULE

#include <boost/int128/int128.hpp>
#include <boost/int128/byte_conversions.hpp>
#include <boost/int128/bit.hpp>
#include <boost/int128/limits.hpp>
#include <boost/int128/iostream.hpp>

#else

import boost.int128;

#endif

#include <boost/core/lightweight_test.hpp>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>

using boost::int128::uint128;
using boost::int128::int128;

static const uint128 pattern_u {UINT64_C(0x0102030405060708), UINT64_C(0x090A0B0C0D0E0F10)};

// The endian macros are not part of the module interface, so the native order is
// taken from the object representation of a known value instead
static bool native_is_little()
{
    const uint128 probe {UINT64_C(0), UINT64_C(1)};
    std::uint8_t raw[sizeof(uint128)] {};
    std::memcpy(raw, &probe, sizeof(uint128));

    return raw[0] == 1U;
}

template <typename ByteType>
void check_bytes(const std::array<ByteType, sizeof(uint128)>& actual, const std::array<unsigned, sizeof(uint128)>& expected)
{
    for (std::size_t i {}; i < actual.size(); ++i)
    {
        BOOST_TEST_EQ(static_cast<unsigned>(actual[i]), expected[i]);
    }
}

// SplitMix64 keeps the test free of any <random> dependency
static std::uint64_t next_word(std::uint64_t& state)
{
    state += UINT64_C(0x9E3779B97F4A7C15);
    auto z {state};
    z = (z ^ (z >> 30U)) * UINT64_C(0xBF58476D1CE4E5B9);
    z = (z ^ (z >> 27U)) * UINT64_C(0x94D049BB133111EB);

    return z ^ (z >> 31U);
}

void test_to_be_bytes_u128()
{
    check_bytes(boost::int128::to_be_bytes(pattern_u),
                {{0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10}});

    check_bytes(boost::int128::to_be_bytes(uint128{0}),
                {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}});

    check_bytes(boost::int128::to_be_bytes(uint128{42}),
                {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x2A}});

    check_bytes(boost::int128::to_be_bytes((std::numeric_limits<uint128>::max)()),
                {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}});
}

void test_to_le_bytes_u128()
{
    check_bytes(boost::int128::to_le_bytes(pattern_u),
                {{0x10, 0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01}});

    check_bytes(boost::int128::to_le_bytes(uint128{42}),
                {{0x2A, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}});

    check_bytes(boost::int128::to_le_bytes(uint128{UINT64_C(1), UINT64_C(0)}),
                {{0, 0, 0, 0, 0, 0, 0, 0, 0x01, 0, 0, 0, 0, 0, 0, 0}});
}

void test_to_be_bytes_i128()
{
    check_bytes(boost::int128::to_be_bytes(int128{-1}),
                {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}});

    check_bytes(boost::int128::to_be_bytes(int128{-42}),
                {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xD6}});

    // The minimum is the sign bit alone, and the maximum is everything but the sign bit
    check_bytes(boost::int128::to_be_bytes((std::numeric_limits<int128>::min)()),
                {{0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}});

    check_bytes(boost::int128::to_be_bytes((std::numeric_limits<int128>::max)()),
                {{0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}});
}

void test_to_le_bytes_i128()
{
    check_bytes(boost::int128::to_le_bytes(int128{-42}),
                {{0xD6, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}});

    check_bytes(boost::int128::to_le_bytes((std::numeric_limits<int128>::min)()),
                {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x80}});
}

void test_from_be_bytes()
{
    const std::array<std::uint8_t, sizeof(uint128)> bytes
        {{0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10}};

    BOOST_TEST_EQ(boost::int128::from_be_bytes<uint128>(bytes), pattern_u);
    BOOST_TEST_EQ(boost::int128::from_be_bytes<int128>(bytes), static_cast<int128>(pattern_u));

    // The same bytes read through the pointer overload
    BOOST_TEST_EQ(boost::int128::from_be_bytes<uint128>(bytes.data()), pattern_u);

    const std::array<std::uint8_t, sizeof(int128)> negative
        {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xD6}};

    BOOST_TEST_EQ(boost::int128::from_be_bytes<int128>(negative), int128{-42});
    BOOST_TEST_EQ(boost::int128::from_be_bytes<uint128>(negative), (std::numeric_limits<uint128>::max)() - 41U);

    const std::array<std::uint8_t, sizeof(int128)> zero {};
    BOOST_TEST_EQ(boost::int128::from_be_bytes<uint128>(zero), uint128{0});
    BOOST_TEST_EQ(boost::int128::from_be_bytes<int128>(zero), int128{0});
}

void test_from_le_bytes()
{
    const std::array<std::uint8_t, sizeof(uint128)> bytes
        {{0x10, 0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01}};

    BOOST_TEST_EQ(boost::int128::from_le_bytes<uint128>(bytes), pattern_u);
    BOOST_TEST_EQ(boost::int128::from_le_bytes<int128>(bytes), static_cast<int128>(pattern_u));
    BOOST_TEST_EQ(boost::int128::from_le_bytes<uint128>(bytes.data()), pattern_u);

    const std::array<std::uint8_t, sizeof(int128)> negative
        {{0xD6, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};

    BOOST_TEST_EQ(boost::int128::from_le_bytes<int128>(negative), int128{-42});
}

void test_ne_bytes()
{
    const auto ne {boost::int128::to_ne_bytes(pattern_u)};

    // Native order is by definition the object representation
    std::uint8_t raw[sizeof(uint128)] {};
    std::memcpy(raw, &pattern_u, sizeof(uint128));
    BOOST_TEST_EQ(std::memcmp(raw, ne.data(), sizeof(uint128)), 0);

    if (native_is_little())
    {
        BOOST_TEST(ne == boost::int128::to_le_bytes(pattern_u));
    }
    else
    {
        BOOST_TEST(ne == boost::int128::to_be_bytes(pattern_u));
    }

    BOOST_TEST_EQ(boost::int128::from_ne_bytes<uint128>(ne), pattern_u);
    BOOST_TEST_EQ(boost::int128::from_ne_bytes<uint128>(ne.data()), pattern_u);

    const auto signed_ne {boost::int128::to_ne_bytes(int128{-42})};
    BOOST_TEST_EQ(boost::int128::from_ne_bytes<int128>(signed_ne), int128{-42});
    BOOST_TEST_EQ(boost::int128::from_ne_bytes<int128>(signed_ne.data()), int128{-42});
}

void test_scalar_swaps()
{
    // On the matching platform the conversion is the identity, and a byteswap otherwise
    if (native_is_little())
    {
        BOOST_TEST_EQ(boost::int128::to_le(pattern_u), pattern_u);
        BOOST_TEST_EQ(boost::int128::to_be(pattern_u), boost::int128::byteswap(pattern_u));
    }
    else
    {
        BOOST_TEST_EQ(boost::int128::to_be(pattern_u), pattern_u);
        BOOST_TEST_EQ(boost::int128::to_le(pattern_u), boost::int128::byteswap(pattern_u));
    }

    // The big-endian image of a value always reads back as the big-endian byte array
    check_bytes(boost::int128::to_ne_bytes(boost::int128::to_be(pattern_u)),
                {{0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10}});

    check_bytes(boost::int128::to_ne_bytes(boost::int128::to_le(pattern_u)),
                {{0x10, 0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01}});

    std::uint64_t state {UINT64_C(0xDEADBEEFCAFEF00D)};

    for (int i {}; i < 1024; ++i)
    {
        const uint128 unsigned_value {next_word(state), next_word(state)};
        const auto signed_value {static_cast<int128>(unsigned_value)};

        BOOST_TEST_EQ(boost::int128::from_be(boost::int128::to_be(unsigned_value)), unsigned_value);
        BOOST_TEST_EQ(boost::int128::from_le(boost::int128::to_le(unsigned_value)), unsigned_value);
        BOOST_TEST_EQ(boost::int128::from_be(boost::int128::to_be(signed_value)), signed_value);
        BOOST_TEST_EQ(boost::int128::from_le(boost::int128::to_le(signed_value)), signed_value);

        // to_be and to_le differ by exactly a byteswap
        BOOST_TEST_EQ(boost::int128::to_be(unsigned_value), boost::int128::byteswap(boost::int128::to_le(unsigned_value)));
    }
}

template <typename T>
void test_array_round_trip(const T value)
{
    const auto be {boost::int128::to_be_bytes(value)};
    const auto le {boost::int128::to_le_bytes(value)};
    const auto ne {boost::int128::to_ne_bytes(value)};

    BOOST_TEST_EQ(boost::int128::from_be_bytes<T>(be), value);
    BOOST_TEST_EQ(boost::int128::from_le_bytes<T>(le), value);
    BOOST_TEST_EQ(boost::int128::from_ne_bytes<T>(ne), value);

    BOOST_TEST_EQ(boost::int128::from_be_bytes<T>(be.data()), value);
    BOOST_TEST_EQ(boost::int128::from_le_bytes<T>(le.data()), value);
    BOOST_TEST_EQ(boost::int128::from_ne_bytes<T>(ne.data()), value);

    // One order is the reverse of the other
    for (std::size_t i {}; i < be.size(); ++i)
    {
        BOOST_TEST_EQ(be[i], le[be.size() - 1U - i]);
    }
}

void test_array_round_trips()
{
    test_array_round_trip(uint128{0});
    test_array_round_trip(uint128{1});
    test_array_round_trip(pattern_u);
    test_array_round_trip((std::numeric_limits<uint128>::max)());

    test_array_round_trip(int128{0});
    test_array_round_trip(int128{-1});
    test_array_round_trip(int128{-42});
    test_array_round_trip((std::numeric_limits<int128>::min)());
    test_array_round_trip((std::numeric_limits<int128>::max)());

    std::uint64_t state {UINT64_C(0x0123456789ABCDEF)};

    for (int i {}; i < 1024; ++i)
    {
        const uint128 unsigned_value {next_word(state), next_word(state)};
        test_array_round_trip(unsigned_value);
        test_array_round_trip(static_cast<int128>(unsigned_value));
    }
}

void test_element_types()
{
    const auto as_char {boost::int128::to_be_bytes<char>(pattern_u)};
    BOOST_TEST_EQ(boost::int128::from_be_bytes<uint128>(as_char), pattern_u);
    BOOST_TEST_EQ(boost::int128::from_be_bytes<uint128>(as_char.data()), pattern_u);

    const auto as_signed_char {boost::int128::to_le_bytes<signed char>(int128{-42})};
    BOOST_TEST_EQ(boost::int128::from_le_bytes<int128>(as_signed_char), int128{-42});

    const auto as_unsigned_char {boost::int128::to_ne_bytes<unsigned char>(pattern_u)};
    BOOST_TEST_EQ(boost::int128::from_ne_bytes<uint128>(as_unsigned_char), pattern_u);

    #if defined(__cpp_lib_byte) && __cpp_lib_byte >= 201603L

    const auto as_byte {boost::int128::to_be_bytes<std::byte>(pattern_u)};
    BOOST_TEST(as_byte[0] == std::byte{0x01});
    BOOST_TEST(as_byte[15] == std::byte{0x10});
    BOOST_TEST_EQ(boost::int128::from_be_bytes<uint128>(as_byte), pattern_u);
    BOOST_TEST_EQ(boost::int128::from_be_bytes<uint128>(as_byte.data()), pattern_u);

    #endif
}

void test_constexpr()
{
    constexpr uint128 value {UINT64_C(0x0102030405060708), UINT64_C(0x090A0B0C0D0E0F10)};

    static_assert(boost::int128::from_be(boost::int128::to_be(value)) == value, "Scalar round trip");
    static_assert(boost::int128::from_le(boost::int128::to_le(value)) == value, "Scalar round trip");

    constexpr auto be {boost::int128::to_be_bytes(value)};
    static_assert(be[0] == 0x01 && be[15] == 0x10, "Big-endian order");
    static_assert(boost::int128::from_be_bytes<uint128>(be) == value, "Array round trip");

    constexpr auto le {boost::int128::to_le_bytes(value)};
    static_assert(le[0] == 0x10 && le[15] == 0x01, "Little-endian order");
    static_assert(boost::int128::from_le_bytes<uint128>(le) == value, "Array round trip");

    constexpr auto ne {boost::int128::to_ne_bytes(value)};
    static_assert(boost::int128::from_ne_bytes<uint128>(ne) == value, "Array round trip");

    constexpr int128 negative {-42};
    static_assert(boost::int128::from_be_bytes<int128>(boost::int128::to_be_bytes(negative)) == negative, "Signed array round trip");
    static_assert(boost::int128::from_le_bytes<int128>(boost::int128::to_le_bytes(negative)) == negative, "Signed array round trip");
}

int main()
{
    test_to_be_bytes_u128();
    test_to_le_bytes_u128();
    test_to_be_bytes_i128();
    test_to_le_bytes_i128();

    test_from_be_bytes();
    test_from_le_bytes();

    test_ne_bytes();
    test_scalar_swaps();
    test_array_round_trips();
    test_element_types();
    test_constexpr();

    return boost::report_errors();
}
