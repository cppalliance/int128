// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// The target of from_be_bytes must be one of the library types, so a built-in
// integer is rejected. This must fail to compile.

#include <boost/int128/byte_conversions.hpp>
#include <array>
#include <cstdint>

int main()
{
    const std::array<std::uint8_t, 16> bytes {};

    return static_cast<int>(boost::int128::from_be_bytes<std::uint64_t>(bytes));
}
