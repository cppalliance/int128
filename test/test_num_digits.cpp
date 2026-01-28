// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128.hpp>
#include <boost/int128/charconv.hpp>
#include <boost/core/lightweight_test.hpp>

void test()
{
    for (int i {}; i < 39; ++i)
    {
        const auto current_val {boost::charconv::detail::int128_pow10[static_cast<std::size_t>(i)]};
        const auto current_val_digits {boost::charconv::detail::num_digits(current_val)};
        BOOST_TEST_EQ(current_val_digits, i + 1);
    }
}

int main()
{
    test();

    return boost::report_errors();
}
