// Copyright 2025 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// On MSVC x64 the division building blocks use the hardware intrinsics _udiv128 (via udiv_2by1)
// and _umul128 (via umul, inside div3by2). This validates that intrinsic path against
// intrinsic-free references on the same inputs: the portable Hacker's Delight divlu, and the
// 32-bit-limb Knuth Algorithm D. Those references are in turn checked against a native 128-bit
// integer on the platforms that have one (see test_div_primitives.cpp), so agreement here pins
// down the MSVC intrinsic wiring specifically.

#include <boost/int128.hpp>
#include <boost/int128/detail/common_div.hpp>
#include <boost/core/lightweight_test.hpp>
#include <random>

#if defined(_M_AMD64) && !defined(__GNUC__) && !defined(__clang__) && _MSC_VER >= 1920

using boost::int128::uint128_t;

static std::mt19937_64 rng{42};
static constexpr std::size_t N{4096U};
static std::uniform_int_distribution<std::uint64_t> dist{UINT64_C(0), UINT64_MAX};

// Independent reference for 128/128 (divisor >= 2^64): the 32-bit-limb Knuth Algorithm D, which
// uses no 64-bit-divide or multiply intrinsics.
static void knuth_oracle(const std::uint64_t uh, const std::uint64_t ul,
                         const std::uint64_t vh, const std::uint64_t vl,
                         uint128_t& quot, uint128_t& rem)
{
    const uint128_t u_val{uh, ul};
    const uint128_t v_val{vh, vl};

    if (u_val < v_val)
    {
        quot = uint128_t{UINT64_C(0)};
        rem = u_val;
        return;
    }

    std::uint32_t u[4]{};
    std::uint32_t v[4]{};
    std::uint32_t q[4]{};

    const auto m{boost::int128::detail::impl::to_words(u_val, u)};
    const auto n{boost::int128::detail::impl::to_words(v_val, v)};

    boost::int128::detail::impl::knuth_divide<true>(u, m, v, n, q);

    quot = boost::int128::detail::impl::from_words<uint128_t>(q);
    rem = boost::int128::detail::impl::from_words<uint128_t>(u);
}

// _udiv128 (udiv_2by1) versus the portable divlu, for 128/64 -> 64.
void test_udiv_2by1()
{
    for (std::size_t i{}; i < N; ++i)
    {
        const auto u0{dist(rng)};
        auto d{dist(rng)};
        if (d == 0)
        {
            d = 1;
        }
        const auto u1{dist(rng) % d}; // precondition u1 < d

        std::uint64_t r_intrin{};
        const auto q_intrin{boost::int128::detail::udiv_2by1(u1, u0, d, r_intrin)};

        std::uint64_t r_soft{};
        const auto q_soft{boost::int128::detail::divlu(u1, u0, d, r_soft)};

        BOOST_TEST_EQ(q_intrin, q_soft);
        BOOST_TEST_EQ(r_intrin, r_soft);
    }
}

static void check_div3by2(const std::uint64_t uh, const std::uint64_t ul,
                          const std::uint64_t vh, const std::uint64_t vl)
{
    std::uint64_t rh{};
    std::uint64_t rl{};
    const auto q{boost::int128::detail::div3by2<true>(uh, ul, vh, vl, rh, rl)};

    uint128_t expected_q{};
    uint128_t expected_r{};
    knuth_oracle(uh, ul, vh, vl, expected_q, expected_r);

    BOOST_TEST_EQ(expected_q.high, UINT64_C(0));
    BOOST_TEST_EQ(q, expected_q.low);
    BOOST_TEST_EQ(uint128_t(rh, rl), expected_r);
}

// _udiv128 + _umul128 (div3by2) versus the 32-bit-limb Knuth reference, for 128/128 -> 64.
void test_div3by2()
{
    for (std::size_t i{}; i < N; ++i)
    {
        auto vh{dist(rng)};
        if (vh == 0)
        {
            vh = 1; // div3by2 requires divisor >= 2^64
        }
        check_div3by2(dist(rng), dist(rng), vh, dist(rng));
    }

    // The widest gap between two-word unsigned values, and the signed-max case the original test
    // exercised, both dividing by exactly 2^64.
    check_div3by2(UINT64_MAX, UINT64_MAX, UINT64_C(1), UINT64_C(0));
    check_div3by2(UINT64_C(0x7FFFFFFFFFFFFFFF), UINT64_MAX, UINT64_C(1), UINT64_C(0));
}

int main()
{
    test_udiv_2by1();
    test_div3by2();

    return boost::report_errors();
}

#else

int main()
{
    return 0;
}

#endif
