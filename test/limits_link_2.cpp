// Copyright 2023 Peter Dimov
// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128.hpp>
#include <limits>

void test_odr_use( int const* );
void test_odr_use( std::size_t const* );

template<typename T> void test()                        // LCOV_EXCL_LINE
{
    test_odr_use( &std::numeric_limits<T>::digits10 );  // LCOV_EXCL_LINE
}

void f2()
{
    test<boost::int128::int128_t>();
    test<boost::int128::uint128_t>();
}
