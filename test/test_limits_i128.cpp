// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128/int128.hpp>
#include <boost/core/lightweight_test.hpp>
#include <limits>

void test_basics()
{
    static_assert(std::numeric_limits<boost::int128::int128_t>::is_specialized, "Should be true");
    static_assert(std::numeric_limits<boost::int128::int128_t>::is_signed, "Should be true");
    static_assert(std::numeric_limits<boost::int128::int128_t>::is_integer, "Should be true");
    static_assert(std::numeric_limits<boost::int128::int128_t>::is_exact, "Should be true");
    static_assert(!std::numeric_limits<boost::int128::int128_t>::has_infinity, "Should be false");
    static_assert(!std::numeric_limits<boost::int128::int128_t>::has_quiet_NaN, "Should be false");
    static_assert(!std::numeric_limits<boost::int128::int128_t>::has_signaling_NaN, "Should be false");

    // C++23 deprecated the following two members
    #if defined(__GNUC__) && __cplusplus > 202002L
    #  pragma GCC diagnostic push
    #  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    #elif defined(_MSC_VER)
    #  pragma warning(push)
    #  pragma warning(disable:4996)
    #endif

    static_assert(std::numeric_limits<boost::int128::int128_t>::has_denorm == std::denorm_absent, "No denorm");
    static_assert(!std::numeric_limits<boost::int128::int128_t>::has_denorm_loss, "No denorm");

    #if defined(__GNUC__) && __cplusplus > 202002L
    #  pragma GCC diagnostic pop
    #elif defined(_MSC_VER)
    #  pragma warning(pop)
    #endif

    static_assert(std::numeric_limits<boost::int128::int128_t>::round_style == std::round_toward_zero, "Integer rounding");
    static_assert(!std::numeric_limits<boost::int128::int128_t>::is_iec559, "Should be false");
    static_assert(std::numeric_limits<boost::int128::int128_t>::is_bounded, "Should be true");
    static_assert(std::numeric_limits<boost::int128::int128_t>::is_modulo, "Should be true");
    static_assert(std::numeric_limits<boost::int128::int128_t>::digits == CHAR_BIT * sizeof(boost::int128::int128_t) - 1, "127 bits");

    // std::numeric_limits<boost::int128::int128_t>::digits * std::log10(2) = 38.23
    static_assert(std::numeric_limits<boost::int128::int128_t>::digits10 == 38, "38 base 10 digits");
    static_assert(std::numeric_limits<boost::int128::int128_t>::max_digits10 == 0, "0 since not fp");
    static_assert(std::numeric_limits<boost::int128::int128_t>::radix == 2, "Should be 2");
    static_assert(std::numeric_limits<boost::int128::int128_t>::min_exponent == 0, "Should be 0");
    static_assert(std::numeric_limits<boost::int128::int128_t>::min_exponent10 == 0, "Should be 0");
    static_assert(std::numeric_limits<boost::int128::int128_t>::max_exponent == 0, "Should be 0");
    static_assert(std::numeric_limits<boost::int128::int128_t>::max_exponent10 == 0, "Should be 0");
    static_assert(std::numeric_limits<boost::int128::int128_t>::traps == std::numeric_limits<std::uint64_t>::traps, "Uses what std::uint64_t does");
    static_assert(!std::numeric_limits<boost::int128::int128_t>::tinyness_before, "should be false");
}

#ifdef BOOST_INT128_HAS_INT128

void test_member_functions()
{
    using builtin_i128 = boost::int128::detail::builtin_i128;
    constexpr auto builtin_max {std::numeric_limits<builtin_i128>::max()};

    // Numeric limits are only defined in GNU mode for old compilers or GCC 13+ automatically
    BOOST_INT128_IF_CONSTEXPR (builtin_max != 0)
    {
        using lib_i128 = boost::int128::int128_t;

        BOOST_TEST(std::numeric_limits<builtin_i128>::min() == std::numeric_limits<lib_i128>::min());
        BOOST_TEST(std::numeric_limits<builtin_i128>::lowest() == std::numeric_limits<lib_i128>::lowest());
        BOOST_TEST(std::numeric_limits<builtin_i128>::max() == std::numeric_limits<lib_i128>::max());
        BOOST_TEST(std::numeric_limits<builtin_i128>::epsilon() == std::numeric_limits<lib_i128>::epsilon());
        BOOST_TEST(std::numeric_limits<builtin_i128>::round_error() == std::numeric_limits<lib_i128>::round_error());
        BOOST_TEST(std::numeric_limits<builtin_i128>::infinity() == std::numeric_limits<lib_i128>::infinity());
        BOOST_TEST(std::numeric_limits<builtin_i128>::quiet_NaN() == std::numeric_limits<lib_i128>::quiet_NaN());
        BOOST_TEST(std::numeric_limits<builtin_i128>::signaling_NaN() == std::numeric_limits<lib_i128>::signaling_NaN());
        BOOST_TEST(std::numeric_limits<builtin_i128>::denorm_min() == std::numeric_limits<lib_i128>::denorm_min());
    }
}

#else

void test_member_functions() {}

#endif // BOOST_INT128_HAS_INT128

int main()
{
    test_basics();
    test_member_functions();

    return boost::report_errors();
}
