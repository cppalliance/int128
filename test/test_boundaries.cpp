//  Copyright 2026 Matt Borland
//  Distributed under the Boost Software License, Version 1.0.
//  https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128.hpp>
#include <cstdint>
#include <limits>

int main()
{
    /*volatile*/ std::int64_t v {(std::numeric_limits<std::int64_t>::min)()};
    const std::int64_t m {v};
    const boost::int128::int128_t a {42, 1234};
    const boost::int128::int128_t b {0, 5};  // operator/(Signed, int128_t) only negates when rhs.high is 0 or -1

    /*volatile*/ bool sink {};
    sink = static_cast<bool>(a + m);       // int128_imp.hpp:1889  -rhs
    sink = static_cast<bool>(m + a);       // int128_imp.hpp:1895  -lhs
    sink = static_cast<bool>(a / m);       // int128_imp.hpp:2295  rhs < 0 ? -rhs : rhs
    sink = static_cast<bool>(m / b);       // int128_imp.hpp:2324  lhs < 0 ? -lhs : lhs
    static_cast<void>(sink);

    return 0;
}

/*
program returned: 0
Program stderr
/app/raw.githubusercontent.com/Becheler/int128/refs/heads/boost_review/extra/int128_amalgamated.hpp:3355:73: runtime error: negation of -9223372036854775808 cannot be represented in type 'long int'; cast to an unsigned type to negate this value to itself
/app/raw.githubusercontent.com/Becheler/int128/refs/heads/boost_review/extra/int128_amalgamated.hpp:3361:73: runtime error: negation of -9223372036854775808 cannot be represented in type 'long int'; cast to an unsigned type to negate this value to itself
/app/raw.githubusercontent.com/Becheler/int128/refs/heads/boost_review/extra/int128_amalgamated.hpp:3761:16: runtime error: negation of -9223372036854775808 cannot be represented in type 'long int'; cast to an unsigned type to negate this value to itself
/app/raw.githubusercontent.com/Becheler/int128/refs/heads/boost_review/extra/int128_amalgamated.hpp:3790:20: runtime error: negation of -9223372036854775808 cannot be represented in type 'long int'; cast to an unsigned type to negate this value to itself
*/