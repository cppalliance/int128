// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
//
// A prefixed user-defined literal whose value does not fit the target type must be
// rejected at compile time. 0x1 followed by 32 zeros is 2^128, one past the
// uint128_t maximum, so the base-16 parse overflows and this must not compile.

#include <boost/int128/literals.hpp>

using namespace boost::int128::literals;

int main()
{
    constexpr auto value = 0x100000000000000000000000000000000_u128;
    static_cast<void>(value);

    return 0;
}
