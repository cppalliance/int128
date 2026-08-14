// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
//
// A user-defined literal whose characters are not valid digits for the base must be
// rejected at compile time instead of being silently truncated. "12xyz" parses the
// leading 12 and then hits the invalid 'x', so the literal is malformed.

#include <boost/int128/literals.hpp>

using namespace boost::int128::literals;

int main()
{
    constexpr auto value = "12xyz"_u128;
    static_cast<void>(value);

    return 0;
}
