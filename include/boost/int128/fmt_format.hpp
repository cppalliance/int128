// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_FMT_FORMAT_HPP
#define BOOST_INT128_FMT_FORMAT_HPP

#if __has_include(<fmt/base.h>) && __has_include(<fmt/format.h>)

#include <boost/int128/detail/config.hpp>
#include <boost/int128/detail/mini_to_chars.hpp>
#include <boost/int128/int128.hpp>
#include <fmt/base.h>
#include <fmt/format.h>
#include <algorithm>
#include <tuple>

#define BOOST_INT128_HAS_FMT_FORMAT

namespace boost {
namespace int128 {
namespace fmt_detail {

enum class sign_option
{
    plus,
    negative,
    space
};

template <typename ParseContext>
constexpr auto parse_impl(ParseContext& ctx)
{
    auto it {ctx.begin()};
    int base = 10;
    bool is_upper = false;
    int padding_digits = 0;
    auto sign = sign_option::negative;
    bool prefix = false;
    bool write_as_character = false;
    bool character_debug_format = false;

    // Handle sign or space
    if (it != ctx.end())
    {
        switch (*it) {
            case ' ':
                sign = sign_option::space;
                ++it;
                break;
            case '+':
                sign = sign_option::plus;
                ++it;
                break;
            case '-':
                sign = sign_option::negative;
                ++it;
                break;
            default:
                break;
        }
    }

    // Alternate form option
    if (it != ctx.end() && *it == '#')
    {
        prefix = true;
        ++it;
    }

    // Character presentation type
    if (it != ctx.end() && (*it == '?' || *it == 'c'))
    {
        character_debug_format = *it == '?';
        ++it;
    }

    // Check for a padding character
    while (it != ctx.end() && *it >= '0' && *it <= '9')
    {
        padding_digits = padding_digits * 10 + (*it - '0');
        ++it;
    }

    // Integer presentation
    if (it != ctx.end() && *it != '}')
    {
        switch (*it++)
        {
            case 'b':
                base = 2;
                break;
            case 'B':
                base = 2;
                is_upper = true;
                break;

            case 'c':
                write_as_character = true;
                break;

            case 'o':
                base = 8;
                break;

            case 'd':
                base = 10;
                break;

            case 'x':
                base = 16;
                break;
            case 'X':
                base = 16;
                is_upper = true;
                break;
                // LCOV_EXCL_START
            default:
                BOOST_INT128_THROW_EXCEPTION(std::logic_error("Unsupported format specifier"));
                // LCOV_EXCL_STOP
        }
    }

    // Verify we're at the closing brace
    if (it != ctx.end() && *it != '}')
    {
        BOOST_INT128_THROW_EXCEPTION(std::logic_error("Expected '}' in format string")); // LCOV_EXCL_LINE
    }

    return std::make_tuple(base, padding_digits, sign, is_upper, prefix, write_as_character, character_debug_format, it);
}

template <typename T>
struct is_library_type_impl
{
    static constexpr bool value {std::is_same<T, boost::int128::uint128_t>::value || std::is_same<T, boost::int128::int128_t>::value};
};

template <typename T>
static constexpr bool is_library_type_v = is_library_type_impl<T>::value;

template <typename T>
struct formatter
{
    int base;
    int padding_digits;
    sign_option sign;
    bool is_upper;
    bool prefix;
    bool write_as_character;
    bool character_debug_format;

    constexpr formatter() : base {10},
                            padding_digits {0},
                            sign {sign_option::negative},
                            is_upper {false},
                            prefix {false},
                            write_as_character {false},
                            character_debug_format {false}
    {}

    constexpr auto parse(fmt::format_parse_context& ctx)
    {
        const auto res {boost::int128::fmt_detail::parse_impl(ctx)};

        base = std::get<0>(res);
        padding_digits = std::get<1>(res);
        sign = std::get<2>(res);
        is_upper = std::get<3>(res);
        prefix = std::get<4>(res);
        write_as_character = std::get<5>(res);
        character_debug_format = std::get<6>(res);

        return std::get<7>(res);
    }

    template <typename FormatContext>
    auto format(T v, FormatContext& ctx) const
    {
        char buffer[64];
        bool isneg {false};

        BOOST_INT128_IF_CONSTEXPR (std::is_same<T, boost::int128::int128_t>::value)
        {
            if (v < T{0})
            {
                isneg = true;
                v = -v;
            }
        }

        const auto end = detail::mini_to_chars(buffer, v, base, is_upper);
        std::string s(end, buffer + sizeof(buffer));

        if (s.size() - 1u < static_cast<std::size_t>(padding_digits))
        {
            s.insert(s.begin(), static_cast<std::size_t>(padding_digits) - s.size() + 1u, '0');
        }

        if (prefix)
        {
            switch (base)
            {
                case 2:
                    if (is_upper)
                    {
                        s.insert(s.begin(), 'B');
                    }
                    else
                    {
                        s.insert(s.begin(), 'b');
                    }
                    s.insert(s.begin(), '0');
                    break;
                case 8:
                    s.insert(s.begin(), '0');
                    break;
                case 16:
                    if (is_upper)
                    {
                        s.insert(s.begin(), 'X');
                    }
                    else
                    {
                        s.insert(s.begin(), 'x');
                    }
                    s.insert(s.begin(), '0');
                    break;
                default:
                    // Nothing to do
                    break;
            }
        }

        // Insert our sign
        switch (sign)
        {
            case sign_option::plus:
                if (!isneg)
                {
                    s.insert(s.begin(), '+');
                }
                break;
            case sign_option::space:
                if (!isneg)
                {
                    s.insert(s.begin(), ' ');
                }
                BOOST_INT128_IF_CONSTEXPR (std::is_same<T, boost::int128::int128_t>::value)
                {
                    if (isneg)
                    {
                        s.insert(s.begin(), '-');
                    }
                }
                break;
            case sign_option::negative:
                BOOST_INT128_IF_CONSTEXPR (std::is_same<T, boost::int128::int128_t>::value)
                {
                    if (isneg)
                    {
                        s.insert(s.begin(), '-');
                    }
                }
                break;
                // LCOV_EXCL_START
            default:
                BOOST_INT128_UNREACHABLE;
                // LCOV_EXCL_STOP
        }

        s.erase(0, s.find_first_not_of('\0'));
        s.erase(s.find_last_not_of('\0') + 1);

        return fmt::format_to(ctx.out(), "{}", s);
    }
};

} // namespace fmt_detail
} // namespace int128
} // namespace boost

namespace fmt {

template <>
struct formatter<boost::int128::uint128_t> : public boost::int128::fmt_detail::formatter<boost::int128::uint128_t> {};

template <>
struct formatter<boost::int128::int128_t> : public boost::int128::fmt_detail::formatter<boost::int128::int128_t> {};

} // namespace fmt

#endif // Has format headers

#endif // BOOST_INT128_FMT_FORMAT_HPP
