// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_FORMAT_HPP
#define BOOST_INT128_FORMAT_HPP

#if (__cplusplus >= 202002L || (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L)) && \
((defined(__GNUC__) && __GNUC__ >= 13) || (defined(__clang__) && __clang_major__ >= 18) || (defined(_MSC_VER) && _MSC_VER >= 1940))

#include <boost/int128/detail/mini_to_chars.hpp>
#include <boost/int128/detail/config.hpp>
#include <boost/int128/int128.hpp>
#include <string>
#include <format>

#define BOOST_INT128_HAS_FORMAT

namespace boost::int128::detail {

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
                BOOST_INT128_THROW_EXCEPTION(std::format_error("Unsupported format specifier"));
            // LCOV_EXCL_STOP
        }
    }

    // Verify we're at the closing brace
    if (it != ctx.end() && *it != '}')
    {
        BOOST_INT128_THROW_EXCEPTION(std::format_error("Expected '}' in format string")); // LCOV_EXCL_LINE
    }

    return std::make_tuple(base, padding_digits, sign, is_upper, prefix, write_as_character, character_debug_format, it);
}

template <typename T>
struct is_library_type_impl
{
    static constexpr bool value {std::is_same_v<T, boost::int128::uint128_t> || std::is_same_v<T, boost::int128::int128_t>};
};

template <typename T>
BOOST_INT128_INLINE_CONSTEXPR bool is_library_type_v = is_library_type_impl<T>::value;

template <typename T>
concept is_library_type = is_library_type_v<T>;

} // namespace boost::int128::detail

namespace std {

template <boost::int128::detail::is_library_type T>
struct formatter<T>
{
    int base;
    int padding_digits;
    boost::int128::detail::sign_option sign;
    bool is_upper;
    bool prefix;
    bool write_as_character;
    bool character_debug_format;

    constexpr formatter() : base {10},
                            padding_digits {0},
                            sign {boost::int128::detail::sign_option::negative},
                            is_upper {false},
                            prefix {false},
                            write_as_character {false},
                            character_debug_format {false}
    {}

    constexpr auto parse(format_parse_context& ctx)
    {
        const auto res {boost::int128::detail::parse_impl(ctx)};

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

        if constexpr (std::is_same_v<T, boost::int128::int128_t>)
        {
            if (v < 0)
            {
                isneg = true;
                v = -v;
            }
        }

        const auto end = boost::int128::detail::mini_to_chars(buffer, v, base, is_upper);
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
            case boost::int128::detail::sign_option::plus:
                if (!isneg)
                {
                    s.insert(s.begin(), '+');
                }
                break;
            case boost::int128::detail::sign_option::space:
                if (!isneg)
                {
                    s.insert(s.begin(), ' ');
                }
                if constexpr (std::is_same_v<T, boost::int128::int128_t>)
                {
                    if (isneg)
                    {
                        s.insert(s.begin(), '-');
                    }
                }
                break;
            case boost::int128::detail::sign_option::negative:
                if constexpr (std::is_same_v<T, boost::int128::int128_t>)
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

        return std::format_to(ctx.out(), "{}", s);
    }
};

} // namespace std

#endif

#endif // BOOST_INT128_FORMAT_HPP
