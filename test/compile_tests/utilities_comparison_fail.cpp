// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// The C++26 integer comparison functions reject plain char (and the other
// character types, bool, and std::byte). This must fail to compile.

#include <boost/int128/utilities.hpp>

int main()
{
    return boost::int128::cmp_equal(boost::int128::uint128{1}, 'a') ? 1 : 0;
}
