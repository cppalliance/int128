// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef __i386__

#include <absl/numeric/int128.h>

int main()
{
    const absl::uint128 x = 0;
    return static_cast<int>(x);
}

#else

int main()
{
    static_assert(1 == 0, "Does not work on 32-bit linux installed on a 64-bit machine");
    return 1;
}

#endif
