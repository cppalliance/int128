// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128.hpp>
#include <limits>
#include <cassert>
#include <type_traits>
#include <sstream>

#ifdef __clang__
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wunused-variable"
#endif

int main()
{
    // There are a number of ways to construct integer types

    // 1) From a builtin-integer type
    constexpr boost::int128::uint128_t from_builtin {6U};
    assert(from_builtin == 6U);

    // 2) By directly adding 128-bit hex/integer values
    constexpr boost::int128::uint128_t from_hex {UINT64_C(0xFFFFFFFFFFFFFFFF), UINT64_C(0xFFFFFFFFFFFFFFFF)};
    assert(from_hex == std::numeric_limits<boost::int128::uint128_t>::max());

    // 3) From literal which allows values >2^64 to be easily constructed
    // To match expectations the literals are in a separate literals namespace
    using namespace boost::int128::literals;
    const auto from_literal = "36893488147419103232"_U128; // Can be constexpr, but older compilers (GCC <= 7) fail
    static_assert(std::is_same<decltype(from_literal), const boost::int128::uint128_t>::value, "uint128_t works fine with auto construction");

    // 4) From macro appending the correct literal like UINT32_C, UINT64_C etc.
    // This also allows values >2^64 to be constructed without needing to separate into high and low hex values
    const auto from_macro {BOOST_INT128_UINT128_C(36893488147419103232)}; // Can be constexpr, but older compilers (GCC <= 7 and Clang <= 8) fail
    assert(from_macro == from_literal);

    // 5) Istream is supported by the types
    std::stringstream in;
    in.str("340282366920938463463374607431768211455");
    boost::int128::uint128_t from_stream;
    in >> from_stream;
    assert(from_stream == from_hex);

    // These same methods also apply to int128_t
    constexpr boost::int128::int128_t from_int {-42};
    assert(from_int == -42);

    constexpr boost::int128::int128_t from_hex_signed {INT64_MIN, 0};
    assert(from_hex_signed == std::numeric_limits<boost::int128::int128_t>::min());

    // Both capital and lowercase letters work
    const auto negative_literal {"-42"_i128};
    assert(negative_literal == from_int);

    const auto negative_macro {BOOST_INT128_INT128_C(-170141183460469231731687303715884105728)};
    assert(negative_macro == from_hex_signed);

    return 0;
}
