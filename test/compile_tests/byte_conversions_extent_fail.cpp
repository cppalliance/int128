// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// A byte array whose size does not match the width of the target type is
// rejected at compile time. This must fail to compile.

#include <boost/int128/byte_conversions.hpp>
#include <array>
#include <cstdint>

int main()
{
    const std::array<std::uint8_t, 8> bytes {};

    return static_cast<int>(boost::int128::from_be_bytes<boost::int128::uint128>(bytes));
}
