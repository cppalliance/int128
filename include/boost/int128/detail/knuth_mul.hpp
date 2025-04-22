// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_DETAIL_KNUTH_MUL_HPP
#define BOOST_INT128_DETAIL_KNUTH_MUL_HPP

// See: The Art of Computer Programming Volume 2 (Semi-numerical algorithms) section 4.3.1
// Algorithm M: Multiplication of Non-negative integers

#include <boost/int128/detail/config.hpp>
#include <cstdint>

namespace boost {
namespace int128 {
namespace detail {

template <typename ReturnType, std::size_t u_size, std::size_t v_size>
BOOST_INT128_FORCE_INLINE constexpr ReturnType knuth_multiply(const std::uint32_t (&u)[u_size],
                                                              const std::uint32_t (&v)[v_size]) noexcept
{
    std::uint32_t w[u_size + v_size] {};

    // M.1
    for (std::size_t j {}; j < v_size; ++j)
    {
        // M.2
        if (v[j] == 0)
        {
            w[j + u_size] = 0;
            continue;
        }

        // M.3
        std::uint64_t t {};
        for (std::size_t i {}; i < u_size; ++i)
        {
            // M.4
            t += static_cast<std::uint64_t>(u[i]) * v[j] + w[i + j];
            w[i + j] = static_cast<std::uint32_t>(t);
            t >>= 32u;
        }

        // M.5
        w[j + u_size] = static_cast<std::uint32_t>(t);
    }

    const auto low {static_cast<std::uint64_t>(w[0]) | (static_cast<std::uint64_t>(w[1]) << 32)};
    const auto high {static_cast<std::uint64_t>(w[2]) | (static_cast<std::uint64_t>(w[3]) << 32)};

    return {high, low};
}

} // namespace detail
} // namespace int128
} // namespace boost

#endif // BOOST_INT128_DETAIL_KNUTH_MUL_HPP
