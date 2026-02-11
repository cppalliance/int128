// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128/int128.hpp>

#ifdef BOOST_INT128_HAS_SPACESHIP_OPERATOR

#include <boost/core/lightweight_test.hpp>

void test_u128()
{
    constexpr boost::int128::uint128_t x {5U};
    BOOST_TEST((x <=> 4U) == std::strong_ordering::greater);
    BOOST_TEST((x <=> 5U) == std::strong_ordering::equivalent);
    BOOST_TEST((x <=> 5U) == std::strong_ordering::equal);
    BOOST_TEST((x <=> 6U) == std::strong_ordering::less);
}

void test_int128()
{
    constexpr boost::int128::int128_t x {5};
    BOOST_TEST((x <=> 4) == std::strong_ordering::greater);
    BOOST_TEST((x <=> 5) == std::strong_ordering::equivalent);
    BOOST_TEST((x <=> 5) == std::strong_ordering::equal);
    BOOST_TEST((x <=> 6) == std::strong_ordering::less);
}

int main()
{
    test_u128();
    test_int128();

    return boost::report_errors();
}

#else

int main()
{
    return 0;
}

#endif
