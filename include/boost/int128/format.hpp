// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_FORMAT_HPP
#define BOOST_INT128_FORMAT_HPP

#if (__cplusplus >= 202002L || (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L)) && \
((defined(__GNUC__) && __GNUC__ >= 13) || (defined(__clang__) && __clang_major__ >= 18) || (defined(_MSC_VER) && _MSC_VER >= 1940))

namespace boost::int128::detail {

enum class sign_option
{
    plus,
    negative,
    space,
    unset
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
    if (it != ctx.end())
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

            default:
                BOOST_INT128_THROW_EXCEPTION(std::format_error("Unsupported format specifier"));
        }
    }

    // Verify we're at the closing brace
    if (it != ctx.end() && *it != '}')
    {
        BOOST_INT128_THROW_EXCEPTION(std::format_error("Expected '}' in format string")); // LCOV_EXCL_LINE
    }

    return std::make_tuple(base, is_upper, padding_digits, sign, prefix, write_as_character, character_debug_format);
}

} // namespace boost::int128::detail

namespace std {

} // namespace std

#endif

#endif // BOOST_INT128_FORMAT_HPP
