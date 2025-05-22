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
