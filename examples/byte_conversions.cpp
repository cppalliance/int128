// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128/int128.hpp>
#include <boost/int128/byte_conversions.hpp>
#include <boost/int128/iostream.hpp>
#include <array>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>

// Prints the bytes of an array in the order they are stored
template <typename Bytes>
void print_bytes(const char* label, const Bytes& bytes)
{
    std::cout << label;

    for (const auto byte : bytes)
    {
        std::cout << ' ' << std::hex << std::setfill('0') << std::setw(2) << static_cast<unsigned>(byte);
    }

    std::cout << std::dec << std::endl;
}

int main()
{
    using boost::int128::uint128;
    using boost::int128::int128;

    // The 16 bytes 01 02 ... 10 read as a big-endian value
    constexpr uint128 value {UINT64_C(0x0102030405060708), UINT64_C(0x090A0B0C0D0E0F10)};

    std::cout << "=== Byte arrays ===" << std::endl;

    // The byte order of the array is the requested one on every platform
    print_bytes("to_be_bytes:", boost::int128::to_be_bytes(value));
    print_bytes("to_le_bytes:", boost::int128::to_le_bytes(value));

    // Native order is whichever of the two matches the host, so this is the
    // one form whose output depends on the platform
    print_bytes("to_ne_bytes:", boost::int128::to_ne_bytes(value));

    std::cout << "\n=== Reading a value back out of bytes ===" << std::endl;

    constexpr std::array<std::uint8_t, sizeof(uint128)> wire
    {{
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x2C
    }};

    // The target type is given explicitly, and the byte count has to match
    std::cout << "from_be_bytes: " << boost::int128::from_be_bytes<uint128>(wire) << std::endl;
    std::cout << "from_le_bytes: " << boost::int128::from_le_bytes<uint128>(wire) << std::endl;

    // Everything is constexpr, so a whole round-trip can be checked at compile time
    static_assert(boost::int128::from_be_bytes<uint128>(boost::int128::to_be_bytes(value)) == value,
                  "Round trip through big-endian bytes");

    std::cout << "\n=== Signed values ===" << std::endl;

    // The two's complement bit pattern is what gets reversed, so negative
    // values need no special handling
    constexpr int128 negative {-300};

    print_bytes("to_be_bytes(-300):", boost::int128::to_be_bytes(negative));
    std::cout << "from_be_bytes:     " << boost::int128::from_be_bytes<int128>(boost::int128::to_be_bytes(negative)) << std::endl;

    std::cout << "\n=== Whole value conversions ===" << std::endl;

    // to_be and to_le produce a value whose object representation is in the
    // requested order, which is what a memcpy into a packet buffer wants.
    // The value itself is only meaningful again after the matching from_be / from_le.
    const auto big_endian_image {boost::int128::to_be(value)};

    print_bytes("object representation of to_be(value):", boost::int128::to_ne_bytes(big_endian_image));
    std::cout << "from_be recovers: " << boost::int128::from_be(big_endian_image) << std::endl;
    std::cout << "value:            " << value << std::endl;

    // Any byte-like element type can be requested, which is convenient when the
    // surrounding buffer is not made of std::uint8_t
    const auto as_char {boost::int128::to_le_bytes<char>(value)};
    std::cout << "\nfrom_le_bytes over a char buffer: " << boost::int128::from_le_bytes<uint128>(as_char.data()) << std::endl;

    return 0;
}
