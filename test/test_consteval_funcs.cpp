// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#define BOOST_INT128_ALLOW_SIGN_CONVERSION
#include <boost/int128.hpp>

#if defined(__cpp_consteval) && __cpp_consteval >= 201811L
#  define BOOST_INT128_CONSTEVAL consteval
#else
#  define BOOST_INT128_CONSTEVAL constexpr
#endif

#if defined(__GNUC__) && __GNUC__ == 7 && defined(__i386__)

/*
32-bit GCC 7 fails with the following:

libs/decimal/test/test_consteval_funcs.cpp:39:9: error: non-constant condition for static assertion
static_assert(x * 2U == 4U, "");
^~~~~~~~~~~~~
In file included from ./boost/int128/detail/int128_imp.hpp:13:0,
from ./boost/int128/int128.hpp:9,
from ./boost/int128.hpp:8,
from libs/decimal/test/test_consteval_funcs.cpp:6:
libs/decimal/test/test_consteval_funcs.cpp:39:25:   in constexpr expansion of 'boost::int128::operator*<unsigned int>(x, 2)'
./boost/int128/detail/uint128_imp.hpp:2276:31:   in constexpr expansion of 'boost::int128::detail::default_mul<long long unsigned int>(lhs, ((uint64_t)rhs))'
./boost/int128/detail/uint128_imp.hpp:2211:37:   in constexpr expansion of 'boost::int128::detail::knuth_multiply<boost::int128::uint128_t, 4, 2>(((const uint32_t (&)[4])(& lhs_words)), ((const uint32_t (&)[2])(& rhs_words)))'
./boost/int128/detail/common_mul.hpp:38:9: error: constexpr loop iteration count exceeds limit of 262144 (use -fconstexpr-loop-limit= to increase the limit)
for (std::size_t i {}; i < u_size; ++i)
^~~

u_size can only possibly be 1, 2, or 4 so this is clearly incorrect on the compilers side
*/

int main()
{
    return 0;
}

#else

template <typename T>
BOOST_INT128_CONSTEVAL bool test()
{
    {
        constexpr T x {2U};
        constexpr T y {3U};

        // Comparisons
        static_assert(x < y, "");
        static_assert(x <= y, "");
        static_assert(y > x, "");
        static_assert(y >= x, "");
        static_assert(y == 3U, "");
        static_assert(x != 3U, "");
        static_assert(x != y, "");

        // Basic arithmetic
        static_assert(x + y == 5U, "");
        static_assert(y - x == 1U, "");
        static_assert(y * x == 6U, "");
        static_assert(y / x == 1U, "");
        static_assert(y % x == 1U, "");

        static_assert(x + 2U == 4U, "");
        static_assert(x - 2U == 0U, "");
        static_assert(x * 2U == 4U, "");
        static_assert(x / 2U == 1U, "");
        static_assert(x % 2U == 0U, "");
    }

    {
        // Bigger numbers
        constexpr T x {1U, 2U};
        constexpr T y {0U, UINT64_MAX - 2U};

        // Comparisons
        static_assert(x > y, "");
        static_assert(x >= y, "");
        static_assert(y < x, "");
        static_assert(y <= x, "");
        static_assert(x == x, "");
        static_assert(x != y, "");

        // Basic arithmetic
        constexpr T add_res {1U, UINT64_MAX};
        constexpr T sub_res {0U, 5U};
        constexpr T div_res {0U, 1U};
        static_assert(x + y == add_res, "");
        static_assert(x - y == sub_res, "");
        static_assert(x / y == div_res, "");
    }

    return true;
}

int main()
{
    static_assert(test<boost::int128::uint128_t>(), "");
    static_assert(test<boost::int128::int128_t>(), "");

    return 0;
}

#endif // GCC 7 32-bit workaround
