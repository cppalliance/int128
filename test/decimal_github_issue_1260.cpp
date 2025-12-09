// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
//
// This only failed with GCC 12+ on 32-bit platforms in release mode
// Debug configuration was fine, as was all other platforms

#include <boost/int128.hpp>
#include <boost/core/lightweight_test.hpp>

using namespace boost::int128;

int main()
{
    uint128_t lhs_sig {UINT64_C(54210108624275), UINT64_C(4089650035136921600)};
    uint128_t rhs_sig {UINT64_C(276471553983803), UINT64_C(11633843142343524352)};

    lhs_sig *= 1000U;
    BOOST_TEST_EQ(lhs_sig, uint128_t(UINT64_C(54210108624275221), UINT64_C(12919594847110692864)));

    // 10^31
    rhs_sig /= BOOST_INT128_UINT128_C(10000000000000000000000000000000);
    BOOST_TEST_EQ(rhs_sig, 510U);

    rhs_sig /= 10U;
    BOOST_TEST_EQ(rhs_sig, 51U);

    auto signed_lhs {static_cast<int128_t>(lhs_sig)};
    auto signed_rhs {static_cast<int128_t>(rhs_sig)};
    signed_rhs = -signed_rhs;

    BOOST_TEST_EQ(signed_rhs, int128_t(INT64_C(-1), UINT64_C(18446744073709551565)));

    auto res_sig {static_cast<uint128_t>(signed_lhs + signed_rhs)};
    BOOST_TEST_EQ(res_sig, uint128_t(UINT64_C(54210108624275221), UINT64_C(12919594847110692813)));

    res_sig /= UINT64_C(10);
    BOOST_TEST_EQ(res_sig, uint128_t(UINT64_C(5421010862427522), UINT64_C(3136633892082024442)));

    res_sig /= UINT64_C(10);
    BOOST_TEST_EQ(res_sig, uint128_t(UINT64_C(542101086242752), UINT64_C(4003012203950112767)));

    return boost::report_errors();
}
