// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
//
// A user-defined literal whose value does not fit the target type must be rejected
// at compile time instead of being silently accepted.

#include <boost/int128/literals.hpp>

using namespace boost::int128::literals;

int main()
{
    constexpr auto value = 340282366920938463463374607431768211456_u128;
    static_cast<void>(value);

    return 0;
}
