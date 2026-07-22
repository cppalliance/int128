// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_BUILD_MODULE

#include <boost/int128/int128.hpp>
#include <boost/int128/literals.hpp>
#include <boost/int128/iostream.hpp>

#else

import boost.int128;
// The convenience macros are not part of the module interface (macros never are).
#include <boost/int128/detail/literal_macros.hpp>

#endif

#include <boost/core/lightweight_test.hpp>
#include <limits>

using namespace boost::int128::literals;

void test_u128_literals()
{
    BOOST_TEST(boost::int128::uint128_t{0} == 0_u128);
    BOOST_TEST(boost::int128::uint128_t{10} == "10"_U128);
    BOOST_TEST(boost::int128::uint128_t{0} == BOOST_INT128_UINT128_C(0));

    const boost::int128::uint128_t max_val {std::numeric_limits<boost::int128::uint128_t>::max()};
    const auto macro_val {BOOST_INT128_UINT128_C(340282366920938463463374607431768211455)};
    BOOST_TEST(max_val == macro_val);
}

void test_u128_digit_separators()
{
    // C++ digit separators (') are ignored inside numeric literals
    BOOST_TEST(boost::int128::uint128_t{1234567} == 1'234'567_u128);
    BOOST_TEST(boost::int128::uint128_t{1000} == 1'000_U128);
    BOOST_TEST(boost::int128::uint128_t{1234} == 12'34_u128);

    // Separators are also honored through the convenience macro
    BOOST_TEST(boost::int128::uint128_t{1234} == BOOST_INT128_UINT128_C(1'234));

    // Full-width value with a separator between every group of three digits
    const boost::int128::uint128_t max_val {std::numeric_limits<boost::int128::uint128_t>::max()};
    BOOST_TEST(max_val == 340'282'366'920'938'463'463'374'607'431'768'211'455_u128);

    // Separators must be usable in a constant expression
    static_assert(100'000_u128 == boost::int128::uint128_t{100000}, "constexpr separator");
}

void test_i128_literals()
{
    BOOST_TEST(boost::int128::int128_t{0} == 0_i128);
    BOOST_TEST(boost::int128::int128_t{10} == "10"_I128);
    BOOST_TEST(boost::int128::int128_t{0} == BOOST_INT128_INT128_C(0));

    const boost::int128::int128_t max_val {std::numeric_limits<boost::int128::int128_t>::max()};
    const auto macro_val {BOOST_INT128_INT128_C(170141183460469231731687303715884105727)};
    BOOST_TEST(max_val == macro_val);

    const boost::int128::int128_t min_val {std::numeric_limits<boost::int128::int128_t>::min()};
    const auto min_macro_val {BOOST_INT128_INT128_C(-170141183460469231731687303715884105728)};
    BOOST_TEST(min_val == min_macro_val);
    
    BOOST_TEST("-42"_i128 == -42);
}

void test_i128_digit_separators()
{
    // C++ digit separators (') are ignored inside numeric literals
    BOOST_TEST(boost::int128::int128_t{1000} == 1'000_i128);
    BOOST_TEST(boost::int128::int128_t{9999} == 9'999_I128);

    // A leading unary minus is applied after the (separated) literal is parsed
    BOOST_TEST(boost::int128::int128_t{-1000000} == -1'000'000_i128);

    // Separators are also honored through the convenience macro
    BOOST_TEST(boost::int128::int128_t{9999} == BOOST_INT128_INT128_C(9'999));

    // Full-width value with a separator between every group of three digits
    const boost::int128::int128_t max_val {std::numeric_limits<boost::int128::int128_t>::max()};
    BOOST_TEST(max_val == 170'141'183'460'469'231'731'687'303'715'884'105'727_i128);

    // Separators must be usable in a constant expression
    static_assert(100'000_i128 == boost::int128::int128_t{100000}, "constexpr separator");
}

int main()
{
    test_u128_literals();
    test_u128_digit_separators();
    test_i128_literals();
    test_i128_digit_separators();

    return boost::report_errors();
}
