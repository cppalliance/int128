// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/int128.hpp>
#include <boost/int128/format.hpp>
#include <boost/core/lightweight_test.hpp>

#ifdef BOOST_INT128_HAS_FORMAT

int main()
{
    return boost::report_errors();
}

#else

int main()
{
    return 0;
}

#endif
