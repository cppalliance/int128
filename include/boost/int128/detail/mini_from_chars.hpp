// Copyright 2022 Peter Dimov
// Copyright 2023 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef MINI_FROM_CHARS_HPP
#define MINI_FROM_CHARS_HPP

#include <boost/int128/detail/uint128_imp.hpp>
#include <boost/int128/detail/int128_imp.hpp>

#ifndef BOOST_INT128_BUILD_MODULE

#include <cerrno>
#include <limits>
#include <cstddef>

#endif

namespace boost {
namespace int128 {
namespace detail {

namespace impl {

#if !defined(BOOST_INT128_HAS_GPU_SUPPORT)

BOOST_INT128_INLINE_CONSTEXPR unsigned char uchar_values[] =
     {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 254, 255, 255, 255, 255, 255, 255, 255, 255,
        0,   1,   2,   3,   4,   5,   6,   7,   8,   9, 255, 255, 255, 255, 255, 255,
      255,  10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
       25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35, 255, 255, 255, 255, 255,
      255,  10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
       25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};

static_assert(sizeof(uchar_values) == 256, "uchar_values should represent all 256 values of unsigned char");

#endif // __NVCC__

// Convert characters for 0-9, A-Z, a-z to 0-35. The digit separator ' is 254. Anything else is 255
BOOST_INT128_HOST_DEVICE BOOST_INT128_FORCE_INLINE constexpr auto digit_from_char(char val) noexcept -> unsigned char
{
    #if defined(BOOST_INT128_HAS_GPU_SUPPORT)

    constexpr unsigned char uchar_values[] =
    {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 254, 255, 255, 255, 255, 255, 255, 255, 255,
       0,   1,   2,   3,   4,   5,   6,   7,   8,   9, 255, 255, 255, 255, 255, 255,
     255,  10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
      25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35, 255, 255, 255, 255, 255,
     255,  10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
      25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};

    static_assert(sizeof(uchar_values) == 256, "uchar_values should represent all 256 values of unsigned char");

    #endif // __NVCC__

    return uchar_values[static_cast<unsigned char>(val)];
}

template <typename Integer, typename Unsigned_Integer, bool is_literal_parse = false>
BOOST_INT128_HOST_DEVICE constexpr int from_chars_integer_impl(const char* first, const char* last, Integer& value, int base) noexcept
{
    if (last - first <= 0)
    {
        return EINVAL;
    }

    Unsigned_Integer result {};
    Unsigned_Integer overflow_value {};
    Unsigned_Integer max_digit {};

    const auto unsigned_base = static_cast<Unsigned_Integer>(base);

    // Strip sign if the type is signed
    // Negative sign will be appended at the end of parsing
    bool is_negative = false;
    static_cast<void>(is_negative);
    auto next = first;

    BOOST_INT128_IF_CONSTEXPR (std::numeric_limits<Integer>::is_signed)
    {
        if (*next == '-')
        {
            is_negative = true;
            ++next;
        }

        overflow_value = static_cast<Unsigned_Integer>((std::numeric_limits<Integer>::max)());
        max_digit = static_cast<Unsigned_Integer>((std::numeric_limits<Integer>::max)());

        if (is_negative)
        {
            ++overflow_value;
            ++max_digit;
        }
    }
    else
    {
        if (*next == '-' || *next == '+')
        {
            return EINVAL;
        }

        overflow_value = (std::numeric_limits<Unsigned_Integer>::max)();
        max_digit = (std::numeric_limits<Unsigned_Integer>::max)();
    }


    overflow_value /= unsigned_base;
    max_digit %= unsigned_base;

    // If the only character was a sign abort now
    if (next == last)
    {
        return EINVAL;
    }

    bool overflowed = false;

    const std::ptrdiff_t nc = last - next;

    // For bases 2..10 the first digits10 characters always fit in the unsigned
    // For bases above 10, the safe window is shorter, so we must check with each iteration
    const std::ptrdiff_t nd {
        base <= 10
            ? static_cast<std::ptrdiff_t>(std::numeric_limits<Integer>::digits10)
            : std::ptrdiff_t{0}
    };

    const std::ptrdiff_t fast_limit {nd < nc ? nd : nc};
    std::ptrdiff_t i = 0;

    for (; i < fast_limit; ++i)
    {
        const auto raw_digit = digit_from_char(*next);

        // When parsing a user-defined literal skip the digit separator ' (marked as 254)
        BOOST_INT128_IF_CONSTEXPR (is_literal_parse)
        {
            if (raw_digit == 254)
            {
                ++next;
                continue;
            }
        }

        const auto current_digit = static_cast<Unsigned_Integer>(raw_digit);

        if (current_digit >= unsigned_base)
        {
            break;
        }

        result = static_cast<Unsigned_Integer>(result * unsigned_base + current_digit);
        ++next;
    }

    for (; i < nc; ++i)
    {
        const auto raw_digit = digit_from_char(*next);

        // When parsing a user-defined literal skip the digit separator ' (marked as 254)
        BOOST_INT128_IF_CONSTEXPR (is_literal_parse)
        {
            if (raw_digit == 254)
            {
                ++next;
                continue;
            }
        }

        const auto current_digit = static_cast<Unsigned_Integer>(raw_digit);

        if (current_digit >= unsigned_base)
        {
            break;
        }

        if (result < overflow_value || (result == overflow_value && current_digit <= max_digit))
        {
            result = static_cast<Unsigned_Integer>(result * unsigned_base + current_digit);
        }
        else
        {
            overflowed = true;
            break;
        }

        ++next;
    }

    // Return the parsed value, adding the sign back if applicable
    // If we have overflowed, then we do not return the result
    if (overflowed)
    {
        return EDOM;
    }

    value = static_cast<Integer>(result);

    BOOST_INT128_IF_CONSTEXPR (std::numeric_limits<Integer>::is_signed)
    {
        if (is_negative)
        {
            value = static_cast<Integer>(-(static_cast<Unsigned_Integer>(value)));
        }
    }

    // This value will be negative to differentiate from errno values
    // since they are in the range of acceptable distances

    // This cast is useless on 32-bit platforms
    #if defined(__GNUC__) && !defined(__clang__)
    #  pragma GCC diagnostic push
    #  pragma GCC diagnostic ignored "-Wuseless-cast"
    #endif

    return static_cast<int>(first - next);

    #if defined(__GNUC__) && !defined(__clang__)
    #  pragma GCC diagnostic pop
    #endif
}
} // namespace impl

BOOST_INT128_TEST_EXPORT BOOST_INT128_HOST_DEVICE constexpr int from_chars(const char* first, const char* last, uint128_t& value, int base = 10) noexcept
{
    return impl::from_chars_integer_impl<uint128_t, uint128_t>(first, last, value, base);
}

BOOST_INT128_TEST_EXPORT BOOST_INT128_HOST_DEVICE constexpr int from_chars(const char* first, const char* last, int128_t& value, int base = 10) noexcept
{
    return impl::from_chars_integer_impl<int128_t, uint128_t>(first, last, value, base);
}

// Parsing entry points for the user-defined literals. Unlike from_chars these skip the
// C++ digit separator ' so that literals such as 1'234'567_u128 are accepted.
BOOST_INT128_TEST_EXPORT BOOST_INT128_HOST_DEVICE constexpr int from_chars_literal(const char* first, const char* last, uint128_t& value, int base = 10) noexcept
{
    return impl::from_chars_integer_impl<uint128_t, uint128_t, true>(first, last, value, base);
}

BOOST_INT128_TEST_EXPORT BOOST_INT128_HOST_DEVICE constexpr int from_chars_literal(const char* first, const char* last, int128_t& value, int base = 10) noexcept
{
    return impl::from_chars_integer_impl<int128_t, uint128_t, true>(first, last, value, base);
}

// Parse a user-defined literal, hard-failing on any malformed or out-of-range input.
// A C++ base prefix (0x/0X hex, 0b/0B binary, or a leading 0 for octal) is stripped and
// the digits parsed in that base, otherwise handled as base 10
template <typename Integer>
BOOST_INT128_HOST_DEVICE constexpr Integer parse_literal(const char* first, const char* last) noexcept
{
    Integer value {};

    // A leading sign stays with the digits; a base prefix, if present, follows it.
    auto next = first;
    const bool negative {next != last && *next == '-'};
    if (negative)
    {
        ++next;
    }

    int base {10};
    bool prefixed {false};

    if (last - next >= 2 && *next == '0')
    {
        const char marker {next[1]};
        if (marker == 'x' || marker == 'X')
        {
            base = 16;
            next += 2;
            prefixed = true;
        }
        else if (marker == 'b' || marker == 'B')
        {
            base = 2;
            next += 2;
            prefixed = true;
        }
        else
        {
            base = 8;
            next += 1;
            prefixed = true;
        }
    }

    // With no prefix, from_chars_literal handles the sign and the full decimal range
    if (!prefixed)
    {
        if (from_chars_literal(first, last, value) != first - last)
        {
            BOOST_INT128_UNREACHABLE;
        }

        return value;
    }

    // Prefixed: parse the magnitude in the detected base, then reapply the sign.
    if (from_chars_literal(next, last, value, base) != next - last)
    {
        BOOST_INT128_UNREACHABLE;
    }

    if (negative)
    {
        BOOST_INT128_IF_CONSTEXPR (std::numeric_limits<Integer>::is_signed)
        {
            value = static_cast<Integer>(-value);
        }
        else
        {
            BOOST_INT128_UNREACHABLE;
        }
    }

    return value;
}

} // namespace detail
} // namespace int128
} // namespace boost

#endif //MINI_FROM_CHARS_HPP
