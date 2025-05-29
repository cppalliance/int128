// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128.hpp>
#include <boost/core/lightweight_test.hpp>

template <typename T>
void test_sat_add()
{
    using boost::int128::add_sat;

    T near_max {std::numeric_limits<T>::max() - T{5}};

    for (T i {0}; i < T{5}; ++i)
    {
        const auto res {add_sat(near_max,  i)};
        BOOST_TEST(res < std::numeric_limits<T>::max());
    }

    near_max += T{5};
    for (T i {0}; i < T{5}; ++i)
    {
        const auto res {add_sat(near_max,  i)};
        BOOST_TEST(res == std::numeric_limits<T>::max());
    }
}

int main()
{
    test_sat_add<boost::int128::uint128_t>();

    return boost::report_errors();
}
